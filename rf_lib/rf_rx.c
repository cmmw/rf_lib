/*
 * rf_rx.c
 *
 * Created: 17.02.2016 23:04:57
 * Author : Christian Wagner
 */

#include <stdint.h>
#include <avr/io.h>
#include <stdbool.h>
#include <string.h>

#include "rf_rx.h"
#include "rf_man.h"

enum RX_State
{
    RX_PRE,
    RX_SYNC,
    RX_DATA_ID,
    RX_DATA_LEN,
    RX_DATA,
};

static volatile bool _receive = false;
static uint8_t _samples_min = 3;
static uint8_t _samples_max = 5;
static uint8_t* _buffer;
static uint8_t _buf_size;
static uint8_t _id;
static uint8_t _len;
static volatile uint8_t* _REG;
static uint8_t _PIN;


//Everything after preamble is Manchester encoded except the EOT bits.
//Manchester code as per IEEE 802.3 is used.
//3 byte PREAMBLE | 2 byte len (len is the size of the decoded message) | 2 byte id | 2*len byte message | EOT bits 0x40
//PREAMBLE: 14 '1010...' bits followed by bits '01'  (0xAAAAA9)
//16 bit len, Manchester encoded
//16 bit id, Manchester encoded
//2*len bytes data, Manchester encoded
//end of transmission bits 0x40
//if id is 0xFF it will be ignored and all traffic is returned
void rf_rx_irq()
{
    static uint8_t rx_count;
    static uint8_t rx_last;
    static uint8_t rx_sync_count;
    static enum RX_State rx_state = RX_PRE;
    static uint8_t rx_bits;
    static uint8_t rx_buf_idx;

    if(!_receive)
        return;

    uint8_t rx_sample = (*_REG) & (1 << _PIN);

    rx_count++;
    if(rx_sample == rx_last)
    {
        return;
    }
    //Transition occurred

    //Too short/long
    if(rx_count < _samples_min || rx_count > _samples_max*2)
    {
        rx_sync_count = 0;
        rx_state = RX_PRE;
    }
    else
    {
        //Valid signal
        bool rx_double_bit = rx_count >= _samples_min*2 && rx_count <= _samples_max*2;
        switch (rx_state)
        {
            case RX_PRE:
                if(rx_sample != 0)		//Transition 0 -> 1
                {
                    rx_state = RX_SYNC;
                    rx_sync_count = 0;
                }
                break;

            case RX_SYNC:
                rx_sync_count++;
                if(rx_double_bit)
                {
                    //If a double bit occurred too early or is '11'
                    if(rx_sync_count < 10 || rx_last != 0)
                    {
                        rx_state = RX_PRE;
                    }
                    else if(rx_sync_count >= 10)		//rx_last == 0, received '...001'
                    {
                        rx_state = RX_DATA_ID;
                        _buffer[0] = _buffer[1] = rx_bits = _len = 0;
                    }
                }
                break;

            case RX_DATA_ID:
            case RX_DATA_LEN:
                if(rx_double_bit)
                {
                    if(rx_bits % 2 == 0)
                    {
                        _len <<= 1;
                        if(!rx_last)
                            _len |= 1;
                    }
                    rx_bits++;
                    if(rx_bits == 16)
                    {
                        rx_bits = 0;
                        if(rx_state == RX_DATA_ID)
                        {
                            if(_len != _id && _id != 0xFF)
                            {
                                rx_state = RX_PRE;
                                break;
                            }
                            rx_state = RX_DATA_LEN;
                            _len = 0;
                        }
                        else
                        {
                            rx_buf_idx = 0;
                            rx_state = RX_DATA;
                            if(_buf_size < _len)
                                _len = _buf_size;
                            if(!rx_sample)
                                _buffer[0] |= 1;
                            rx_bits = 1;
                            if(_buf_size < _len)
                                _len = _buf_size;
                            break;
                        }

                    }
                }
                if(rx_bits % 2 == 0)
                {
                    _len <<= 1;
                    if(!rx_sample)
                        _len |= 1;
                }
                rx_bits++;
                if(rx_bits == 16)
                {
                    rx_bits = 0;
                    if(rx_state == RX_DATA_ID)
                    {
                        if(_len != _id && _id != 0xFF)
                        {
                            rx_state = RX_PRE;
                            break;
                        }
                        rx_state = RX_DATA_LEN;
                        _len = 0;
                    }
                    else
                    {
                        rx_buf_idx = 0;
                        rx_state = RX_DATA;
                        if(_buf_size < _len)
                            _len = _buf_size;
                    }
                }
                break;

            case RX_DATA:
                if(rx_double_bit)
                {
                    if(rx_bits % 2 == 0)
                    {
                        _buffer[rx_buf_idx] <<= 1;
                        if(!rx_last)
                            _buffer[rx_buf_idx] |= 1;
                    }
                    rx_bits++;
                    if(rx_bits == 16)
                    {
                        rx_buf_idx++;
                        rx_bits = 0;
                        if(rx_buf_idx == _len)
                        {
                            rx_state = RX_PRE;
                            _receive = false;
                            break;
                        }
                    }
                }

                if(rx_bits % 2 == 0)
                {
                    _buffer[rx_buf_idx] <<= 1;
                    if(!rx_sample)
                        _buffer[rx_buf_idx] |= 1;
                }
                rx_bits++;
                if(rx_bits == 16)
                {
                    rx_buf_idx++;
                    rx_bits = 0;
                    if(rx_buf_idx == _len)
                    {
                        rx_state = RX_PRE;
                        _receive = false;
                    }
                }
                break;
        }
    }

    //Transition occurred, reset counter and store last sample
    rx_count = 0;
    rx_last = rx_sample;
}

void rf_rx_start(void* buffer, uint8_t size, uint8_t samples, uint8_t id)
{
    uint8_t d = samples / 4;
    _samples_min = samples - d;
    _samples_max = samples + d;
    _buffer = (uint8_t*) buffer;
    _buf_size = size;
    _id = id;
    rf_rx_restart();
}

void rf_rx_restart()
{
    memset(_buffer, 0, _buf_size);
    _receive = true;
}

uint8_t rf_rx_done()
{
    if(_receive)
        return 0;
    return _len;
}

uint8_t rf_rx_wait()
{
    while(_receive);
    return _len;
}

void rf_rx_set_io(volatile uint8_t* reg, uint8_t pin)
{
    _REG = reg;
    _PIN = pin;
}
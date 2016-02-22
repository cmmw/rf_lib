/*
 * rf_tx.c
 *
 * Created: 17.02.2016 22:02:53
 * Author : Christian Wagner
 */

#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#define RF_ON (PORTB |= (1 << PORTB3))
#define RF_OFF (PORTB &= ~(1 << PORTB3))

#include <avr/io.h>
#include <util/delay.h>
#include <stddef.h>
#include <string.h>

#include "rf_tx.h"
#include "rf_man.h"

static bool volatile _send = false;
const static uint8_t PREAMBLE[] = {0xAA, 0xAA, 0xA9};
const static uint8_t EOT[] = {0x40};
static uint8_t _tx_bytes;
static uint8_t _tx_id;

static struct _tx_packet_t
{
    const uint8_t* ptr;
    uint8_t size;
    bool encode;
} _tx_packet[] = { {PREAMBLE, sizeof(PREAMBLE), false}, {(const uint8_t*) &_tx_id, sizeof(_tx_id), true}, {(const uint8_t*) &_tx_bytes, sizeof(_tx_bytes), true}, {NULL, 0, true}, {EOT, sizeof(EOT), false} };

void rf_tx_irq()
{
    static uint8_t bit = 0;
    static uint8_t ptr_idx = 0;
    static uint8_t packet_idx = 0;
    static uint8_t enc_flag = 0;
    static uint8_t signal;
    if(_send)
    {
        if(packet_idx < sizeof(_tx_packet) / sizeof(_tx_packet[0]))
        {
            signal = _tx_packet[packet_idx].ptr[ptr_idx] & (0x80 >> bit);
            if(_tx_packet[packet_idx].encode)
            {
                if(enc_flag == 0)
                {
                    enc_flag = 1;
                    if(signal)
                        RF_OFF;
                    else
                        RF_ON;
                }
                else
                {
                    bit++;
                    enc_flag = 0;
                    if(signal)
                        RF_ON;
                    else
                        RF_OFF;
                }
            }
            else
            {
                bit++;
                if(signal)
                    RF_ON;
                else
                    RF_OFF;
            }
            if(bit == 8)
            {
                bit = 0;
                ptr_idx++;
                if(ptr_idx == _tx_packet[packet_idx].size)
                {
                    ptr_idx = 0;
                    packet_idx++;
                }
            }
        }
        else
        {
            RF_OFF;
            packet_idx = 0;
            _send = false;
        }
    }
}


void rf_tx_start(const void* data, uint8_t len, uint8_t id)
{
    _tx_bytes = len;
    _tx_packet[3].ptr = data;
    _tx_packet[3].size = len;
    _tx_id = id;
    _send = true;

}

bool rf_tx_done()
{
    return !_send;
}

void rf_tx_wait()
{
    while(_send);
}

void rf_tx_pulse()
{
    RF_OFF;
    uint8_t high;
    for(uint8_t k = 0; k < sizeof(_tx_packet)/sizeof(_tx_packet[0]); k++)
    {
        for(uint16_t j = 0; j < _tx_packet[k].size; j++)
        {
            uint8_t i = 0;
            bool encode = _tx_packet[k].encode;
            while(i < 8)
            {
                high = _tx_packet[k].ptr[j] & (0x80 >> i);
                if(encode)
                {
                    if(high)
                    {
                        RF_OFF;
                        _delay_us(890);
                        RF_ON;
                    }
                    else
                    {
                        RF_ON;
                        _delay_us(880);
                        RF_OFF;
                    }
                }
                else
                {
                    if(high)
                    {
                        RF_ON;
                    }
                    else
                    {
                        RF_OFF;
                    }
                }
                _delay_us(900);
                i++;
            }
        }
    }
    RF_OFF;
}

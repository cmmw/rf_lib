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
static uint8_t _tx_bytes[2];
static uint8_t _tx_buffer[100];

static struct _tx_packet_t
{
    const uint8_t* ptr;
    uint8_t size;
} _tx_packet[4] = { {PREAMBLE, sizeof(PREAMBLE)}, {(const uint8_t*) &_tx_bytes, sizeof(_tx_bytes)}, {(const uint8_t*) &_tx_buffer, 0}, {EOT, sizeof(EOT)} };

void rf_tx_irq()
{
    static uint8_t bits = 8;
    static uint8_t ptr_idx = 0;
    static uint8_t packet_idx = 0;
    if(_send)
    {
        if(packet_idx < sizeof(_tx_packet) / sizeof(_tx_packet[0]))
        {
            if(ptr_idx < _tx_packet[packet_idx].size)
            {
                if(_tx_packet[packet_idx].ptr[ptr_idx] & (1 << --bits))
                {
                    RF_ON;
                }
                else
                {
                    RF_OFF;
                }
                if(bits == 0)
                {
                    bits = 8;
                    ptr_idx++;
                }
            }
            if(ptr_idx == _tx_packet[packet_idx].size)
            {
                ptr_idx = 0;
                packet_idx++;
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

void rf_tx_start(const void* data, uint8_t len)
{
    len <<= 1;
    len = (len > sizeof(_tx_buffer)) ? sizeof(_tx_buffer) : len;
    memset(_tx_bytes, 0, sizeof(_tx_bytes));
    rf_man_enc(len, _tx_bytes);
    _tx_packet[2].size = len;
    memset(_tx_buffer, 0, sizeof(_tx_buffer));
    len = rf_man_dec (_tx_bytes);
    for(uint8_t i = 0; i < (len >> 1); i++)
    {
        rf_man_enc(((uint8_t*)data)[i], &_tx_buffer[i << 1]);
    }
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
    uint32_t do_send;
    for(uint8_t k = 0; k < sizeof(_tx_packet)/sizeof(_tx_packet[0]); k++)
    {
        for(uint16_t j = 0; j < _tx_packet[k].size; j++)
        {
            for(int8_t i = 7; i >= 0; i--)
            {
                do_send = _tx_packet[k].ptr[j] & (1 << i);
                if(do_send)
                {
                    RF_ON;
                }
                else
                {
                    RF_OFF;
                }
                _delay_us(935);
            }
        }
    }
    RF_OFF;
}

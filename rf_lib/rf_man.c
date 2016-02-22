/*
 * rf_man.c
 *
 * Created: 20.02.2016 12:05:41
 *  Author: Christian Wagner
 */

#include <stdint.h>
#include "rf_man.h"

void rf_man_enc(const uint8_t data, uint8_t encoded[2])
{
    for(uint8_t i = 0; i < 8; i++)
    {
        uint8_t idx = (i >> 2);
        encoded[idx] <<= 1;
        if((data << i) & 0x80)
        {
            encoded[idx] <<= 1;
            encoded[idx] |= 1;
        }
        else
        {
            encoded[idx] |= 1;
            encoded[idx] <<= 1;
        }
    }
}


uint8_t rf_man_dec(const uint8_t data[2])
{
    uint8_t byte = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        uint8_t idx = (i >> 2);
        byte <<= 1;
        if(!((data[idx] << (2 * (i - (i & 0x4)))) & 0x80))
            byte |= 1;
    }
    return byte;
}
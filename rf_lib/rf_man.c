/*
 * rf_man.c
 *
 * Created: 20.02.2016 12:05:41
 *  Author: Christian
 */

#include <stdint.h>
#include "rf_man.h"

void rf_man_enc(uint8_t data, uint8_t encoded[2])
{
    encoded[0] = 0;
    encoded[1] = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        uint8_t idx = (i >> 2);
        encoded[idx] <<= 1;
        if(data & 0x80)
        {
            encoded[idx] <<= 1;
            encoded[idx] |= 1;
        }
        else
        {
            encoded[idx] |= 1;
            encoded[idx] <<= 1;
        }
        data <<= 1;
    }
}


uint8_t rf_man_dec(uint8_t data[2])
{
    uint8_t byte = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        uint8_t idx = (i >> 2);
        byte <<= 1;
        if(!(data[idx] & 0x80))
            byte |= 1;
        data[idx] <<= 2;
    }
    return byte;
}
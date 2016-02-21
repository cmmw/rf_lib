/*
 * rf_man.c
 *
 * Created: 20.02.2016 12:05:41
 *  Author: Christian
 */

#include <stdint.h>
#include "rf_man.h"

uint16_t rf_man_enc(uint8_t data)
{
    uint16_t ret = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        ret <<= 1;
        if(data & 0x80)
        {
            ret <<= 1;
            ret |= 1;
        }
        else
        {
            ret |= 1;
            ret <<= 1;
        }
        data <<= 1;
    }
    return ret;
}


uint8_t rf_man_dec(uint16_t data)
{
    uint8_t byte = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        byte <<= 1;
        if(!(data & 0x8000))
            byte |= 1;
        data <<= 2;
    }
    return byte;
}
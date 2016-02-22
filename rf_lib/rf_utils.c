/*
 * rf_utils.c
 *
 * Created: 22.02.2016 02:56:26
 *  Author: Christian Wagner
 */

#include "rf_utils.h"
static uint16_t swap16(uint16_t);
static uint32_t swap32(uint32_t);

uint16_t conv16(uint16_t var)
{
    if(is_little_endian())
        var = swap16(var);
    return var;
}

uint16_t swap16(uint16_t var)
{
    return ((var & 0xFF00) >> 8) | (var << 8);
}

uint32_t conv32(uint32_t var)
{
    if(is_little_endian())
        return swap32(var);
    return var;
}

uint32_t swap32(uint32_t var)
{
    return ((var & 0xFF000000) >> 24) | ((var & 0x00FF0000) >> 8) | ((var & 0x0000FF00) << 8) | (var << 24);
}

bool is_little_endian()
{
    static uint16_t var = 0x0001;
    return ((uint8_t*) &var)[0];
}
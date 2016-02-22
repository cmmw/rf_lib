/*
 * rf_utils.h
 *
 * Created: 22.02.2016 02:56:35
 *  Author: Christian Wagner
 */


#ifndef RF_UTILS_H_
#define RF_UTILS_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//Functions for converting 16 and 32 bit data type from network to host endian order and back
#define hton16(x) conv16(x)
#define ntoh16(x) conv16(x)
#define hton32(x) conv32(x)
#define ntoh32(x) conv32(x)

uint16_t conv16(uint16_t var);
uint32_t conv32(uint32_t var);

//Find out what endian the controller uses
bool is_little_endian();



#ifdef __cplusplus
}
#endif

#endif /* RF_UTILS_H_ */
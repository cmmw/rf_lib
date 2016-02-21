/*
 * rf_man.h
 *
 * Created: 20.02.2016 12:06:45
 *  Author: Christian Wagner
 */


#ifndef RF_MAN_H_
#define RF_MAN_H_


#ifdef __cplusplus
extern "C" {
#endif

uint16_t rf_man_enc(uint8_t data);
uint8_t rf_man_dec(uint16_t data);


#ifdef __cplusplus
}
#endif

#endif /* RF_MAN_H_ */
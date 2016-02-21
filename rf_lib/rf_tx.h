/*
 * rf_tx.h
 *
 * Created: 20.02.2016 11:55:08
 *  Author: Christian
 */


#ifndef SEND_H_
#define SEND_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void rf_tx_irq();
void rf_tx_start(uint8_t* data, uint8_t len);
bool rf_tx_done();
void rf_tx_wait();
void rf_tx_pulse();

#ifdef __cplusplus
}
#endif

#endif /* SEND_H_ */
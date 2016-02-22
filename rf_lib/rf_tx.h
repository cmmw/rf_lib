/*
 * rf_tx.h
 *
 * Created: 20.02.2016 11:55:08
 *  Author: Christian Wagner
 */


#ifndef SEND_H_
#define SEND_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void rf_tx_irq();
void rf_tx_start(const void* data, uint8_t len, uint8_t id);
void rf_tx_restart();
bool rf_tx_done();
void rf_tx_wait();
void rf_tx_set_io(volatile uint8_t* reg, uint8_t pin);
void rf_tx_pulse();

#ifdef __cplusplus
}
#endif

#endif /* SEND_H_ */
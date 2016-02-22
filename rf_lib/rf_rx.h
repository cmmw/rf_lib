/*
 * rf_rx.h
 *
 * Created: 20.02.2016 11:56:18
 *  Author: Christian Wagner
 */


#ifndef RECEIVE_H_
#define RECEIVE_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void rf_rx_irq();
void rf_rx_start(void* buffer, uint8_t size, uint8_t samples, uint8_t id);
void rf_rx_restart();
bool rf_rx_done();
void rf_rx_wait();

#ifdef __cplusplus
}
#endif

#endif /* RECEIVE_H_ */
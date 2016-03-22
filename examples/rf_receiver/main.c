/*
 * rf_receiver.c
 *
 * Created: 20.02.2016 13:42:04
 * Author : Christian Wagner
 */


#define F_CPU 1000000UL
#define _SAMPLES 4

#define LED_ON (PORTB |= (1 << PORTB4))
#define LED_SWITCH (PORTB ^= (1 << PORTB4))
#define LED_OFF (PORTB &= ~(1 << PORTB4))

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include <rf_rx.h>


ISR(TIMER0_COMPA_vect)
{
    rf_rx_irq();
}

int main(void)
{
    DDRB &= ~(1 << DDB3);	//RF Receiver
    PORTB |= (1 << PB3);	//Pull-up
    DDRB |= (1 << DDB4)	;	//LED

    TCCR0A |= (1 << WGM01);		//CTC Mode
    TCCR0B |= (1 << CS00);		//No prescale
    TIMSK |= (1 << OCIE0A);		//Enable Compare Match A interrupt
    //1000 Hz = (F_CPU / 1000 / _SAMPLES_PER_BIT) - 1 -> 249
    OCR0A = (F_CPU / 1000.0 / _SAMPLES) - 1;
    OCR0A = 249;

    /**Unrelated power savings**/
    //Turn off analog comparator to save power
    ACSR |= (1 << ACD);
    /***************************/

    sei();

    char* ON = "TURN ME ON";
    char* OFF = "TURN ME OFF";

    uint8_t data[120];
    rf_rx_set_io(&PINB, PINB3);
    rf_rx_start(data, sizeof(data), 4, 0x09);
    while (1)
    {
        rf_rx_wait();
        if(memcmp(data, ON, strlen(ON)) == 0)
        {
            LED_ON;
        }
        else if(memcmp(data, OFF, strlen(OFF)) == 0)
        {
            LED_OFF;
        }
        rf_rx_restart();
    }
}
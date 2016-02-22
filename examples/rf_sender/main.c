/*
 * rf_sender.c
 *
 * Created: 20.02.2016 11:46:21
 * Author : Christian Wagner
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <rf_tx.h>
#include <rf_man.h>
#include <string.h>

ISR(TIMER0_COMPA_vect)
{
    rf_tx_irq();
}

int main(void)
{
    DDRB &= ~(1 << DDB2 | 1 << DDB4);		//Buttons
    PORTB |= (1 << PB2 | 1 << PB4);			//Pull-ups
    DDRB |= (1 << DDB3);		//RF-Sender

    //Set up interrupt for 1000 bit transmission per second
    TCCR0A |= (1 << WGM01);		//CTC Mode
    TCCR0B |= (1 << CS01);		//prescale 8
    //     TIMSK |= (1 << OCIE0A);		//Enable Compare Match A interrupt
    //1000 Hz = (F_CPU / 1000 / prescale=8) - 1 -> 124
    OCR0A = 124;
    sei();
    while (1)
    {
        static uint8_t ON[] = {'O', 'N'};
        static uint8_t OFF[] = {'O', 'F', 'F'};
        if(!(PINB & (1 << PINB2)))
        {
            rf_tx_start(ON, sizeof(ON));
//             send_pulse();
            TIMSK |= (1 << OCIE0A);			//Enable Compare Match A interrupt
            rf_tx_wait();
            TIMSK &= ~(1 << OCIE0A);		//Disable Compare Match A interrupt
        }
        else if(!(PINB & (1 << PINB4)))
        {
            rf_tx_start(OFF, sizeof(OFF));
//             send_pulse();
            TIMSK |= (1 << OCIE0A);			//Enable Compare Match A interrupt
            rf_tx_wait();
            TIMSK &= ~(1 << OCIE0A);		//Disable Compare Match A interrupt
        }
    }
}

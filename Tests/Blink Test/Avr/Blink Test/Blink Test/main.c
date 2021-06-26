 /* *********************************** */
 /*            Blink Test.c             */
 /*                                     */
 /*     Created: 6/26/2021 2:50:54 PM   */
 /*           Author : rng70            */
 /* *********************************** */ 
#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    /* Set Port B for output */
	DDRB |= (1 << PINB0);
	
    while (1) 
    {
		PORTB |= (1 << PINB0);
		_delay_ms(1000);
		PORTB &= ~(1 << PINB0);
		_delay_ms(1000);
    }
}


 /* *********************************** */
 /*            Blink Test.c             */
 /*                                     */
 /*     Created: 6/26/2021 2:50:54 PM   */
 /*           Author : rng70            */
 /* *********************************** */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    /* Set Port B for output */
	DDRD |= (1 << PIND6);
	
    while (1) 
    {
		PORTD |= (1 << PIND6);
		_delay_ms(50);
		PORTD &= ~(1 << PIND6);
		_delay_ms(50);
    }
}


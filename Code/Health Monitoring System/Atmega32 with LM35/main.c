/*
 * GccApplication3.c
 *
 * Created: 7/16/2021 12:41:41 PM
 * Author : my
 */ 
#define F_CPU 1000000
#include<avr/io.h>
#include<avr/interrupt.h>
#include <util/delay.h>
volatile unsigned char result;
ISR(ADC_vect)
{
	
	result=ADCH;
	
}
void UART_init(void){
	// Normal speed, disable multi-proc
	UCSRA = 0b00000000;
	// Enable Tx and Rx, disable interrupts
	UCSRB = 0b00001000;
	// Asynchronous mode, no parity, 1 stop bit, 8 data bits
	UCSRC = 0b10000110;
	// Baud rate 1200bps, assuming 1MHz clock
	UBRRL = 0x12;
	UBRRH = 0x00;
}
void UART_send(unsigned char data){
	// wait until UDRE flag is set to logic 1
	while ((UCSRA & (1<<UDRE)) == 0x00);
	UDR = data; // Write character to UDR for 				 transmission
}

int main(void)
{
   ADCSRA=0b10001101;
   ADMUX=0b11100000;
   UART_init();
   sei();
   
    while (1) 
    {
		ADCSRA|=1<<ADSC;
		UART_send(result);
    }
}


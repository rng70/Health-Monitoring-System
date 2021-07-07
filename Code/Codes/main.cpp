#define F_CPU 16000000
#include<avr/io.h>
#include<avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7
#define DHT11_PIN 0
#include "lcd.h"
uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;
char buffer[20];

uint8_t Receive_data()			/* receive data */
{	
	for (int q=0; q<8; q++)
	{
		while((PINB & (1<<DHT11_PIN)) == 0);  /* check received bit 0 or 1 */
		_delay_us(30);
		if(PINB & (1<<DHT11_PIN))/* if high pulse is greater than 30ms */
		    c = (c<<1)|(0x01);	/* then its logic HIGH */
		else			/* otherwise its logic LOW */
		    c = (c<<1);
		while(PINB & (1<<DHT11_PIN));
	}
	return c;
}

int main(void)
{
uint16_t bin;
char temp_data[5];
float body_temp,room_temp;
DDRD = 0xFF;
DDRC = 0xFF;

ADCSRA=0b10000101;
Lcd4_Init();
while(1)
{
    //lm35
    ADMUX=0b11000000;
    ADCSRA|=1<<ADSC;
    while(ADCSRA &(1<<ADSC))
    {}
    uint8_t bin1=ADCL;
    bin=(ADCH<<8|bin1);
    body_temp=bin*0.0025*100.0;
    //dht11
    DDRB |= (1<<DHT11_PIN);
	PORTB &= ~(1<<DHT11_PIN);	/* set to low pin */
	_delay_ms(20);			/* wait for 20ms */
	PORTB |= (1<<DHT11_PIN);
    DDRB &= ~(1<<DHT11_PIN);
	while(PINB & (1<<DHT11_PIN));
	while((PINB & (1<<DHT11_PIN))==0);
	while(PINB & (1<<DHT11_PIN));
    I_RH=Receive_data();	/* store first eight bit in I_RH */
	D_RH=Receive_data();	/* store next eight bit in D_RH */
	I_Temp=Receive_data();	/* store next eight bit in I_Temp */
	D_Temp=Receive_data();	/* store next eight bit in D_Temp */
	CheckSum=Receive_data();/* store next eight bit in CheckSum */
    itoa(I_Temp,temp_data,10); //temp data=temperature
    //
    Lcd4_Set_Cursor(1,1);
    //Lcd4_Write_String("Body Temp ");
    dtostrf(body_temp, 1, 0, buffer);
    Lcd4_Write_String("BT ");
    Lcd4_Write_String(buffer);
    Lcd4_Write_String(" RT ");
    Lcd4_Write_String(temp_data);
    _delay_ms(500);
    

}
}
//LM35 connected with A0 pin and DHT1 with B0 pin

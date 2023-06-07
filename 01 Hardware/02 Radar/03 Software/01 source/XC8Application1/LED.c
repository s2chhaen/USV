/*
 * LED.c
 *
 * Created: 20.03.2023 13:37:44
 *  Author: Admin
 */ 

#include <xc.h>
#include <avr/io.h>
#include <stdio.h>

/*
led_config()
LED-Pin als Ausgang setzen
*/
void led_config(void)
{
	PORTA.DIR |= (0b00001000);			//(1<<PA3);
}

/*
led_ein()
LED einschalten, indem Pin auf Low gezogen wird
*/
void led_ein(void)
{
	PORTA.OUT &= ~(0b00001000);			//(1<<PA3);
}

/*
led_aus()
LED ausschalten, indem Pin auf High gezogen wird
*/
void led_aus(void)
{
	PORTA.OUT |= (0b00001000);			//(1<<PA3);
}

/*
led_tgl()
LED umschalten
*/
void led_tgl(void)
{
	PORTA.OUT ^= (0b00001000);			//(1<<PA3);
}
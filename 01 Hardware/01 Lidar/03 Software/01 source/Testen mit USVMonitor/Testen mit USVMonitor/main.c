/*
 * Datenframe testen.c
 *
 * Created: 7/6/2023 4:59:08 PM
 * Author : Thach
 */ 

#include <avr/io.h>
#include "main.h"


int main(void)
{
	//Systemclock init
	uint8_t prescalerClk=1;
	init_Core_CLK(INTERN_CLK,prescalerClk);
	uint8_t timerResolutionUs = 1;
	//Stopuhr init
	uint16_t timerPrescaler = 1024;
	timerInit(timerResolutionUs,timerPrescaler);
	//Benutzereinheit init
	usartConfig_t config= {
		.usartNo= iUSART0,
		.baudrate = 250000,
		.usartChSize = USART_CHSIZE_8BIT_gc,
		.parity = USART_PMODE_ODD_gc,
		.stopbit = USART_SBMODE_1BIT_gc,
		.sync = false,
		.mpcm = false,
		.address = 0,
		.portMux = PORTMUX_USARTx_DEFAULT_gc,
	};
	initUserUnit(config);
	//User-Unit and Slave API
	usvMonitorHandler_t handler;
	initDev(&handler,usartDataRx,usartDataTx,waitUs,0xD5);
	/** Testbeginn **/
	//Anfrage des GESB
	uint8_t add = 0;
	uint8_t reg = SEN_GESB_ADD;
	uint8_t output[10];
	uint8_t rxLen = 1;
	getData(add,reg,&handler,output,rxLen);
	waitUs(100);
	reg = SEN_LONGNITUDE_ADD;
	rxLen = 4;
	getData(add,reg,&handler,output,rxLen);
    /* Replace with your application code */
    while (1) 
    {
    }
}


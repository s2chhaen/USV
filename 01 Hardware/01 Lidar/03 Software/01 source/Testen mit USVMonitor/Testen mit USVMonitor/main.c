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
	//Stopuhr init
	//uint8_t timerResolutionUs = 1;
	//uint16_t timerPrescaler = 1024;
	//timerInit(timerResolutionUs,timerPrescaler);
	//Benutzereinheit init
	usartConfig_t config= {
		.usartNo= iUSART1,
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
	//volatile bool a = USART_init(iUSART1,250000,USART_CHSIZE_8BIT_gc,USART_PMODE_ODD_gc,USART_SBMODE_1BIT_gc,0,0,0,PORTMUX_USARTx_DEFAULT_gc);
	//USART_set_send_Array_callback_fnc(iUSART1,&usartCallbackTx1);
	//USART_set_receive_Array_callback_fnc(iUSART1,&usartCallbackRx1);
	//USART_set_Bytes_to_receive(iUSART1,1);
	sei();
	/** Testbeginn **/
#ifdef TEST01
	//Anfrage des GESB
	//uint8_t add = 0;
	//uint8_t reg = SEN_GESB_ADD;
	//uint8_t output[10];
	//uint8_t rxLen = 1;
	//getData(add,reg,&handler,output,rxLen);
	//waitUs(100);
	//reg = SEN_LONGNITUDE_ADD;
	//rxLen = 4;
	//getData(add,reg,&handler,output,rxLen);
#endif
#define TEST02 1

#ifdef TEST02
#define LENGTH_TEST 62
	uint8_t test1[LENGTH_TEST]={0};
	for (uint8_t i = 0; i<LENGTH_TEST;i++){
		test1[i] = i;
	}
	uint8_t length = LENGTH_TEST;
	(*(handler.transmitFunc_p))(test1,length);
	//USART_send_Array(iUSART1,0,test1,length);
	//a = 0;
#endif
    /* Replace with your application code */
    while (1) 
    {
		//a++;
    }
}


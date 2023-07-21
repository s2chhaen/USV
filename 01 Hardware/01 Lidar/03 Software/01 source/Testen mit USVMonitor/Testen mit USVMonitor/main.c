/*
 * Datenframe testen.c
 *
 * Created: 7/6/2023 4:59:08 PM
 * Author : Thach
 */ 

#include <avr/io.h>
#include "main.h"

#define ERR1 0
#define ERR2 1
#define ON 1
#define OFF 0
#define INPUT_DISABLE 0x04

//verhindern Reset bei falschen Interrupt
EMPTY_INTERRUPT(BADISR_vect)

static void ioInit(){
	PORTD.DIR |= (1<<ERR1);
	PORTD.PIN0CTRL |= (INPUT_DISABLE<<0);
}

static void setErr1State(uint8_t state){
	switch(state){
		case OFF:
			PORTD.OUT &= ~(1<<ERR1);
			break;
		case ON:
			PORTD.OUT |= (1<<ERR1);
			break;
		default:
			PORTD.OUT &= ~(1<<ERR1);
			break;	
	};
}

volatile uint8_t output[500]={0};
	
int main(void)
{
	/************************************************************************/
	/* Initialisierung aller notwendigen Module                                                                     */
	/************************************************************************/
	//Systemclock init
	uint8_t prescalerClk=1;
	init_Core_CLK(INTERN_CLK,prescalerClk);
	//Stopuhr init
	uint8_t timerResolutionUs = 1;
	uint16_t timerPrescaler = 1024;
	timerInit(timerResolutionUs,timerPrescaler);
	//IO-Pin für Fehleranzeige Init
	
	ioInit();
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
	setErr1State(OFF);
	//User-Unit and Slave API Handler Init
	usvMonitorHandler_t handler;
	initDev(&handler,usartDataRx,usartDataTx,waitCycle,0xD5);
	sei();//globales Interrupt aktiviert
	
	/************************************************************************/
	/* Testbeginn                                                                     */
	/************************************************************************/
	
//#define  LED_ERR1_TEST 1
#ifdef LED_ERR1_TEST
	setErr1State(ON);
#endif	
	
	volatile uint8_t error1 = NO_ERROR;
	const uint8_t add = 1;
	uint16_t reg = 0;
	uint8_t rxLen = 0;
	
//#define TEST01 1

#ifdef TEST01
	//Lesen in Registern
	reg = SEN_COURSE_ANGLE_ADD;
	rxLen = 2;
	error1 = getData(add,reg,&handler,(uint8_t*)output,rxLen);
	waitUs(5);
	if (error1!=NO_ERROR){
		setErr1State(ON);
	} else{
		setErr1State(OFF);
	}
#endif

//#define TEST02 1

#ifdef TEST02
	//Schreiben in einem Register
	uint8_t input[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	reg = REF_DRV_CTRL_REF_A_ADD;	
	rxLen = 8;
	error1 = setData(add,reg,&handler,(uint8_t*)input, rxLen);
	if (error1!=NO_ERROR){
		setErr1State(ON);
	} else {
		setErr1State(OFF);
	}
#endif

#define TEST03 1

#ifdef TEST03
	//Multiregister schreiben und lesen
	uint8_t input[]={0,1,2,3,4,5,6,7,8,9,10,11,12,14,15,16,17,18,19,20};
	reg = REF_DRV_CTRL_REF_A_ADD;
	rxLen = 20;
	error1 = setMultiregister(add,reg,&handler,(uint8_t*)input, rxLen);
	if (error1!=NO_ERROR){
		setErr1State(ON);
	} else {
		setErr1State(OFF);
	}
	error1 = getMultiregister(add,reg,&handler,(uint8_t*)output, rxLen);
	if (error1!=NO_ERROR){
		setErr1State(ON);
	} else {
		setErr1State(OFF);
	}
#endif
    /* Replace with your application code */
    while (1){
    }
}


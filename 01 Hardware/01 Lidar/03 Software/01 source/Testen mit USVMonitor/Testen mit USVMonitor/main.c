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

volatile uint8_t answer[9]={0};
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
	//waitUs(100);
	//User-Unit and Slave API Handler Init
	usvMonitorHandler_t handler;
	//waitCycle
	initDev(&handler,usartDataRx,usartDataTx,waitCycle,0xD5);
	sei();//globales Interrupt aktiviert
	
	/************************************************************************/
	/* Testbeginn                                                                     */
	/************************************************************************/
	
//#define  LED_ERR1_TEST 1
#ifdef LED_ERR1_TEST
	setErr1State(ON);
#endif	
	
#define TEST01 1
	
#ifdef TEST01
	volatile uint8_t error1 = NO_ERROR;
	//Lesen in Registern
	const uint8_t add = 1;
	uint16_t reg = 0;
	uint8_t rxLen = 0;
	
	//reg = SEN_COURSE_ANGLE_ADD;
	//rxLen = 2;
	//reg = SEN_GESB_ADD;
	//rxLen = 1;
	//error1 = getData(add,reg,&handler,output,rxLen);
	//waitUs(5);
	//error1 = TIME_OUT;
	if (error1!=NO_ERROR){
		setErr1State(ON);
		//Warte 1min
		for (int i = 0;i<60;i++){
			waitUs(1000000);
		}
		setErr1State(OFF);
	} else{
		setErr1State(OFF);
		//reg = SEN_LONGNITUDE_ADD;
		//rxLen = 4;
		//getData(add,reg,&handler,output,rxLen);
		//waitUs(5);
		//reg = SEN_LATITUDE_ADD;
		//rxLen = 4;
		//getData(add,reg,&handler,output,rxLen);
		//waitUs(5);
		//reg = SEN_SATFIX_ADD;
		//rxLen = 1;
		//getData(add,reg,&handler,output,rxLen);
		//waitUs(5);
		//reg = SEN_GPS_VEL_ADD;
		//rxLen = 2;
		//getData(add,reg,&handler,output,rxLen);
		//waitUs(5);
		//reg = SEN_COURSE_ANGLE_ADD;
		//rxLen = 2;
		//getData(add,reg,&handler,output,rxLen);
		//waitUs(5);
		//reg = SEN_TIMESTAMP_ADD;
		//rxLen = 3;
		//getData(add,reg,&handler,output,rxLen);
		//waitUs(5);
		//reg = SEN_GESB_ADD;
		//rxLen= 1;
		//getData(add,reg,&handler,output,rxLen);
		//waitUs(5);
	}
	//Schreiben in Registern
	//uint8_t input[20]={1,2};
	//uint8_t txLen = 2;
	//reg = REF_DRV_CTRL_VEL_ADD;
	//error1 = setData(add,reg,&handler,input,txLen);
	//if (error1!=NO_ERROR){
		//setErr1State(ON);
		//for (int i = 0;i<10;i++){
			//waitUs(1000000);
		//}
		
		//setErr1State(OFF);
	//}
	//reg = SEN_GESB_ADD;
	//rxLen = 17;
	reg = REF_DRV_CTRL_REF_A_ADD;
	rxLen = 21;
	error1 = getMultiregister(add,reg,&handler,(uint8_t*)output, rxLen);
	if (error1!=NO_ERROR){
		setErr1State(ON);
	} else {
		setErr1State(OFF);
	}
	
#endif

//#define TEST02 1

#ifdef TEST02
//#define LENGTH_TEST 62
	//uint8_t test1[LENGTH_TEST]={0};
	//for (uint8_t i = 0; i<LENGTH_TEST;i++){
		//test1[i] = i;
	//}
	uint8_t dataSet[]={0xA5,0x01,0x0C,0x40,0x08,0x02,0x7F,0xA6};
	//uint8_t dataSet[]={0xA5,0x00,0x0C,0x40,0x08,0x02, 0x7F,0xA6};
	uint8_t length = sizeof(dataSet)/sizeof(uint8_t);
	
	(*(handler.transmitFunc_p))(dataSet,length);
	waitUs(800);
	length = 9;
	(*(handler.receiveFunc_p))((uint8_t*)answer,length);
	//USART_send_Array(iUSART1,0,test1,length);
	uint8_t test111 = 0;
#endif
    /* Replace with your application code */
    while (1){
		//test111++;
    }
}


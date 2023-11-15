/*
 * main.c
 *
 * Created: 7/6/2023 4:59:08 PM
 * Author : Thach
 * Version: 1.0
 * Revision: 1.1
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

/************************************************************************/
/* Definition des Macros für das Test                                                                     */
/************************************************************************/

//Einschalten des LEDs zum Testen
//#define  LED_ERR1_TEST 1
//Lesen eines Registers
//#define READ_ONE_REGISTER 1
//Schreiben in eines Register
//#define WRITE_IN_ONE_REGISTER 1
//Lesen Multiregister
//#define READ_MULTIREGISTER 1
//Echo (Schreiben und dann Lesen Multiregister)
#define WRITE_AND_READ_MULTI_REGISTER 1


/**
 * \brief Initialisation des I/O-Pin D0 als Ausgabe
 * 
 * 
 * \return void
 */
static void ioInit(){
	PORTD.DIR |= (1<<ERR1);
	PORTD.PIN0CTRL |= (INPUT_DISABLE<<0);
}

/**
 * \brief Ein- oder Auschalten des LEDs
 * 
 * \param state der Zustand von LED (0:Aus, sonst: Ein)
 * 
 * \return void
 */
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
	
/**
 * \brief Vergleich der zwei Zeichenfolge
 * 
 * \param str1_p der Zeiger zur ersten Zeichenfolge
 * \param lenStr1 die Länge der ersten Zeichenfolge
 * \param str2_p der Zeiger zur zweiten Zeichenfolge
 * \param lenStr2 die Länge der zweiten Zeichenfolge
 * 
 * \return uint8_t 1: gleich, 0:ungleich
 */
uint8_t stringCmp(uint8_t* str1_p, uint16_t lenStr1, uint8_t* str2_p, uint16_t lenStr2){
	uint8_t result = 1;
	if (lenStr1==lenStr2){
		for (uint16_t i = 0; i<lenStr1; i++){
			if (str1_p[i]!=str2_p[i]){
				result = 0;
				break;
			}
		}
	} else{
		result = 0;
	}
	return result;
}

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
	

#ifdef LED_ERR1_TEST
	setErr1State(ON);
#endif	
	
	volatile uint8_t error1 = NO_ERROR;
	const uint8_t add = 1;
	uint16_t reg = 0;
	uint16_t rxLen = 0;
	volatile uint8_t output[500] = {0};
	
#ifdef READ_ONE_REGISTER
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

#ifdef WRITE_IN_ONE_REGISTER
	//Schreiben in einem Register
	uint8_t input1[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	reg = REF_DRV_CTRL_REF_A_ADD;	
	rxLen = 8;
	error1 = setData(add,reg,&handler,(uint8_t*)input1, rxLen);
	if (error1!=NO_ERROR){
		setErr1State(ON);
	} else {
		setErr1State(OFF);
	}
#endif

#ifdef READ_MULTIREGISTER
#define BUFFER_3_LEN 361
	volatile uint8_t outputBuffer3[BUFFER_3_LEN] = {0};
#ifdef WRITE_AND_READ_MULTI_REGISTER
#define INPUT_2_LEN 20
	//Multiregister schreiben und lesen
	volatile uint8_t input2[INPUT_2_LEN] = {0};
	//memset((uint8_t*)input2,0x14,INPUT_2_LEN);
	for (int i = 0; i<INPUT_2_LEN;i++)
	{
		input2[i] = i;
	}
	reg = LIDAR_VALUE_ADD;
	rxLen = BUFFER_3_LEN;
	error1 = getMultiregister(add,reg,&handler,(uint8_t*)outputBuffer3,rxLen);
	__asm__("nop");
	if (error1==NO_ERROR){
		setErr1State(ON);
	} else{
		setErr1State(OFF);
	}
#endif //READ_MULTIREGISTER

		setErr1State(ON);
	} else {
		setErr1State(OFF);
	}
	
	error1 = stringCmp((uint8_t*)input2,rxLen,(uint8_t*)output,rxLen);
	if (error1==0){
		setErr1State(ON);
	} else {
		setErr1State(OFF);
	}
#endif

    /* Replace with your application code */
    while (1){
    }
}


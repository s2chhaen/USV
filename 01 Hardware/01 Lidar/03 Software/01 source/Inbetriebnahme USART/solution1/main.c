/*
 * solution1.c
 *
 * Created: 11.03.2023 16:31:29
 * Author : Thach
 */ 


#include "main.h"

/************************************************************************/
/* Definition der Macros für Testversuche                                                                     */
/************************************************************************/
//Echo-Test
#define ACTIVE_ECHO_TEST 1
//Test der Sendefunktion von slaveDevice.c
#define ACTIVE_USER_DEFINED_SEND_FUNCTION_TEST 1
//Test der Sendefunktion von HAL-Bibliothek
#define ACTIVE_HAL_USART_TEST 1


/**
 * \brief Initalisierung für das ganze Programm
 * \return uint8_t 0:Fehlerfrei, sonst Fehler
 */
static uint8_t init(){
	uint8_t result;
	uint8_t prescaler = 1;
	uint8_t timerResUs = 1;
	uint16_t timerPre = 1024;
	uint32_t baudrateSlave = 250000;
	//CPU-Init
	init_Core_CLK(INTERN_CLK,prescaler);
	//USART-Beobachter- und Stopuhr-Einheit-Init
	timerInit(timerResUs,timerPre);
	//Slave-Device-Init
	result = initDev(iUSART1, baudrateSlave, USART_CHSIZE_8BIT_gc, USART_PMODE_ODD_gc, USART_SBMODE_1BIT_gc, DISBL_MPC_MODE, DISBL_MPC_MODE, 0, PORTMUX_USARTx_DEFAULT_gc);
	sei();//aktiviert das global Interrupt, weil es bisher nicht aktiviert wird
	return result;
}

#ifdef ACTIVE_ECHO_TEST
#define DATA_LENGTH 100
static uint8_t data[DATA_LENGTH]={0};
static uint16_t rxLength = sizeof(data)/sizeof(uint8_t);

/**
 * \brief empfangen eine Zeichenfolge mit beliebiger Länge und schicke sie mit ihrer Länge zurück 
 * \return void
 */
static void echoTest(){
	volatile bool checkReceive = dataRx((uint8_t*)data,(uint16_t*)&rxLength)==NO_ERROR;
	volatile uint16_t temp1 = 48;
	volatile uint16_t temp2 = 48;
	if(checkReceive){
		dataTx((uint8_t*)data,(uint16_t)rxLength);
		waitUs(1000);
		temp1+=rxLength/10;
		temp2+=rxLength%10;
		data[0]=temp1;
		data[1]=temp2;
		dataTx((uint8_t*)data,2);
		rxLength = sizeof(data)/sizeof(uint8_t);
		memset((uint8_t*)data,0,rxLength);
	}
}
#endif

int main(void) {
	init();
#ifdef ACTIVE_HAL_USART_TEST
#define MAX_VALUE_TEST_1 31
	uint8_t test1[MAX_VALUE_TEST_1]={0};
	for (int i = 0; i < MAX_VALUE_TEST_1; i++){
		test1[i] = i;
	}
	uint8_t txDataLength1= sizeof(test1)/sizeof(uint8_t);
	USART_send_Array(iUSART1, 0x0, test1, txDataLength1);
#endif
		
#ifdef ACTIVE_USER_DEFINED_SEND_FUNCTION_TEST
	#define MAX_VALUE_TEST_2 70
	uint8_t test2[MAX_VALUE_TEST_2]={0};
	for (int i = 0; i < MAX_VALUE_TEST_2; i++){
		test2[i] = i;
	}
	uint8_t txDataLength2= sizeof(test2)/sizeof(uint8_t);
	dataTx(test2,txDataLength2);
#endif

    while (1){
#ifdef ACTIVE_ECHO_TEST
		echoTest();
#endif
    }
	return 0;
}


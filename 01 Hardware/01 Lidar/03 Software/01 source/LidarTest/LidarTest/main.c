/*
 * LidarTest.c
 *
 * Created: 10/8/2023 2:49:32 PM
 * Author : Thach
 */ 

#include "main.h"
#include "importedFiles/wr_qFormFilterCof.h"
#include <string.h>

EMPTY_INTERRUPT(BADISR_vect);

volatile uint8_t output[LIDAR_RX_BUFFER_MAX_LEN] = {0};
volatile uint16_t outputLen = sizeof(output)/sizeof(uint8_t);
//#define MAIN_DEBUG_1
#ifdef MAIN_DEBUG_1
volatile uint16_t mData[DATA_SPL_NUM] = {0};
#endif

volatile uint8_t fData[DATA_SPL_NUM] = {0};
volatile uint16_t fDataLen = sizeof(fData)/sizeof(uint8_t);
volatile reg8Model_t mainStream = {0}; 

#ifdef TEST_RX
volatile uint8_t input[10] = {0};
volatile uint16_t inputLen = sizeof(input)/sizeof(uint8_t);
#endif

#define D1_INIT PORTD.DIR |= (1<<0)
#define D1_ON PORTD.OUT |= (1<<0)
#define D1_OFF PORTD.OUT &= ~(1<<0)
#define D1_TOOGLE PORTD.OUT ^= (1<<0)

static void ucInit(){
	init_Core_CLK(INTERN_CLK,1);//Clock-Init
	D1_INIT;////IO-Pin Init: Error Pin = PIN PD0
	//D1_TOOGLE;
	usartConfig_t config = {
		.usartNo = iUSART0,
		.baudrate = 38400,
		.usartChSize = USART_CHSIZE_8BIT_gc,
		.parity = USART_PMODE_DISABLED_gc,
		.stopbit = USART_SBMODE_1BIT_gc,
		.sync = false,
		.mpcm = false,
		.address = 0,
		.portMux = PORTMUX_USARTx_DEFAULT_gc,
	};
	timerInit(REZ_MS,10);
	lidar_initDev((const usartConfig_t*)&config,0x8005,(uint8_t*)output,(uint16_t*)&outputLen,(reg8Model_t*)&mainStream);
	fil_init(wr_num,(uint8_t)wr_numLen);
	//D1_TOOGLE;
}

int main(void){
	volatile uint8_t temp1 = 0;
	//Set Flag
	const lidarStatus_t* lidarFlag_p = lidar_getStatus();
	//Lidar-Handler
	uint8_t lidarFSMState = LIDAR_MAIN_SYNC_STATE;
	lidarFSMState = LIDAR_MAIN_RESET_STATE;
	//Initialisierung des Mikrocontrollers
	ucInit();
	sei();
    /* Replace with your application code */
    while (1) {
		//Data rx
		lidarFSMState = lidar_mainFsmLookupTable[lidarFSMState]();
		//Data Filter
		if (outputLen){
			fil_setNConvertData((uint8_t*)output, (uint16_t*)&outputLen);
			fil_run();
			fil_compressNReturn((uint8_t*)fData,361,1);
		}
		temp1++;
		//Data zu USVDATA Tx
		
    }
}


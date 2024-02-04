/*
 * LidarMain.c
 *
 * Created: 12/15/2023 9:08:52 AM
 * Author : Thach
 * Version: 1.1
 * Revision: 1.0
 */ 

#include "main.h"

EMPTY_INTERRUPT(BADISR_vect);

int main(void){
	/**** Deklaration der Variablen ****/
	/* io-stream für Lidar-Handler, Data-Processing, USVData-Handler*/
	volatile reg8Model_t mainStream = {0};
	/* LidarHandler-Variablen */
	volatile uint8_t lidarOutput[LIDAR_RX_BUFFER_MAX_LEN] = {0};
	volatile uint16_t lidarOutputLen = sizeof(lidarOutput)/sizeof(uint8_t);
	volatile uint8_t lidarFSMState = LIDAR_MAIN_SYNC_STATE;
	volatile uint8_t lidarFlagLen = LIDAR_STATUS_REG_LEN_BYTE;
	const lidarStatus_t* lidarFlag_p = lidar_getStatus();
	/* USV-Variablen */
	volatile uint8_t usvDataBuffer[LIDAR_OUTPUT_IDEAL_LEN] = {0};
	volatile uint16_t usvDataBufferLen = sizeof(usvDataBuffer)/sizeof(uint8_t);
	volatile uint8_t usvDataFSMState = USV_MAIN_FSM_START_STATE;
	/*temp-Variable*/
	usartConfig_t config = {
		.usartNo = iUSART0, .baudrate = 38400,
		.usartChSize = USART_CHSIZE_8BIT_gc, .parity = USART_PMODE_DISABLED_gc,
		.stopbit = USART_SBMODE_1BIT_gc, .sync = false,
		.mpcm = false, .address = 0, .portMux = PORTMUX_USARTx_DEFAULT_gc,
	};
	uint8_t timerRes_ms = 10;
	
	/**** Initialisierung der Module ****/
	//CPU-CLK
	init_Core_CLK(INTERN_CLK,1);//magic number prescaler
	//Timer
	timer_init(REZ_MS,timerRes_ms,timerRes_ms);
	//IO-Pin
	PORTD.DIR |= (1<<2);//PD2 als Output, die anderen als Input (Default)
	//Lidar-Interpretator
	lidar_initDev((const usartConfig_t*)&config, LIDAR_CHECKSUM_POLYNOM, (uint8_t*)lidarOutput,\
	(uint16_t*)&lidarOutputLen, (reg8Model_t*)&mainStream);
	//USVData
	config.usartNo = iUSART1;
	config.baudrate = 250000;
	config.parity = USART_PMODE_ODD_gc;
	usv_initDev((const usartConfig_t*)&config, USV_CHECKSUM_POLYNOM,(reg8Model_t*)&mainStream,\
	(const uint8_t*)usvDataBuffer, (const uint16_t*)&usvDataBufferLen,\
	(const uint8_t*)lidarFlag_p->reg8, (uint8_t*)&lidarFlagLen);
	//Filter
	fil_init(wr_num,(uint8_t)wr_numLen,(reg8Model_t*)&mainStream);
	sei();
}


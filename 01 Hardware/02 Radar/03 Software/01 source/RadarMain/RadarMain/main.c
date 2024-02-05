/*
 * RadarMain.c
 *
 * Created: 12/19/2023 12:32:42 PM
 * Author : Thach
 * Version: 1.1
 * Revision: 1.0
 */ 


#include "main.h"

EMPTY_INTERRUPT(BADISR_vect);

int main(void){
	/* Deklaration der Variablen */
	/* io-stream für Radar-Handler, Data-Processing, USVData-Handler*/
	volatile reg8Model_t mainStream = {0};
	/* RadarHandler Variablen*/
	volatile float radarVel = 0;
	volatile float radarDis = 0;
	uint8_t radarDataUpdated = 0;
	uint8_t radarFsmState = RADAR_MAIN_FSM_SYNC_STATE;
	const radarStatus_t* radarFlag_p = radar_getStatus();
	volatile uint8_t radarFlagLen = 1;
	/* USV-Variablen */
	volatile uint8_t usvDataBuffer[RADAR_OUTPUT_IDEAL_LEN] = {0};
	volatile uint16_t usvDataBufferLen = sizeof(usvDataBuffer)/sizeof(uint8_t);
	volatile uint8_t usvDataFSMState = USV_MAIN_FSM_START_STATE;
	/*Temp-Var*/
	usartConfig_t config = {
		.usartNo = iUSART0,.baudrate = 19200,
		.usartChSize = USART_CHSIZE_8BIT_gc, .parity = USART_PMODE_DISABLED_gc,
		.stopbit = USART_SBMODE_1BIT_gc, .sync = false,
		.mpcm = false, .address = 0, .portMux = PORTMUX_USARTx_DEFAULT_gc,
	};
	uint8_t timerRes_ms = 10;
	
	/*Initialisierung der Module*/
	//CPU-CLK
	init_Core_CLK(INTERN_CLK,1);
	//IO-Init
	PORTD.DIR |= (1<<2);//PD2 als Output, die anderen als Input (Default)
	//Radar-Handler
	radar_initDev((const usartConfig_t*)&config,(float*)&radarVel,(float*)&radarDis,\
				  &radarDataUpdated, (reg8Model_t*)&mainStream);
	//Timer
	timer_init(REZ_MS,timerRes_ms,timerRes_ms);
	//USV-Data
	config.usartNo = iUSART1;
	config.baudrate = 250000;
	config.parity = USART_PMODE_ODD_gc;
	usv_initDev((const usartConfig_t*)&config, USV_CHECKSUM_POLYNOM,(reg8Model_t*)&mainStream,\
				(const uint8_t*)usvDataBuffer, (const uint16_t*)&usvDataBufferLen,\
				(const uint8_t*)&radarFlag_p->reg8, (uint8_t*)&radarFlagLen);
    sei();
    while (1){
		/* Radar-FSM */
		radarFsmState = radar_mainFsmLookupTable[radarFsmState]();
		/* USV-FSM */
		usvDataFSMState = usv_mainFsmLookupTable[usvDataFSMState]();
		usvDataFSMState = usv_mainFsmLookupTable[usvDataFSMState]();
		/*Daten in usvDataBuffer kopieren*/
		if (radarDataUpdated){
			radarDataUpdated = 0;
			data_compareNReturn(radarVel,radarDis,(uint8_t*)usvDataBuffer,3);
			mainStream.val |= (1<<STREAM_RADAR_DATA_BIT_POS);
		}
		
    }
}


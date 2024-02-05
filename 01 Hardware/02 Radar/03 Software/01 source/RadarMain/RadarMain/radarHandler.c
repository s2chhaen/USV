/*
 * radarHandler.c
 *
 * Created: 12/12/2023 11:56:26 AM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.1
 */ 

#include "radarHandler.h"

//Lokalvariable für Radar
//USART-Modul-FIFO-Max-Len
static const uint16_t usartFIFOMaxLen = _FIFO_max_def - 1;
//Protokollbereich
static uint8_t radar_protocol[RADAR_PROTOCOL_MAX_LEN]  = {0};
//Daten-RX-Buffer
volatile radarRxDataFIFO_t radar_fifo = {
	.rPtr = 0, .wPtr = 0, .full = 0, .empty = 1
};

//Zeiger zur Ausgabe für Radar-Daten
volatile float* radar_velData;
volatile float* radar_disData;
volatile uint8_t* radar_dataUpdated;
//Verwaltungsbereich
volatile radarMgr_t radar_mgr = {0};
volatile radarStatus_t radar_status = {0};
volatile usartConfig_t radar_comParam = {0};
volatile uint8_t radar_dataPackRemained = RADAR_DATA_REQ_MAX_PACK;
volatile int8_t radar_tryTime = 1;

volatile reg8Model_t* radar_ioStream = NULL;
volatile uint8_t radar_programPos = COM_PROGRAMM_NORMAL_POS;
	
/* temp-Var zum Speichern des Params von der Rückruffunktion */
//für usart TX - callback Funktion angewendet
volatile uint8_t* radar_txTempData[1] = {0};
volatile uint8_t* radar_txTempLength;
volatile uint8_t radar_txTempMax_length;
//für usart RX - callback Funktion angewendet
volatile uint8_t* radar_rxTempData;
volatile uint8_t radar_rxTempLength;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Interne FSM-Deklaration
volatile uint8_t radar_mode = RADAR_SYNC_MODE;
volatile uint8_t radar_fsmState[RADAR_MODE_NUM] ={
	RADAR_SYNC_FSM_END_STATE,
	RADAR_DATA_FSM_END_STATE
};
volatile uint8_t radar_tempState = 0;

//Sync-FSM 
static uint8_t radar_syncStartSHandlerFunc();
static uint8_t radar_syncResetMsgBeginSHandlerFunc();
static uint8_t radar_syncResetMsgRxSHandlerFunc();
static uint8_t radar_syncDataMsgBeginSHandlerFunc();
static uint8_t radar_syncDataMsgRxSHandlerFunc();
static uint8_t radar_syncEndSHandlerFunc();

radar_fsmStateHandlerFunc_t radar_syncCbTable[RADAR_SYNC_FSM_STATE_NUM] = {
	&radar_syncStartSHandlerFunc,
	&radar_syncResetMsgBeginSHandlerFunc,
	&radar_syncResetMsgRxSHandlerFunc,
	&radar_syncDataMsgBeginSHandlerFunc,
	&radar_syncDataMsgRxSHandlerFunc,
	&radar_syncEndSHandlerFunc,
};

//Getter(Datenempfangen)-FSM
static uint8_t radar_dataStartSHandlerFunc();
static uint8_t radar_dataRXPrepSHandlerFunc();
static uint8_t radar_dataRXBeginSHandlerFunc();
static uint8_t radar_dataRXSHandlerFunc();
static uint8_t radar_dataRXTerminalSHandlerFunc();
static uint8_t radar_dataEndSHandlerFunc();

radar_fsmStateHandlerFunc_t radar_dataCbTable[RADAR_DATA_FSM_STATE_NUM] = {
	&radar_dataStartSHandlerFunc,
	&radar_dataRXPrepSHandlerFunc,
	&radar_dataRXBeginSHandlerFunc,
	&radar_dataRXSHandlerFunc,
	&radar_dataRXTerminalSHandlerFunc,
	&radar_dataEndSHandlerFunc
};

//Gesamten-Look-up-Tabelle der Zustand-Handler-Funktionen
radar_fsmStateHandlerFunc_t* radar_allFsmLookuptable[] = {
	(radar_fsmStateHandlerFunc_t*)&radar_syncCbTable,
	(radar_fsmStateHandlerFunc_t*)&radar_dataCbTable
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//externe FSM- Deklaration
static uint8_t radar_mainSyncHandlerFunc();
static uint8_t radar_mainSyncPollingSHandlerFunc();
static uint8_t radar_mainSyncDataCheckSHandlerFunc();
static uint8_t radar_mainDataReqSHandlerFunc();
static uint8_t radar_mainRspPollingSHandlerFunc();
static uint8_t radar_mainDataCheckSHandlerFunc();
static uint8_t radar_mainResetSHandlerFunc();
static uint8_t radar_mainErrorSHandlerFunc();

radar_mainFsmSHandlerFunc_t radar_mainFsmLookupTable[RADAR_MAIN_FSM_STATE_NUM] = {
	&radar_mainSyncHandlerFunc,
	&radar_mainSyncPollingSHandlerFunc,
	&radar_mainSyncDataCheckSHandlerFunc,
	&radar_mainDataReqSHandlerFunc,
	&radar_mainRspPollingSHandlerFunc,
	&radar_mainDataCheckSHandlerFunc,
	&radar_mainResetSHandlerFunc,
	&radar_mainErrorSHandlerFunc,
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//interne verwendete Funktionen

static inline uint8_t checkFIFOFullState(){//TODO test again
	return (radar_fifo.wPtr==radar_fifo.rPtr);
}

static inline uint8_t checkFIFOEmptyState(){//TODO test again
	return (radar_fifo.wPtr==radar_fifo.rPtr) && (!radar_fifo.full);
}

static inline uint8_t getFIFODataNum(){//TODO test again
	uint8_t result;
	if (radar_fifo.full){
		result = RADAR_RX_STR_FIFO_LEN;
	} else{
		result = (RADAR_RX_STR_FIFO_LEN + radar_fifo.wPtr - radar_fifo.rPtr)%RADAR_RX_STR_FIFO_LEN;
	}
	return result;
}

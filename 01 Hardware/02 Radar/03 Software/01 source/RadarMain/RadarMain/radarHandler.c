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

static inline uint8_t getFIFOFreeSpace(){//TODO test again
	uint8_t result;
	if (radar_fifo.empty){
		result = 0;
	} else{
		result = (RADAR_RX_STR_FIFO_LEN + radar_fifo.rPtr - radar_fifo.wPtr)%RADAR_RX_STR_FIFO_LEN;
	}
	return result;
}

static inline uint8_t radar_ioStreamStatusAvai(){
	return !(radar_ioStream->val & (1 << STREAM_RADAR_STATUS_BIT_POS));
}

static inline uint8_t radar_ioStreamDataAvai(){
	return !(radar_ioStream->val & (1 << STREAM_RADAR_DATA_BIT_POS));
}

static inline void radar_fifoFlush(){
	memset((uint8_t*)radar_fifo.dataStrIdx,0,RADAR_RX_STR_FIFO_LEN);
	radar_fifo.rPtr = 0;
	radar_fifo.wPtr = 0;
	radar_fifo.full = 0;
	radar_fifo.empty = 1;
}

static uint8_t searchNumFormatInStr(uint8_t* input_p, uint8_t inputLen){
	uint8_t result = 0;
	if (inputLen < (RADAR_RX_STR_MAX_LEN-1)){
		uint8_t check;
		for(volatile int i = 0; i < inputLen; i++){
			check = ((input_p[i]=='+') || (input_p[i]=='-')) && isdigit(input_p[i+1]);
			check = check || isdigit(input_p[i]);
			if (check){
				result = i;
				break;
			}
		}
	}
	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//interne-FSM-Zustand-Handler-Implementierung
//Sync
static uint8_t radar_syncStartSHandlerFunc(){
	uint8_t retVal = RADAR_SYNC_FSM_RESET_MSG_BEGIN_STATE;
	uint8_t check = (radar_programPos == COM_PROGRAMM_NORMAL_POS) || (radar_programPos == COM_PROGRAMM_TX_POS);
	if (check){
		radar_status.dataBf.sync = 0;
		radar_mgr.syncStatus = 1;
		USART_set_Bytes_to_receive(radar_comParam.usartNo,1);
	} else{
		radar_mgr.syncStatus = 0;
		radar_status.dataBf.lineStatus = 1;
		retVal = RADAR_SYNC_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t radar_syncResetMsgBeginSHandlerFunc(){
	uint8_t retVal = RADAR_SYNC_FSM_RESET_MSG_RX_STATE;
	uint8_t check = (radar_programPos == COM_PROGRAMM_RX_POS) && (radar_rxTempLength == 1);
	if (check){
		check = (radar_rxTempData[0] == RADAR_SYNC_PROTOCOL_START_SYM);
		if (check){
			radar_status.reg8 = 0;
			radarTimer_setState(1);
			uint8_t tempStrIdx = radar_fifo.dataStrIdx[0];
			radar_fifo.data[0][tempStrIdx] = RADAR_SYNC_PROTOCOL_START_SYM;
			(radar_fifo.dataStrIdx[0])++;
		} else{
			retVal = RADAR_SYNC_FSM_RESET_MSG_BEGIN_STATE;
		}
		USART_set_Bytes_to_receive(radar_comParam.usartNo,1);
	} else{
		radar_mgr.syncStatus = 0;
		radar_status.dataBf.lineStatus = 1;
		retVal = RADAR_SYNC_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t radar_syncResetMsgRxSHandlerFunc(){
	uint8_t retVal = RADAR_SYNC_FSM_RESET_MSG_RX_STATE;
	uint8_t check = (radar_programPos == COM_PROGRAMM_RX_POS) && (radar_rxTempLength==1);
	if (check){
		uint8_t tempWPtr = radar_fifo.wPtr;
		uint8_t tempStrIdx = radar_fifo.dataStrIdx[tempWPtr];
		radar_fifo.data[tempWPtr][tempStrIdx] = radar_rxTempData[0];
		(radar_fifo.dataStrIdx[tempWPtr])++;
		if (radar_rxTempData[0]==RADAR_PROTOCOL_END_SYM){
			//feed NULL char to make string
			radar_fifo.data[tempWPtr][tempStrIdx+1] = ASCII_NULL_CHAR;
			radar_fifo.wPtr++;
			radar_fifo.empty = 0;
			radar_fifo.full = checkFIFOFullState();
			retVal = RADAR_SYNC_FSM_DATA_MSG_BEGIN_STATE;
		}
		USART_set_Bytes_to_receive(radar_comParam.usartNo,1);
	} else{
		radar_mgr.syncStatus = 0;
		radar_status.dataBf.lineStatus = 1;
		retVal = RADAR_SYNC_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t radar_syncDataMsgBeginSHandlerFunc(){
	uint8_t retVal = RADAR_SYNC_FSM_DATA_MSG_RX_STATE;
	uint8_t check = (radar_programPos == COM_PROGRAMM_RX_POS) && (radar_rxTempLength==1) &&\
					(radar_rxTempData[0] == RADAR_DATA_PROTOCOL_START_SYM);
	if (check){
		uint8_t tempWPtr = radar_fifo.wPtr;
		uint8_t tempStrIdx = radar_fifo.dataStrIdx[tempWPtr];
		radar_fifo.data[tempWPtr][tempStrIdx] = RADAR_DATA_PROTOCOL_START_SYM;
		(radar_fifo.dataStrIdx[tempWPtr])++;
		USART_set_Bytes_to_receive(radar_comParam.usartNo,1);
	} else{
		radar_mgr.syncStatus = 0;
		radar_status.dataBf.lineStatus = 1;
		retVal = RADAR_SYNC_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t radar_syncDataMsgRxSHandlerFunc(){
	uint8_t retVal = RADAR_SYNC_FSM_DATA_MSG_RX_STATE;
	uint8_t check = (radar_programPos == COM_PROGRAMM_RX_POS) && (radar_rxTempLength == 1);
	if (check){
		uint8_t tempWPtr = radar_fifo.wPtr;
		uint8_t tempStrIdx = radar_fifo.dataStrIdx[tempWPtr];
		radar_fifo.data[tempWPtr][tempStrIdx] = radar_rxTempData[0];
		(radar_fifo.dataStrIdx[tempWPtr])++;
		if (radar_rxTempData[0]==RADAR_PROTOCOL_END_SYM){
			radar_fifo.data[tempWPtr][tempStrIdx+1] = ASCII_NULL_CHAR;//feed NULL char to make string
			radar_fifo.wPtr++;
			radar_fifo.empty = 0;
			radar_fifo.full = checkFIFOFullState();
			radar_mgr.syncStatus = 0;
			retVal = RADAR_SYNC_FSM_END_STATE;
		}
	} else{
		radar_mgr.syncStatus = 0;
		radar_status.dataBf.lineStatus = 1;
		retVal = RADAR_SYNC_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t radar_syncEndSHandlerFunc(){
	return RADAR_SYNC_FSM_END_STATE;
}

//Getter(Datenempfangen)-FSM
static uint8_t radar_dataStartSHandlerFunc(){
	radar_mode = RADAR_GETTER_MODE;
	radar_mgr.rxStatus = 1;
	return RADAR_DATA_FSM_RX_PREP_STATE;
}

static uint8_t radar_dataRXPrepSHandlerFunc(){
	uint8_t retVal = RADAR_DATA_FSM_RX_BEGIN_STATE;
	uint8_t check = (radar_programPos == COM_PROGRAMM_TX_POS);
	if (check){
		USART_set_Bytes_to_receive(radar_comParam.usartNo,1);
	} else{
		radar_mgr.rxStatus = 0;
		radar_status.dataBf.lineStatus = 1;
		retVal = RADAR_DATA_FSM_END_STATE;
	}
	
	return retVal;
}

static uint8_t radar_dataRXBeginSHandlerFunc(){
	uint8_t retVal = RADAR_DATA_FSM_RX_STATE;
	uint8_t check = (radar_programPos == COM_PROGRAMM_RX_POS) && (radar_rxTempLength == 1) &&\
					(radar_rxTempData[0] == RADAR_DATA_PROTOCOL_START_SYM) &&\
					(!(radar_fifo.full));
	if (check){
		uint8_t tempWPtr = radar_fifo.wPtr;
		uint8_t tempStrIdx = radar_fifo.dataStrIdx[tempWPtr];
		radar_fifo.data[tempWPtr][tempStrIdx] = RADAR_DATA_PROTOCOL_START_SYM;
		radar_fifo.dataStrIdx[tempWPtr]++;
		USART_set_Bytes_to_receive(radar_comParam.usartNo,1);
	} else{
		radar_mgr.rxStatus = 0;
		radar_status.dataBf.lineStatus = 1;
		retVal = RADAR_DATA_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t radar_dataRXSHandlerFunc(){
	uint8_t retVal = RADAR_DATA_FSM_RX_STATE;
	uint8_t check = (radar_programPos == COM_PROGRAMM_RX_POS) && (radar_rxTempLength == 1);
	if (check){
		uint8_t tempWPtr = radar_fifo.wPtr;
		uint8_t tempStrIdx = radar_fifo.dataStrIdx[tempWPtr];
		radar_fifo.data[tempWPtr][tempStrIdx] = radar_rxTempData[0];
		radar_fifo.dataStrIdx[tempWPtr]++;
		if (radar_rxTempData[0]==RADAR_DATA_PROTOCOL_NEAR_END_SYM){
			retVal = RADAR_DATA_FSM_RX_TERMINAL_STATE;
		}
		USART_set_Bytes_to_receive(radar_comParam.usartNo,1);
	} else{
		radar_mgr.rxStatus = 0;
		radar_status.dataBf.lineStatus = 1;
		retVal = RADAR_DATA_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t radar_dataRXTerminalSHandlerFunc(){
	uint8_t retVal = RADAR_DATA_FSM_END_STATE;
	uint8_t check = (radar_programPos == COM_PROGRAMM_RX_POS) && (radar_rxTempLength == 1) &&\
					(radar_rxTempData[0] == RADAR_PROTOCOL_END_SYM);
	if (check){
		uint8_t tempWPtr = radar_fifo.wPtr;
		uint8_t tempStrIdx = radar_fifo.dataStrIdx[tempWPtr];
		radar_fifo.data[tempWPtr][tempStrIdx] = RADAR_PROTOCOL_END_SYM;
		radar_fifo.dataStrIdx[tempWPtr]++;
		//feed NULL char to make string
		radar_fifo.data[tempWPtr][tempStrIdx+1] = ASCII_NULL_CHAR;
		radar_fifo.wPtr++;
		radar_fifo.empty = 0;
		radar_fifo.full = checkFIFOFullState();
		radar_dataPackRemained--;
		if (radar_dataPackRemained){
			retVal = RADAR_DATA_FSM_RX_BEGIN_STATE;
			USART_set_Bytes_to_receive(radar_comParam.usartNo,1);
		} else{
			radar_dataPackRemained = RADAR_DATA_REQ_MAX_PACK;
			radar_mgr.rxStatus = 0;
		}
	} else{
		radar_mgr.rxStatus = 0;
		radar_status.dataBf.lineStatus = 1;
	}
	return retVal;
}

static uint8_t radar_dataEndSHandlerFunc(){
	return RADAR_DATA_FSM_END_STATE;
}

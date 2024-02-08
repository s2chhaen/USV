/*
 * radarHandler.c
 * Beschreibung: Quellcode-Datei der radarHandler.h-Datei
 * Created: 12/12/2023 11:56:26 AM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.2
 */ 

#include "radarHandler.h"

/* Abkürzungen: vel: velocity, dis:distance,
 * Len: Length, SHandler:State-Handler,
 * Sync: Synchronisation
 */

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

/**
 * \brief Überprüfen, ob das FIFO-Buffer voll ist oder nicht
 * 
 * 
 * \return uint8_t 1: voll, 0: nicht voll
 */
static inline uint8_t checkFIFOFullState(){
	return (radar_fifo.wPtr==radar_fifo.rPtr);
}

/**
 * \brief  Überprüfen, ob das FIFO-Buffer leer ist oder nicht
 *
 *
 * \return uint8_t 1: leer, 0: nicht leer
 */
static inline uint8_t checkFIFOEmptyState(){
	return (radar_fifo.wPtr==radar_fifo.rPtr) && (!radar_fifo.full);
}

/**
 * \brief Ausgabe der Anzahl der Zeichenfolgen im FIFO
 * 
 * 
 * \return uint8_t die Anzahl der empfangenen Daten vom Radar
 */
static inline uint8_t getFIFODataNum(){
	uint8_t result;
	if (radar_fifo.full){
		result = RADAR_RX_STR_FIFO_LEN;
	} else{
		result = (RADAR_RX_STR_FIFO_LEN + radar_fifo.wPtr - radar_fifo.rPtr)%RADAR_RX_STR_FIFO_LEN;
	}
	return result;
}

/**
 * \brief Ausgabe der freien Plätze im FIFO-Buffer
 * 
 * 
 * \return uint8_t die noch verfügbaren Plätze im FIFO-Buffer
 */
static inline uint8_t getFIFOFreeSpace(){
	uint8_t result;
	if (radar_fifo.empty){
		result = 0;
	} else{
		result = (RADAR_RX_STR_FIFO_LEN + radar_fifo.rPtr - radar_fifo.wPtr)%RADAR_RX_STR_FIFO_LEN;
	}
	return result;
}

/**
 * \brief Überprüfung, ob das IO-Stream für Zustandsenden frei ist
 * 
 * 
 * \return uint8_t 0: frei, sonst nicht
 */
static inline uint8_t radar_ioStreamStatusAvai(){
	return !(radar_ioStream->val & (1 << STREAM_RADAR_STATUS_BIT_POS));
}

/**
 * \brief Überprüfung, ob das IO-Stream für Datensenden frei ist
 * 
 * 
 * \return uint8_t 0: frei, sonst nicht
 */
static inline uint8_t radar_ioStreamDataAvai(){
	return !(radar_ioStream->val & (1 << STREAM_RADAR_DATA_BIT_POS));
}

/**
 * \brief Rücksetzen des FIFO-Buffers
 * 
 * 
 * \return void
 */
static inline void radar_fifoFlush(){
	memset((uint8_t*)radar_fifo.dataStrIdx,0,RADAR_RX_STR_FIFO_LEN);
	radar_fifo.rPtr = 0;
	radar_fifo.wPtr = 0;
	radar_fifo.full = 0;
	radar_fifo.empty = 1;
}

/**
 * \brief Suchen der Nummer-Ziffer-Folge in Zeichenfolge
 * 
 * \param input_p der Zeiger zur Zeichenfolge
 * \param inputLen die Länge der Zeichenfolge
 * 
 * \return uint8_t Anfangsposition vom Nummer-Ziffer-Folge
 */
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

//Externe-FSM
static uint8_t radar_mainSyncHandlerFunc(){
	uint8_t retVal = RADAR_MAIN_FSM_SYNC_STATE;
	uint8_t check = radar_mgr.init && radar_ioStreamStatusAvai() && radar_ioStreamDataAvai();
	if (check){
		radar_mode = RADAR_SYNC_MODE;
		radar_fsmState[RADAR_SYNC_MODE] = radar_allFsmLookuptable[RADAR_SYNC_MODE][RADAR_SYNC_FSM_START_STATE]();
		//Set timeout
		radarTimer_setCounter(5000UL);
		retVal = RADAR_MAIN_FSM_SYNC_POLLING_STATE;
	}
	return retVal;
}

static uint8_t radar_mainSyncPollingSHandlerFunc(){
	uint8_t retVal = RADAR_MAIN_FSM_SYNC_POLLING_STATE;
	if (radar_mgr.syncStatus){
		if (radarTimer_getCounter() < 0){
			radarTimer_setState(0);
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				USART_flushRXFIFO(radar_comParam.usartNo);
				radar_fsmState[RADAR_SYNC_MODE] = RADAR_SYNC_FSM_END_STATE;
			}
			radar_status.dataBf.timeOut = 1;
			radar_mgr.syncStatus = 0;
			retVal = RADAR_MAIN_FSM_SYNC_DATA_CHECK_STATE;
		}
	} else{
		radarTimer_setState(0);
		retVal = RADAR_MAIN_FSM_SYNC_DATA_CHECK_STATE;
	}
	return retVal;
}

static uint8_t radar_mainSyncDataCheckSHandlerFunc(){
	uint8_t retVal = RADAR_MAIN_FSM_DATA_REQ_STATE;
	if (radar_status.dataBf.lineStatus || radar_status.dataBf.timeOut){
		radar_status.dataBf.sync = 0;
		retVal = RADAR_MAIN_FSM_SYNC_STATE;
	} else{
		uint8_t dataNum = getFIFODataNum();
		static const uint8_t syncMsg[] = RADAR_SYNC_MSG;
		uint8_t tempRPtr = radar_fifo.rPtr;
		const char* tempData = (const char*)radar_fifo.data[tempRPtr];
		uint8_t check = (dataNum == RADAR_DATA_REQ_MAX_PACK) &&\
						(!strcmp(tempData,(const char*)syncMsg));
		if (check){
			radar_fifo.dataStrIdx[tempRPtr] = 0;
			radar_fifo.rPtr++;
			tempRPtr = radar_fifo.rPtr;
			tempData = (const char*)radar_fifo.data[tempRPtr];
			check = strstr((const char*)tempData,"mps") != NULL;
			if (check){
				radar_status.dataBf.sync = 1;
				uint8_t tempIdx = searchNumFormatInStr((uint8_t*)tempData,radar_fifo.dataStrIdx[tempRPtr]);
				radar_fifo.dataStrIdx[tempRPtr] = 0;
				radar_fifo.rPtr++;
				*radar_velData = (float)strtod(&tempData[tempIdx],NULL);
				*radar_dataUpdated = 1;
			} else{
				radar_status.dataBf.fmwDevStatus = 1;
				retVal = RADAR_MAIN_FSM_SYNC_STATE;
			}
		} else{
			radar_status.dataBf.fmwDevStatus = 1;
			retVal = RADAR_MAIN_FSM_SYNC_STATE;
		}	
	}
	if (radar_ioStreamStatusAvai()){
		radar_ioStream->val |= (1<<STREAM_RADAR_STATUS_BIT_POS);
	}
	radar_fifoFlush();
	return retVal;
}

static uint8_t radar_mainDataReqSHandlerFunc(){
	uint8_t retVal = RADAR_MAIN_FSM_DATA_REQ_STATE;
	uint8_t check = radar_mgr.init && radar_ioStreamStatusAvai() && radar_ioStreamDataAvai();
	if (check){
		radar_mgr.rxStatus = 1;
		radar_status.reg8 = 0x01;
		radar_fsmState[RADAR_GETTER_MODE] = radar_allFsmLookuptable[RADAR_GETTER_MODE][RADAR_DATA_FSM_START_STATE]();
		uint8_t dataLen = (RADAR_VEL_RSP_LEN + RADAR_DIS_RSP_LEN + RADAR_REQ_PROTOCOL_LEN);
		radarTimer_setCounter(RADAR_TX_TIME_US*dataLen/1000 + RADAR_WRK_TIME_MS +\
							  RADAR_TOLERANCE_MS);
		memcpy(radar_protocol,RADAR_DATA_REQ_CMD,RADAR_REQ_PROTOCOL_LEN);
		USART_send_Array(radar_comParam.usartNo,0,radar_protocol,RADAR_REQ_PROTOCOL_LEN);
		radarTimer_setState(1);
		retVal = RADAR_MAIN_FSM_RSP_POLLING_STATE;
	}
	return retVal;
}

static uint8_t radar_mainRspPollingSHandlerFunc(){
	uint8_t retVal = RADAR_MAIN_FSM_RSP_POLLING_STATE;
	if (radar_mgr.rxStatus){
		if (radarTimer_getCounter() < 0){
			radarTimer_setState(0);
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				USART_flushRXFIFO(radar_comParam.usartNo);
				radar_fsmState[RADAR_GETTER_MODE] = RADAR_DATA_FSM_END_STATE;
			}
			radar_status.dataBf.timeOut = 1;
			radar_mgr.rxStatus = 0;
			retVal = RADAR_MAIN_FSM_DATA_CHECK_STATE;
		}
	} else{
		radarTimer_setState(0);
		retVal = RADAR_MAIN_FSM_DATA_CHECK_STATE;
	}
	return retVal;
}

static uint8_t radar_mainDataCheckSHandlerFunc(){
	uint8_t retVal = RADAR_MAIN_FSM_DATA_REQ_STATE;
	uint8_t check = radar_status.dataBf.lineStatus || radar_status.dataBf.timeOut;
	if (check){
		retVal = RADAR_MAIN_FSM_ERROR_STATE;
	} else{
		volatile uint8_t dataNum = getFIFODataNum();
		check = (!radar_fifo.empty) && (dataNum == RADAR_DATA_REQ_MAX_PACK);
		if (check){
			uint8_t checkVelCmd, checkDisCmd, tempRPtr, tempIdx;
			volatile uint8_t* tempData;
			while (dataNum){
				tempRPtr = radar_fifo.rPtr;
				tempData = radar_fifo.data[tempRPtr];
				checkVelCmd = strstr((const char*)tempData,"mps") != NULL;
				checkDisCmd = strstr((const char*)tempData,"m") != NULL;
				if (checkVelCmd){
					tempIdx = searchNumFormatInStr((uint8_t*)tempData,radar_fifo.dataStrIdx[tempRPtr]);
					*radar_velData = (float)strtod((const char*)&tempData[tempIdx],NULL);
				} else if(checkDisCmd){
					tempIdx = searchNumFormatInStr((uint8_t*)tempData,radar_fifo.dataStrIdx[tempRPtr]);
					*radar_disData = (float)strtod((const char*)&tempData[tempIdx],NULL);
				} else{
					break;
				}
				radar_fifo.dataStrIdx[tempRPtr] = 0;
				radar_fifo.rPtr++;
				dataNum--;
			}
			radar_fifo.full = 0;
			radar_fifo.empty = checkFIFOEmptyState();
			if (dataNum){
				radar_status.dataBf.lineStatus = 1;
				retVal = RADAR_MAIN_FSM_ERROR_STATE;
			} else {
				*radar_dataUpdated = 1;
			}
		} else{
			radar_status.dataBf.lineStatus = 1;
			retVal = RADAR_MAIN_FSM_ERROR_STATE;
		}
	}
	if (radar_ioStreamStatusAvai()){
		radar_ioStream->val |= (1<<STREAM_RADAR_STATUS_BIT_POS);
	}
	return retVal;
}

static uint8_t radar_mainResetSHandlerFunc(){
	uint8_t retVal = RADAR_MAIN_FSM_RESET_STATE;
	uint8_t check = radar_mgr.init && radar_ioStreamStatusAvai() && radar_ioStreamDataAvai();
	if (check){
		radar_mode = RADAR_SYNC_MODE;
		radar_fsmState[RADAR_SYNC_MODE] = RADAR_SYNC_FSM_START_STATE;
		radar_status.dataBf.sync = 0;
		uint8_t rxDataLen = (52 + RADAR_DIS_RSP_LEN + RADAR_RESET_PROTOCOL_LEN);
		radarTimer_setCounter(RADAR_TX_TIME_US*rxDataLen/1000 + RADAR_WRK_TIME_MS +\
							  RADAR_TOLERANCE_MS);
		memcpy(radar_protocol,RADAR_RESET_CMD,RADAR_RESET_PROTOCOL_LEN);
		USART_send_Array(radar_comParam.usartNo, 0, radar_protocol, RADAR_RESET_PROTOCOL_LEN);
		retVal = RADAR_MAIN_FSM_SYNC_POLLING_STATE;
	}
	return retVal;
}

static uint8_t radar_mainErrorSHandlerFunc(){
	uint8_t retVal = RADAR_MAIN_FSM_DATA_REQ_STATE;
	radar_tryTime--;
	radar_fifoFlush();
	if (radar_tryTime < 0){
		retVal = RADAR_MAIN_FSM_RESET_STATE;
		radar_tryTime = 1;
	}
	if (radar_ioStreamStatusAvai()){
		radar_ioStream->val |= (1<<STREAM_RADAR_STATUS_BIT_POS);
	}
	return retVal;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//in ISR zurückgerufte Funktionen
static bool radar_callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length, uint8_t max_length){
	radar_txTempLength = length;
	radar_txTempMax_length = max_length;
	radar_programPos = COM_PROGRAMM_TX_POS;
	//FSM-State-Handler
	radar_tempState = radar_fsmState[radar_mode];
	radar_fsmState[radar_mode] = radar_allFsmLookuptable[radar_mode][radar_tempState]();
	if (radar_txTempData[0] != NULL){
		*data = (uint8_t*)radar_txTempData[0];
	}
	radar_programPos = COM_PROGRAMM_NORMAL_POS;
	return true;
}

static bool radar_callbackRx(uint8_t adress, uint8_t data[], uint8_t length){
	radar_rxTempLength = length;
	radar_rxTempData = data;
	radar_programPos = COM_PROGRAMM_RX_POS;
	//FSM-State-Handler
	radar_tempState = radar_fsmState[radar_mode];
	radar_fsmState[radar_mode] = radar_allFsmLookuptable[radar_mode][radar_tempState]();
	radar_programPos = COM_PROGRAMM_NORMAL_POS;
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Extern
uint8_t radar_initDev(const usartConfig_t* config, float* outVel_p, float* outDis_p,\
					  uint8_t* outDataState_p, reg8Model_t* io_p){
	uint8_t result = NO_ERROR;
	uint8_t check = (config!= NULL) && (outVel_p != NULL) && ( outDis_p != NULL) &&\
					(outDataState_p != NULL) && (io_p != NULL);
	if (check){
		uint8_t temp = config->usartNo;
		check = check && !USART_init(temp, config->baudrate, config->usartChSize, config->parity,\
									 config->stopbit, config->sync, config->mpcm, config->address,\
									 config->portMux);
		if (check){
			radar_mgr.init = 1;
			radar_comParam = (*config);
			radar_velData = outVel_p;
			radar_disData = outDis_p;
			radar_dataUpdated = outDataState_p;
			radar_ioStream = io_p;
			//Zuweisung der Rückruf vom Lidar
			USART_set_send_Array_callback_fnc(temp,&radar_callbackTx);
			USART_set_receive_Array_callback_fnc(temp,&radar_callbackRx);
		} else{
			result = PROCESS_FAIL;
		}
		} else{
		result = PROCESS_FAIL;
	}
	return result;
}

const radarStatus_t* radar_getStatus(){
	return (const radarStatus_t*)&radar_status;
}
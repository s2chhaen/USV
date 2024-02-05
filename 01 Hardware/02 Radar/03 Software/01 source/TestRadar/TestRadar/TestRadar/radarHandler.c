/*
 * radarHandler.c
 *
 * Created: 12/12/2023 11:56:26 AM
 *  Author: Thach
 */ 

#include "radarHandler.h"

//radar-sensor-bezogene
static const uint16_t usartFIFOMaxLen = _FIFO_max_def - 1;
//Protokollbereich
static uint8_t radar_protocol[RADAR_PROTOCOL_MAX_LEN]  = {0};
//Data Bereich RX-Buffer
volatile radarRxDataFIFO_t radar_fifo = {
	.rPtr = 0, .wPtr = 0, .full = 0, .empty = 1
};

//volatile uint16_t radar_rxBufferToHandleBytes = 0;
//volatile uint8_t radar_expectedRxBytes = 0;
//Ausgabe, vel: velocity/Geschwindigkeit, dis:distance/Abstand
volatile int8_t* radar_velData;
volatile uint16_t* radar_disData;
//Verwaltungsbereich
volatile uint8_t radar_dataPackRemained = RADAR_DATA_REQ_MAX_PACK;
volatile radarMgr_t radar_mgr = {0};
volatile uint8_t radar_tryTime = 1;
volatile radarStatus_t radar_status = {0};
volatile reg8Model_t* radar_ioStream = NULL;
	
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

//SHandler:State-Handler
//Sync-FSM 
//TODO calculate the time the data come after turning on
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

//Getter-FSM
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
//static inline void radar_rxRountine(){
	//if (radar_rxBufferToHandleBytes <= usartFIFOMaxLen){
		//radar_expectedRxBytes = radar_rxBufferToHandleBytes;
		//USART_set_Bytes_to_receive(radar_mgr.usartNo, radar_rxBufferToHandleBytes);
	//} else{
		//radar_expectedRxBytes = (uint8_t)usartFIFOMaxLen;
		//USART_set_Bytes_to_receive(radar_mgr.usartNo, usartFIFOMaxLen);
	//}
//}

static inline uint8_t checkFIFOFullState(){
	return (radar_fifo.wPtr==radar_fifo.rPtr)?1:0;
}

static inline uint8_t checkFIFOEmptyState(){
	return (radar_fifo.wPtr==radar_fifo.rPtr)?1:0;
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
	radar_status.dataBf.sync = 0;
	radar_mgr.syncStatus = 1;
	USART_set_Bytes_to_receive(radar_mgr.usartNo,1);
	return RADAR_SYNC_FSM_RESET_MSG_BEGIN_STATE;
}

static uint8_t radar_syncResetMsgBeginSHandlerFunc(){
	uint8_t retVal = RADAR_SYNC_FSM_RESET_MSG_RX_STATE;
	timer_setState(1);
	if (radar_rxTempLength == 1){
		if (radar_rxTempData[0] == RADAR_SYNC_PROTOCOL_START_SYM){
			uint8_t tempStrIdx = radar_fifo.dataStrIdx[0];
			radar_fifo.data[0][tempStrIdx] = RADAR_SYNC_PROTOCOL_START_SYM;
			(radar_fifo.dataStrIdx[0])++;
			USART_set_Bytes_to_receive(radar_mgr.usartNo,1);
		} else{
			radar_mgr.syncStatus = 0;
			radar_status.dataBf.lineStatus = 1;
			retVal = RADAR_SYNC_FSM_END_STATE;
		}
	} else{
		radar_mgr.syncStatus = 0;
		radar_status.dataBf.lineStatus = 1;
		retVal = RADAR_SYNC_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t radar_syncResetMsgRxSHandlerFunc(){
	uint8_t retVal = RADAR_SYNC_FSM_RESET_MSG_RX_STATE;
	if (radar_rxTempLength==1){
		uint8_t tempWPtr = radar_fifo.wPtr;
		uint8_t tempStrIdx = radar_fifo.dataStrIdx[tempWPtr];
		radar_fifo.data[tempWPtr][tempStrIdx] = radar_rxTempData[0];
		(radar_fifo.dataStrIdx[tempWPtr])++;
		if (radar_rxTempData[0]==RADAR_PROTOCOL_END_SYM){
			radar_fifo.data[tempWPtr][tempStrIdx+1] = ASCII_NULL_CHAR;//feed NULL char to make string
			radar_fifo.wPtr++;
			//TODO can nhac bo cai nay vi khong can thiet, bot duoc buoc nao hay buoc do, sau khi implement no vao nhan du lieu
			radar_fifo.empty = 0;
			radar_fifo.full = checkFIFOFullState();
			retVal = RADAR_SYNC_FSM_DATA_MSG_BEGIN_STATE;
		}
		USART_set_Bytes_to_receive(radar_mgr.usartNo,1);
	} else{
		radar_mgr.syncStatus = 0;
		radar_status.dataBf.lineStatus = 1;
		retVal = RADAR_SYNC_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t radar_syncDataMsgBeginSHandlerFunc(){
	uint8_t retVal = RADAR_SYNC_FSM_DATA_MSG_RX_STATE;
	if (radar_rxTempLength == 1){
		if (radar_rxTempData[0] == RADAR_DATA_PROTOCOL_START_SYM){
			uint8_t tempWPtr = radar_fifo.wPtr;
			uint8_t tempStrIdx = radar_fifo.dataStrIdx[tempWPtr];
			radar_fifo.data[tempWPtr][tempStrIdx] = RADAR_DATA_PROTOCOL_START_SYM;
			(radar_fifo.dataStrIdx[tempWPtr])++;
			USART_set_Bytes_to_receive(radar_mgr.usartNo,1);
		} else{
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

static uint8_t radar_syncDataMsgRxSHandlerFunc(){
	uint8_t retVal = RADAR_SYNC_FSM_DATA_MSG_RX_STATE;
	if (radar_rxTempLength == 1){
		uint8_t tempWPtr = radar_fifo.wPtr;
		uint8_t tempStrIdx = radar_fifo.dataStrIdx[tempWPtr];
		radar_fifo.data[tempWPtr][tempStrIdx] = radar_rxTempData[0];
		(radar_fifo.dataStrIdx[tempWPtr])++;
		if (radar_rxTempData[0]==RADAR_PROTOCOL_END_SYM){
			__asm__("nop");
			radar_fifo.data[tempWPtr][tempStrIdx+1] = ASCII_NULL_CHAR;//feed NULL char to make string
			radar_fifo.wPtr++;
			//TODO can nhac bo cai nay vi khong can thiet, bot duoc buoc nao hay buoc do, sau khi implement no vao nhan du lieu
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

//Getter-FSM
static uint8_t radar_dataStartSHandlerFunc(){
	radar_mgr.rxStatus = 1;
	return RADAR_DATA_FSM_RX_PREP_STATE;
}

static uint8_t radar_dataRXPrepSHandlerFunc(){
	USART_set_Bytes_to_receive(radar_mgr.usartNo,1);
	return RADAR_DATA_FSM_RX_BEGIN_STATE;
}

static uint8_t radar_dataRXBeginSHandlerFunc(){
	uint8_t retVal = RADAR_DATA_FSM_RX_STATE;
	uint8_t check = (radar_rxTempLength == 1) &&\
					(radar_rxTempData[0] == RADAR_DATA_PROTOCOL_START_SYM) &&\
					(!(radar_fifo.full));
	if (check){
		uint8_t tempWPtr = radar_fifo.wPtr;
		uint8_t tempStrIdx = radar_fifo.dataStrIdx[tempWPtr];
		radar_fifo.data[tempWPtr][tempStrIdx] = RADAR_DATA_PROTOCOL_START_SYM;
		radar_fifo.dataStrIdx[tempWPtr]++;
		USART_set_Bytes_to_receive(radar_mgr.usartNo,1);
	} else{
		radar_mgr.rxStatus = 0;
		radar_status.dataBf.lineStatus = 1;
		retVal = RADAR_DATA_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t radar_dataRXSHandlerFunc(){
	uint8_t retVal = RADAR_DATA_FSM_RX_STATE;
	uint8_t check = (radar_rxTempLength == 1);
	if (check){
		uint8_t tempWPtr = radar_fifo.wPtr;
		uint8_t tempStrIdx = radar_fifo.dataStrIdx[tempWPtr];
		radar_fifo.data[tempWPtr][tempStrIdx] = radar_rxTempData[0];
		radar_fifo.dataStrIdx[tempWPtr]++;
		if (radar_rxTempData[0]==RADAR_DATA_PROTOCOL_NEAR_END_SYM){
			retVal = RADAR_DATA_FSM_RX_TERMINAL_STATE;
		}
		USART_set_Bytes_to_receive(radar_mgr.usartNo,1);
	} else{
		radar_mgr.rxStatus = 0;
		radar_status.dataBf.lineStatus = 1;
		retVal = RADAR_DATA_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t radar_dataRXTerminalSHandlerFunc(){
	uint8_t retVal = RADAR_DATA_FSM_END_STATE;
	uint8_t check = (radar_rxTempLength == 1) && (radar_rxTempData[0] == RADAR_PROTOCOL_END_SYM);
	if (check){
		uint8_t tempWPtr = radar_fifo.wPtr;
		uint8_t tempStrIdx = radar_fifo.dataStrIdx[tempWPtr];
		radar_fifo.data[tempWPtr][tempStrIdx] = RADAR_PROTOCOL_END_SYM;
		radar_fifo.dataStrIdx[tempWPtr]++;
		radar_fifo.data[tempWPtr][tempStrIdx+1] = ASCII_NULL_CHAR;//feed NULL char to make string
		radar_fifo.wPtr++;
		radar_fifo.empty = 0;
		radar_fifo.full = checkFIFOFullState();
		radar_dataPackRemained--;
		if (radar_dataPackRemained){
			retVal = RADAR_DATA_FSM_RX_BEGIN_STATE;
			USART_set_Bytes_to_receive(radar_mgr.usartNo,1);
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
	if (radar_mgr.init){
		radar_mode = RADAR_SYNC_MODE;
		radar_fsmState[RADAR_SYNC_MODE] = radar_allFsmLookuptable[RADAR_SYNC_MODE][RADAR_SYNC_FSM_START_STATE]();
		//Set timeout
		timer_setCounter(1000UL);
		retVal = RADAR_MAIN_FSM_SYNC_POLLING_STATE;
		//TODO to test
	}
	return retVal;
}

static uint8_t radar_mainSyncPollingSHandlerFunc(){
	uint8_t retVal = RADAR_MAIN_FSM_SYNC_POLLING_STATE;
	if (radar_mgr.syncStatus){
		if (timer_getCounter() < 0){
			timer_setState(0);
			radar_fsmState[RADAR_SYNC_MODE] = RADAR_SYNC_FSM_END_STATE;
			radar_mgr.syncStatus = 0;
			radar_status.dataBf.lineStatus = 1;
			retVal = RADAR_MAIN_FSM_SYNC_DATA_CHECK_STATE;
		}
	} else{
		timer_setState(0);
		retVal = RADAR_MAIN_FSM_SYNC_DATA_CHECK_STATE;
	}
	return retVal;
}

static uint8_t radar_mainSyncDataCheckSHandlerFunc(){
	uint8_t retVal = RADAR_MAIN_FSM_DATA_REQ_STATE;
	__asm__("nop");
	if (radar_status.dataBf.lineStatus){
		radar_status.dataBf.sync = 0;
		radar_fifoFlush();
		retVal = RADAR_MAIN_FSM_SYNC_STATE;
	} else{
		//TODO to test again
		static const uint8_t syncMsg[] = RADAR_SYNC_MSG;
		uint8_t tempRPtr = radar_fifo.rPtr;
		const char* tempData = (const char*)radar_fifo.data[tempRPtr];
		volatile uint8_t check = !strcmp(tempData,(const char*)syncMsg);
		if (check){
			radar_fifo.dataStrIdx[tempRPtr] = 0;
			radar_fifo.rPtr++;
			tempRPtr = radar_fifo.rPtr;
			tempData = (const char*)radar_fifo.data[tempRPtr];
			check = strstr((const char*)tempData,"mps") != NULL;
			if (check){
				//wenn erfolgreich synchronisiert dann wird die Flaggen "reset" wird
				radar_status.reg8 = (1<<0);
				radar_fifo.dataStrIdx[tempRPtr] = 0;
				radar_fifo.rPtr++;
				float temp = (float)strtod(tempData,NULL);
				*radar_velData = (uint8_t) temp * (1 >> VEL_FIXED_POINT_BIT);
				if (radar_ioStreamDataAvai()){
					radar_ioStream->val |= (1<<STREAM_RADAR_DATA_BIT_POS);
				}
			} else{
				radar_status.dataBf.fmwDevStatus = 1;
				retVal = RADAR_MAIN_FSM_SYNC_STATE;
			}
		} else{
			radar_status.dataBf.fmwDevStatus = 1;
			retVal = RADAR_MAIN_FSM_SYNC_STATE;
		}
		if (radar_ioStreamStatusAvai()){
			radar_ioStream->val |= (1<<STREAM_RADAR_STATUS_BIT_POS);
		}
		radar_fifoFlush();
	}
	return retVal;
}

static uint8_t radar_mainDataReqSHandlerFunc(){
	uint8_t retVal = RADAR_MAIN_FSM_DATA_REQ_STATE;
	uint8_t check = radar_mgr.init && radar_ioStreamStatusAvai() && radar_ioStreamDataAvai();
	if (check){
		radar_mgr.rxStatus = 1;
		radar_mode = RADAR_GETTER_MODE;
		radar_fsmState[RADAR_GETTER_MODE] = radar_allFsmLookuptable[RADAR_GETTER_MODE][RADAR_DATA_FSM_START_STATE]();
		timer_setCounter(1000UL);//TODO berechnen nochmals mit vollständige Param
		memcpy(radar_protocol,RADAR_DATA_REQ_CMD,RADAR_REQ_PROTOCOL_LEN);
		USART_send_Array(radar_mgr.usartNo,0,radar_protocol,RADAR_REQ_PROTOCOL_LEN);
		timer_setState(1);
		retVal = RADAR_MAIN_FSM_RSP_POLLING_STATE;
	}
	return retVal;
}

static uint8_t radar_mainRspPollingSHandlerFunc(){
	uint8_t retVal = RADAR_MAIN_FSM_RSP_POLLING_STATE;
	if (radar_mgr.rxStatus){
		if (timer_getCounter() < 0){
			timer_setState(0);
			radar_fsmState[RADAR_GETTER_MODE] = RADAR_DATA_FSM_END_STATE;
			radar_mgr.rxStatus = 0;
			radar_status.dataBf.lineStatus = 1;
			retVal = RADAR_MAIN_FSM_DATA_CHECK_STATE;
		}
	} else{
		timer_setState(0);
		retVal = RADAR_MAIN_FSM_DATA_CHECK_STATE;
	}
	return retVal;
}

static uint8_t radar_mainDataCheckSHandlerFunc(){
	uint8_t retVal = RADAR_MAIN_FSM_DATA_REQ_STATE;
	//TODO stopped here, to countinue
	__asm__("nop");
	if (radar_status.dataBf.lineStatus){
		retVal = RADAR_MAIN_FSM_ERROR_STATE;
	} else{
		uint8_t check = !(radar_fifo.empty);
		if (check){
			uint8_t rxDataLen = 0;
			uint8_t checkVelCmd, checkDisCmd, tempRPtr, tempIdx;
			volatile float temp;
			volatile uint8_t* tempData;
			rxDataLen = (radar_fifo.full)?RADAR_RX_STR_FIFO_LEN:((radar_fifo.wPtr - radar_fifo.rPtr + RADAR_RX_STR_FIFO_LEN)%RADAR_RX_STR_FIFO_LEN);
			while (rxDataLen){
				tempRPtr = radar_fifo.rPtr;
				tempData = radar_fifo.data[tempRPtr];
				//TODO check CMD here
				checkVelCmd = strstr((const char*)tempData,"mps") != NULL;
				checkDisCmd = strstr((const char*)tempData,"m") != NULL;
				if (checkVelCmd){
					tempIdx = searchNumFormatInStr((uint8_t*)tempData,radar_fifo.dataStrIdx[tempRPtr]);
					temp = (float)strtod((const char*)&tempData[tempIdx],NULL);
					//temp = (float)atof((const char*)tempData);
					*radar_velData = (uint8_t)(temp * (1 << VEL_FIXED_POINT_BIT));
				} else if(checkDisCmd){
					tempIdx = searchNumFormatInStr((uint8_t*)tempData,radar_fifo.dataStrIdx[tempRPtr]);
					temp = (float)strtod((const char*)&tempData[tempIdx],NULL);
					//temp = (float)atof((const char*)tempData);
					//sscanf((const char*)tempData,"%6f",&temp);
					*radar_disData = (uint8_t)(temp * (1 << DIS_FIXED_POINT_BIT));
				} else{
					break;
				}
				radar_fifo.dataStrIdx[tempRPtr] = 0;
				radar_fifo.rPtr++;
				rxDataLen--;
			}
			radar_fifo.full = 0;
			radar_fifo.empty = checkFIFOEmptyState();
			if (rxDataLen){
				radar_status.dataBf.fmwDevStatus = 1;
				retVal = RADAR_MAIN_FSM_SYNC_STATE;
			} else{
				if (radar_ioStreamDataAvai()){
					radar_ioStream->val |= (1<<STREAM_RADAR_DATA_BIT_POS);
				}
			}
			if (radar_ioStreamStatusAvai()){
				radar_ioStream->val |= (1<<STREAM_RADAR_STATUS_BIT_POS);
			}
		} else{
			radar_status.dataBf.fmwDevStatus = 1;
			retVal = RADAR_MAIN_FSM_SYNC_STATE;
		}
	}
	return retVal;
}

static uint8_t radar_mainResetSHandlerFunc(){
	radar_mode = RADAR_SYNC_MODE;
	radar_fsmState[RADAR_SYNC_MODE] = RADAR_SYNC_FSM_START_STATE;
	timer_setCounter(1000UL);
	memcpy(radar_protocol,RADAR_RESET_CMD,RADAR_RESET_PROTOCOL_LEN);
	USART_send_Array(radar_mgr.usartNo,0,radar_protocol,RADAR_RESET_PROTOCOL_LEN);
	return RADAR_MAIN_FSM_SYNC_POLLING_STATE;
}

static uint8_t radar_mainErrorSHandlerFunc(){
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//in ISR zurückgerufte Funktionen
static bool radar_callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length, uint8_t max_length){
	radar_txTempLength = length;
	radar_txTempMax_length = max_length;
	//FSM-State-Handler
	radar_tempState = radar_fsmState[radar_mode];
	radar_fsmState[radar_mode] = radar_allFsmLookuptable[radar_mode][radar_tempState]();
	if (radar_txTempData[0] != NULL){
		*data = (uint8_t*)radar_txTempData[0];
	}
	return true;
}

static bool radar_callbackRx(uint8_t adress, uint8_t data[], uint8_t length){
	radar_rxTempLength = length;
	radar_rxTempData = data;
	//FSM-State-Handler
	radar_tempState = radar_fsmState[radar_mode];
	radar_fsmState[radar_mode] = radar_allFsmLookuptable[radar_mode][radar_tempState]();
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Extern
uint8_t radar_initDev(const usartConfig_t* config, int8_t* outVel_p, uint16_t* outDis_p, reg8Model_t* io_p){
	uint8_t result = NO_ERROR;
	uint8_t check = (config!=NULL) && (outVel_p!=NULL) && ( outDis_p!=NULL) && (io_p != NULL);
	if (check){
		uint8_t temp = config->usartNo;
		check = check && !USART_init(temp, config->baudrate, config->usartChSize, config->parity,config->stopbit, config->sync, config->mpcm, config->address, config->portMux);
		if (check){
			radar_mgr.init = 1;
			radar_mgr.usartNo = temp;
			radar_velData = outVel_p;
			radar_disData = outDis_p;
			radar_ioStream = io_p;
			radar_mgr.syncStatus = 1;
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

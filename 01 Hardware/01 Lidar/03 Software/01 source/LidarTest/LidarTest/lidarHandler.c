/*
 * lidarHandler.c
 *
 * Created: 11/26/2023 4:06:34 PM
 *  Author: Thach
 */ 

#include "lidarHandler.h"

//TODO to test again
//innerhalb der Datei verwendete globale Variable
//lidar-sensor-bezogene
static const uint16_t usartFIFOMaxLen = _FIFO_max_def - 1;
static uint8_t lidar_addr = 0;//TODO add addr via init
volatile uint16_t lidar_tempChecksumVal = 0;
//Protokollbereich
volatile uint8_t lidar_protocol[LIDAR_REQ_PROTOCOL_MAX_LEN] = {0};
volatile uint8_t lidar_protocolToHandleBytes = 0;
volatile uint8_t lidar_protocolIdx = 0; //Idx: Index
//Data Bereich RX-Buffer
volatile uint8_t* lidar_rxBuffer;
volatile uint16_t* lidar_rxBufferStrLen;
volatile uint16_t lidar_rxBufferToHandleBytes = 0;
volatile uint16_t lidar_rxBufferIdx = 0;
volatile uint8_t lidar_expectedRxBytes = 0;
//Data Bereich TX-Buffer
static uint8_t lidar_txDataBuffer[10];
//Verwaltungsbereich
volatile lidarMgr_t lidar_mgr = {0};
static int8_t lidar_tryTime = 1; 
volatile uint16_t lidar_checksumPolynom = 0;
volatile lidarStatus_t  lidar_status = {0};
volatile reg8Model_t* lidar_ioStream = NULL;

/* temp-Var zum Speichern des Params von der Rückruffunktion */
//für usart TX - callback Funktion angewendet
volatile uint8_t* lidar_txTempData[1] = {0};
volatile uint8_t* lidar_txTempLength;
volatile uint8_t lidar_txTempMax_length;
//für usart RX - callback Funktion angewendet
volatile uint8_t* lidar_rxTempData;
volatile uint8_t lidar_rxTempLength;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Interne FSM-Deklaration
volatile uint8_t lidar_mode = LIDAR_PARAM_MODE;
volatile uint8_t lidar_fsmState[LIDAR_MODE_NUM] = {
	LIDAR_PARAM_FSM_END_STATE,
	LIDAR_RESET_FSM_END_STATE,
	LIDAR_GETTER_FSM_END_STATE
};
volatile uint8_t tempState = 0;
//FSM - Parametrierung
static uint8_t lidar_paramStartSHandlerFunc();
static uint8_t lidar_paramFPHandleSHandlerFunc();
static uint8_t lidar_paramRX1stCheckSHandlerFunc();
static uint8_t lidar_paramRXSHandlerFunc();
static uint8_t lidar_paramRX2ndCheckSHandlerFunc();
static uint8_t lidar_paramEndSHandlerFunc();

lidar_fsmStateHandlerFunc_t lidar_paramtCbTable[LIDAR_PARAM_FSM_STATE_NUM] = {
	&lidar_paramStartSHandlerFunc,
	&lidar_paramFPHandleSHandlerFunc,
	&lidar_paramRX1stCheckSHandlerFunc,
	&lidar_paramRXSHandlerFunc,
	&lidar_paramRX2ndCheckSHandlerFunc,
	&lidar_paramEndSHandlerFunc
};
	
//FSM-Getter
static uint8_t lidar_getterStartSHandlerFunc();
static uint8_t lidar_getterTXSHandlerFunc();
static uint8_t lidar_getterRX1stCheckSHandlerFunc();
static uint8_t lidar_getterRX2ndCheckSHandlerFunc();
static uint8_t lidar_getterRXDataSHandlerFunc();
static uint8_t lidar_getterRX3rdSHandlerFunc();
static uint8_t lidar_getterEndSHandlerFunc();

lidar_fsmStateHandlerFunc_t lidar_getterCbTable[LIDAR_GETTER_FSM_STATE_NUM] = {
	&lidar_getterStartSHandlerFunc,
	&lidar_getterTXSHandlerFunc,
	&lidar_getterRX1stCheckSHandlerFunc,
	&lidar_getterRX2ndCheckSHandlerFunc,
	&lidar_getterRXDataSHandlerFunc,
	&lidar_getterRX3rdSHandlerFunc,
	&lidar_getterEndSHandlerFunc
};

//FSM-Reset
static uint8_t lidar_resetStartSHandlerFunc();
static uint8_t lidar_resetRxSHandlerFunc();
static uint8_t lidar_resetTerminalSHandlerFunc();
static uint8_t lidar_resetEndSHandlerFunc();
lidar_fsmStateHandlerFunc_t lidar_resetCbTable[LIDAR_RESET_FSM_STATE_NUM] = {
	&lidar_resetStartSHandlerFunc,
	&lidar_resetRxSHandlerFunc,
	&lidar_resetTerminalSHandlerFunc,
	&lidar_resetEndSHandlerFunc
};

lidar_fsmStateHandlerFunc_t* lidar_allFsmLookuptable[LIDAR_MODE_NUM] = {//TODO check this
	(lidar_fsmStateHandlerFunc_t*)&lidar_paramtCbTable,
	(lidar_fsmStateHandlerFunc_t*)&lidar_resetCbTable,
	(lidar_fsmStateHandlerFunc_t*)&lidar_getterCbTable
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//externe FSM- Deklaration
//SHandler: State Handler
static uint8_t lidar_mainSyncSHandler();//OK
static uint8_t lidar_mainSyncPollingSHandler();//OK
static uint8_t lidar_mainSyncSignalCheckSHandler();//OK
static uint8_t lidar_mainDataReqSHandler();//OK
static uint8_t lidar_mainRspPollingSHandler();//OK
static uint8_t lidar_mainDataCheckSHandler();
static uint8_t lidar_mainResetSHandler();//OK
static uint8_t lidar_mainResetPollingSHandler();//OK
static uint8_t lidar_mainErrorSHandler();//OK

lidar_mainFsmSHandlerFunc_t lidar_mainFsmLookupTable[LIDAR_MAIN_FSM_STATE_NUM] = {
	&lidar_mainSyncSHandler,
	&lidar_mainSyncPollingSHandler,
	&lidar_mainSyncSignalCheckSHandler,
	&lidar_mainDataReqSHandler,
	&lidar_mainRspPollingSHandler,
	&lidar_mainDataCheckSHandler,
	&lidar_mainResetSHandler,
	&lidar_mainResetPollingSHandler,
	&lidar_mainErrorSHandler
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//interne verwendete Funktionen
uint16_t lidar_checksum16(uint8_t* input, uint16_t length){
	uint16_t uCrc16=0;
	uint8_t temp[]={0,0};
	for (uint32_t i=0;i<length;i++){
		temp[1]=temp[0];
		temp[0]=input[i];
		if (uCrc16&0x8000){
			uCrc16 = (uCrc16&0x7fff)<<1;
			uCrc16 ^= lidar_checksumPolynom;
		} else {
			uCrc16<<=1;
		}
		uCrc16 ^= (temp[0]|(temp[1]<<8));
	}
	return uCrc16;
}

static inline uint8_t lidar_rspVal(uint8_t cmdOAddr){ //O: Or
	return (cmdOAddr+0x80);
}

static inline void lidar_rxRountine(){
	if (lidar_rxBufferToHandleBytes <= usartFIFOMaxLen){
		lidar_expectedRxBytes = lidar_rxBufferToHandleBytes;
		USART_set_Bytes_to_receive(lidar_mgr.usartNo, lidar_rxBufferToHandleBytes);
	} else{
		lidar_expectedRxBytes = (uint8_t)usartFIFOMaxLen;
		USART_set_Bytes_to_receive(lidar_mgr.usartNo, usartFIFOMaxLen);
	}	
}

static inline uint8_t lidar_checkRXData(uint8_t* data, uint16_t dataLen, uint16_t rxChecksumValue){
	return (lidar_checksum16(data,dataLen)==rxChecksumValue);//TODO ausfüllen nach der Aktualisierung der CRC16-Funktion
}

static uint8_t lidar_dataCheck(uint8_t* input_p, const uint8_t* defaultVal_p, uint8_t defaultValLen){
	uint8_t result = 1;
	for (volatile uint8_t i = 0; i < defaultValLen; i++){
		if (input_p[i] != defaultVal_p[i]){
			result = 0;
			break;
		}
	}
	return result;
}

#pragma GCC push_options
#pragma GCC optimize("O3")
static int16_t lidar_getCmdDataLen(uint8_t cmdNum, uint8_t segNum){
	int16_t retVal;//no optimized this pls
	switch(cmdNum){
		case LIDAR_INIT_N_RESET:
			retVal = 0;
			break;
		case MEASURED_DATA_REQ:
			retVal = 1;
			break;
		case STATUS_REQ:
			retVal = 0;
			break;
		case ERROR_OR_TEST_TELEGRAM_REQ:
			retVal = 0;
			break;
		case OP_DATA_COUNTER_REQ:
			retVal = 0;
			break;
		case AVG_MEASURED_DATA_REQ:
			retVal = 1;
			break;
		case SEG_MEASURED_DATA_REQ:
			retVal = 4;
			break;
		case LIDAR_TYPE_REQ:
			retVal = 0;
			break;
		case MEASURED_DATA_WITH_FIELD_DATA_REQ:
			retVal = 4;
			break;
		case SEG_AVG_MEASURED_DATA_REQ:
			retVal = 5;
			break;
		case SEG_MEASURED_DATA_AND_REFLECTANCE_REQ:
			retVal = 4;
			break;
		case FIELD_REQ:
			retVal = 1;
			break;
		case FIELDS_STATUS_OUT_REQ:
			retVal = 0;
			break;
		case LIDAR_CONFIG_P1_REQ:
			retVal = 0;
			break;
		case MEASURED_DATA_WITH_REFLECTANCE_REQ:
			segNum = segNum%2;//Nur 1 bis 2 Bereiche möglich
			retVal = (segNum?6:10);
			break;
		case MEASURED_DATA_IN_XY_COORD_REQ:
			retVal = 0;
			break;
		case LIDAR_CONFIG_P2_REQ:
			retVal = 0;
			break;
		default:
			retVal = -1;
			break;
	}
	return retVal;
}
#pragma GCC pop_options

static inline uint8_t lidar_setProtocol(uint8_t cmd, uint16_t cmdDataLen){
	uint8_t tempIdx = 0;
	lidar_protocol[LIDAR_START_BYTE_POS] = LIDAR_PROTOCOL_START_SYM;
	lidar_protocol[LIDAR_ADDR_BYTE_POS] = lidar_addr;
	lidar_protocol[LIDAR_PROTOCOL_LEN_LBYTE_POS] = (cmdDataLen+1) & 0xff;
	lidar_protocol[LIDAR_PROTOCOL_LEN_HBYTE_POS] = (cmdDataLen+1) >> 8;
	lidar_protocol[LIDAR_PROTOCOL_CMD_BYTE_POS] = cmd;
	tempIdx = LIDAR_PROTOCOL_CMD_BYTE_POS+1;
	if (cmdDataLen){
		memcpy((void*)&(lidar_protocol[tempIdx]),(void*)lidar_txDataBuffer,cmdDataLen);
		tempIdx += cmdDataLen;
	}
	lidar_tempChecksumVal = lidar_checksum16((uint8_t*)lidar_protocol,tempIdx);
	lidar_protocol[tempIdx] = lidar_tempChecksumVal & 0xff;
	tempIdx++;
	lidar_protocol[tempIdx] = lidar_tempChecksumVal >> 8;
	tempIdx++;
	return tempIdx;
}

static inline void lidar_sendProtocol(){
	uint8_t temp1 = 0;
	lidar_protocolIdx = 0;
	if (lidar_protocolToHandleBytes < usartFIFOMaxLen){
		temp1 = lidar_protocolToHandleBytes;
		lidar_protocolToHandleBytes = 0;
		USART_send_Array(lidar_mgr.usartNo,0,(uint8_t*)lidar_protocol,temp1);
	} else{
		lidar_protocolToHandleBytes -= usartFIFOMaxLen;
		lidar_protocolIdx = usartFIFOMaxLen;
		USART_send_Array(lidar_mgr.usartNo,0,(uint8_t*)lidar_protocol,usartFIFOMaxLen);
	}
}

static inline uint8_t lidar_ioStreamCheckAvai(){
	return !(lidar_ioStream->val & (1<<STREAM_LIDAR_STATUS_BIT_POS));
}

static inline uint8_t lidar_checkRXBufferAvai(){
	return (!(*lidar_rxBufferStrLen));
}

#pragma GCC push_options
#pragma GCC optimize("O2")
static uint8_t lidar_dataGet(uint8_t addr, uint8_t cmd, uint8_t segNum, uint8_t* input_p, uint8_t inputLen){
	uint8_t result = NO_ERROR;
	lidar_addr = addr;
	int16_t cmdDataLen;
	cmdDataLen = lidar_getCmdDataLen(cmd,segNum);
	if (cmdDataLen!=-1){
		lidar_fsmState[LIDAR_GETTER_MODE] = lidar_allFsmLookuptable[LIDAR_GETTER_MODE][LIDAR_GETTER_FSM_START_STATE]();
		memcpy((void*)lidar_txDataBuffer,(void*)input_p,inputLen);
		lidar_protocolToHandleBytes = lidar_setProtocol(cmd, (uint16_t)cmdDataLen);
		lidar_sendProtocol();
	} else{
		result = PROCESS_FAIL;
	}
	return result;
}
#pragma GCC pop_options
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//interne-FSM-Zustand-Handler-Implementierung
//PARAM
static uint8_t lidar_paramStartSHandlerFunc(){
	lidar_status.dataBf.sync = 0;
	lidar_mgr.syncStatus = 1;
	USART_set_Bytes_to_receive(lidar_mgr.usartNo, 1);//checken der floating-pin
	return LIDAR_PARAM_FP_HANDLE_STATE;
}

static uint8_t lidar_paramFPHandleSHandlerFunc(){
	uint8_t retVal = LIDAR_PARAM_FSM_RX_CHECK_1_OHD_STATE;
	if (lidar_rxTempLength == 1){
		timer_setState(1);
		if (lidar_rxTempData[0] == LIDAR_PROTOCOL_START_SYM){
			lidar_rxBuffer[lidar_rxBufferIdx] = LIDAR_PROTOCOL_START_SYM;
			lidar_rxBufferIdx++;
			USART_set_Bytes_to_receive(lidar_mgr.usartNo, 4);
		} else{
			USART_set_Bytes_to_receive(lidar_mgr.usartNo, 5);
		}
	} else{
		lidar_mgr.syncStatus = 0;
		lidar_status.dataBf.lineStatus = 1;
		retVal = LIDAR_PARAM_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t lidar_paramRX1stCheckSHandlerFunc(){
	uint8_t retVal = LIDAR_PARAM_FSM_RX_STATE;
	uint8_t check;
	switch (lidar_rxTempLength){
		case 4:
			check = (lidar_rxTempData[LIDAR_ADDR_BYTE_POS-1] == lidar_rspVal(lidar_addr)) &&\
					(lidar_rxTempData[LIDAR_PROTOCOL_CMD_BYTE_POS-1] == lidar_rspVal(LIDAR_INIT_N_RESET));
			lidar_rxBufferToHandleBytes = (lidar_rxTempData[LIDAR_PROTOCOL_LEN_LBYTE_POS-1] |\
										  (lidar_rxTempData[LIDAR_PROTOCOL_LEN_HBYTE_POS-1]<<8)) -1;//CMD-Eklusiv
			break;
		case 5:
			check = (lidar_rxTempData[LIDAR_START_BYTE_POS] == LIDAR_PROTOCOL_START_SYM) &&\
					(lidar_rxTempData[LIDAR_ADDR_BYTE_POS] == lidar_rspVal(lidar_addr)) &&\
					(lidar_rxTempData[LIDAR_PROTOCOL_CMD_BYTE_POS] == lidar_rspVal(LIDAR_INIT_N_RESET));
			lidar_rxBufferToHandleBytes = (lidar_rxTempData[LIDAR_PROTOCOL_LEN_LBYTE_POS] |\
										  (lidar_rxTempData[LIDAR_PROTOCOL_LEN_HBYTE_POS]<<8)) -1;//CMD-Eklusiv
			break;
		default:
			check = 0;
			break;
	}
	if (check){
		memcpy((uint8_t*)(&lidar_rxBuffer[lidar_rxBufferIdx]),(uint8_t*)lidar_rxTempData,lidar_rxTempLength);
		lidar_rxBufferIdx += lidar_rxTempLength;
		lidar_rxRountine();
	} else{
		lidar_mgr.syncStatus = 0;
		lidar_status.dataBf.lineStatus = 1;
		retVal = LIDAR_PARAM_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t lidar_paramRXSHandlerFunc(){
	uint8_t retVal = LIDAR_PARAM_FSM_RX_STATE;
	if (lidar_expectedRxBytes == lidar_rxTempLength){
		lidar_rxBufferToHandleBytes -= lidar_rxTempLength;
		memcpy((uint8_t*)&lidar_rxBuffer[lidar_rxBufferIdx], (uint8_t*)lidar_rxTempData, (uint8_t)lidar_rxTempLength);
		lidar_rxBufferIdx += lidar_rxTempLength;
		if (lidar_rxBufferToHandleBytes){
			lidar_rxRountine();
		} else{
			USART_set_Bytes_to_receive(lidar_mgr.usartNo, 2);
			retVal = LIDAR_PARAM_FSM_RX_CHECK_2_OHD_STATE;
		}
	} else{
		lidar_mgr.syncStatus = 0;
		lidar_status.dataBf.lineStatus = 1;
		retVal = LIDAR_PARAM_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t lidar_paramRX2ndCheckSHandlerFunc(){
	uint8_t check = lidar_rxTempLength == 2;
	if (check){
		lidar_tempChecksumVal = (lidar_rxTempData[1]<<8)|lidar_rxTempData[0];
		lidar_status.dataBf.lineStatus = 0;
	} else{
		lidar_status.dataBf.lineStatus = 1;
	}
	lidar_mgr.syncStatus = 0;
	return LIDAR_PARAM_FSM_END_STATE;
}

static uint8_t lidar_paramEndSHandlerFunc(){
	return LIDAR_PARAM_FSM_END_STATE;
}

//Reset
static uint8_t lidar_resetStartSHandlerFunc(){
	lidar_protocolToHandleBytes = lidar_setProtocol(0x10,0);
	lidar_sendProtocol();
	return LIDAR_RESET_FSM_RX_STATE;
}

static uint8_t lidar_resetRxSHandlerFunc(){
	lidar_rxBufferToHandleBytes = LIDAR_RESET_RSP_LEN + 1;
	lidar_rxRountine();
	lidar_txTempData[0] = NULL;
	return LIDAR_RESET_FSM_TERMINAL_STATE;
}

static uint8_t lidar_resetTerminalSHandlerFunc(){
	lidar_mgr.resetStatus = 0;
	lidar_rxBufferIdx = 0;
	return LIDAR_RESET_FSM_END_STATE;
}

static uint8_t lidar_resetEndSHandlerFunc(){
	return LIDAR_RESET_FSM_END_STATE;
}

//Getter
static uint8_t lidar_getterStartSHandlerFunc(){
	lidar_mode = LIDAR_GETTER_MODE;
	lidar_mgr.rxStatus = 1;
	return LIDAR_GETTER_FSM_TX_STATE;
}

#pragma GCC push_options
#pragma GCC optimize("O3")
static uint8_t lidar_getterTXSHandlerFunc(){
	uint8_t retVal = LIDAR_GETTER_FSM_TX_STATE;
	if (lidar_protocolToHandleBytes){
		lidar_txTempData[0] = (uint8_t*)&(lidar_protocol[lidar_protocolIdx]);
		if(lidar_protocolToHandleBytes < lidar_txTempMax_length){
			*lidar_txTempLength = lidar_protocolToHandleBytes;
			lidar_protocolToHandleBytes = 0;
		} else{
			*lidar_txTempLength = lidar_txTempMax_length;
			lidar_protocolToHandleBytes -= lidar_txTempMax_length;
			lidar_protocolIdx += lidar_txTempMax_length;
		}
	} else{
		USART_set_Bytes_to_receive(lidar_mgr.usartNo,1);//Empfangen ACK/NACK Byte
		lidar_protocolIdx = 0;
		retVal = LIDAR_GETTER_FSM_RX_CHECK_1_OHD_STATE;
	}
	return retVal;
}

static uint8_t lidar_getterRX1stCheckSHandlerFunc(){
	uint8_t retVal = LIDAR_GETTER_FSM_RX_CHECK_2_OHD_STATE;
	uint8_t check = (lidar_rxTempLength == 1) && (lidar_rxTempData[0] == ASCII_ACK_CHAR);
	if (check){
		USART_set_Bytes_to_receive(lidar_mgr.usartNo,5);//Empfangen STX (1 Byte), ADDR (1 Byte), LEN (2 Byte), CMD (1 Byte)
	} else{
		lidar_mgr.rxStatus = 0;
		lidar_status.dataBf.lineStatus = 1;
		 //TODO add 1 flag for software status, to debug
		retVal = LIDAR_GETTER_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t lidar_getterRX2ndCheckSHandlerFunc(){
	uint8_t retVal = LIDAR_GETTER_FSM_RX_DATA_STATE;
	if (lidar_rxTempLength == 5){
		uint8_t check = (lidar_rxTempData[LIDAR_START_BYTE_POS] == LIDAR_PROTOCOL_START_SYM) &&\
						(lidar_rxTempData[LIDAR_ADDR_BYTE_POS] == lidar_rspVal(lidar_addr)) &&\
						(lidar_rxTempData[LIDAR_PROTOCOL_CMD_BYTE_POS] == lidar_rspVal(lidar_protocol[LIDAR_PROTOCOL_CMD_BYTE_POS]));
		if (check){
			memcpy((uint8_t*)&(lidar_rxBuffer[lidar_rxBufferIdx]),(uint8_t*)lidar_rxTempData,lidar_rxTempLength);
			lidar_rxBufferIdx += lidar_rxTempLength;
			lidar_rxBufferToHandleBytes = (lidar_rxTempData[LIDAR_PROTOCOL_LEN_LBYTE_POS] |\
										  (lidar_rxTempData[LIDAR_PROTOCOL_LEN_HBYTE_POS]<<8)) - 1;//CMD-Eklusiv
			lidar_rxRountine();
		} else{
			lidar_mgr.rxStatus = 0;
			lidar_status.dataBf.lineStatus = 1;
			retVal = LIDAR_GETTER_FSM_END_STATE;	
		}
	} else{
		lidar_mgr.rxStatus = 0;
		lidar_status.dataBf.lineStatus = 1; //TODO add 1 flag for software status, to debug
		retVal = LIDAR_GETTER_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t lidar_getterRXDataSHandlerFunc(){
	uint8_t retVal = LIDAR_GETTER_FSM_RX_DATA_STATE;
	if (lidar_expectedRxBytes == lidar_rxTempLength){
		lidar_rxBufferToHandleBytes -= lidar_rxTempLength;
		memcpy((uint8_t*)&lidar_rxBuffer[lidar_rxBufferIdx], (uint8_t*)lidar_rxTempData, (uint8_t)lidar_rxTempLength);
		lidar_rxBufferIdx += lidar_rxTempLength;
		if (lidar_rxBufferToHandleBytes){
			lidar_rxRountine();
		} else{
			USART_set_Bytes_to_receive(lidar_mgr.usartNo, 2);
			retVal = LIDAR_GETTER_FSM_RX_3_OHD_STATE;
		}
	} else{
		lidar_mgr.rxStatus = 0;
		lidar_status.dataBf.lineStatus = 1;//TODO add 1 flag for software status, to debug
		retVal = LIDAR_GETTER_FSM_END_STATE;
	}
	return retVal;
} 

static uint8_t lidar_getterRX3rdSHandlerFunc(){
	if (lidar_rxTempLength == 2){
		lidar_tempChecksumVal = (lidar_rxTempData[1]<<8)|lidar_rxTempData[0];
	} else{
		lidar_status.dataBf.lineStatus = 1;//TODO add 1 flag for software status, to debug
	}
	lidar_mgr.rxStatus = 0;
	return LIDAR_GETTER_FSM_END_STATE;
}

static uint8_t lidar_getterEndSHandlerFunc(){
	return LIDAR_GETTER_FSM_END_STATE;
}
#pragma GCC pop_options

//externe-FSM-Implementierung
static uint8_t lidar_mainSyncSHandler(){
	uint8_t retVal = LIDAR_MAIN_SYNC_STATE;
	if (lidar_mgr.init){
		//Parameterierung/Sync
		lidar_mode = LIDAR_PARAM_MODE;
		lidar_fsmState[LIDAR_PARAM_MODE] = lidar_allFsmLookuptable[LIDAR_PARAM_MODE][LIDAR_PARAM_FSM_START_STATE]();
		retVal = LIDAR_MAIN_SYNC_POLLING_STATE;
		//Set timeout
		timer_setCounter(180000UL);
	}
	return retVal;
}

static uint8_t lidar_mainSyncPollingSHandler(){
	uint8_t retVal = LIDAR_MAIN_SYNC_POLLING_STATE;
	if (lidar_mgr.syncStatus){
		if (timer_getCounter() < 0){
			timer_setState(0);
			lidar_status.dataBf.lineStatus = 1;
			lidar_fsmState[LIDAR_PARAM_MODE] = LIDAR_PARAM_FSM_END_STATE;//TODO try atomic
			lidar_mgr.syncStatus = 0;//TODO can nhac xem co nen tat khong hay la fail thi doi sync tiep luon
			retVal = LIDAR_MAIN_SYNC_DATA_CHECKING_STATE;
		}
	} else{
		timer_setState(0);
		retVal = LIDAR_MAIN_SYNC_DATA_CHECKING_STATE;//TODO to test, can nhac mot giai phap de quyet xem thang usvData duoc gui khi nao
	}
	return retVal;
}

static uint8_t lidar_mainSyncSignalCheckSHandler(){
	uint8_t retVal = LIDAR_MAIN_DATA_REQ_STATE;
	__asm__("nop");
	if (lidar_status.dataBf.lineStatus){
		retVal = LIDAR_MAIN_SYNC_STATE;
	} else{
		uint8_t checksum = lidar_checkRXData((uint8_t*)lidar_rxBuffer,lidar_rxBufferIdx, lidar_tempChecksumVal);
		if (checksum){
			lidar_status.dataBf.lidarStatus = lidar_rxBuffer[lidar_rxBufferIdx-1];
			memmove((uint8_t*)lidar_rxBuffer,(uint8_t*)&(lidar_rxBuffer[LIDAR_PROTOCOL_CMD_BYTE_POS+1]),lidar_rxBufferIdx-6);//Status-Byte Exklusiv
			//siehe "Telegramme zur Konfiguration und Bedienung der Lasermesssysteme LMS2xx-V2.30" S29
			uint8_t checkType= lidar_dataCheck((uint8_t*)&lidar_rxBuffer[LIDAR_INFO_TYPE_BYTE_POS],(const uint8_t*)LIDAR_TYPE_STR,LIDAR_TYPE_STR_LEN);
			uint8_t checkVer = lidar_dataCheck((uint8_t*)&lidar_rxBuffer[LIDAR_INFOR_VER_BYTE_POS],(const uint8_t*)LIDAR_VER_STR,LIDAR_VER_STR_LEN);
			//siehe "Telegramme zur Konfiguration und Bedienung der Lasermesssysteme LMS2xx-V2.30" S106
			uint8_t savedStatusByte = lidar_status.dataBf.lidarStatus;
			uint8_t checkError = (savedStatusByte & 0x07) < 3;
			if (checkType&&checkVer&&checkError){
				//wenn erfolgreich synchronisiert dann wird die Flaggen "reset" wird
				lidar_status.reg8[LIDAR_STATUS_MODULE_REG_TYPE] = (1<<0);
			} else{
				lidar_status.dataBf.fmwDevStatus = !(checkType&&checkVer);
				lidar_status.dataBf.devStatus = !checkError;
				retVal = LIDAR_MAIN_SYNC_STATE;
			}
		} else{
			lidar_status.dataBf.lineStatus = 1;
			retVal = LIDAR_MAIN_SYNC_STATE;
		}
	}
	if (lidar_ioStreamCheckAvai()){
		lidar_ioStream->val = (1<<STREAM_LIDAR_STATUS_BIT_POS);
	}
	lidar_rxBufferIdx = 0;
	return retVal;
}

static uint8_t lidar_mainDataReqSHandler(){
	uint8_t retVal = LIDAR_MAIN_RSP_POLLING_STATE;
	__asm__("nop");
	if (lidar_ioStreamCheckAvai() && lidar_checkRXBufferAvai() &&lidar_mgr.init){
		lidar_mgr.rxStatus = 1;
		static uint8_t dataContent = 0x01;//Subcommand send all value
		timer_setCounter(LIDAR_TX_TIME_US*LIDAR_RX_BUFFER_MAX_LEN/1000 + LIDAR_WRK_TIME_MS +\
						LIDAR_TOLERANCE_MS);
		lidar_dataGet(0,MEASURED_DATA_REQ,1,(uint8_t*)&dataContent,1);//magic num: Länge der Dateninhalt
		timer_setState(1);
	} else{
		retVal = LIDAR_MAIN_DATA_REQ_STATE;
	}
	return retVal;
}

static uint8_t lidar_mainRspPollingSHandler(){
	uint8_t retVal = LIDAR_MAIN_RSP_POLLING_STATE;
	if (lidar_mgr.rxStatus){
		if (timer_getCounter() < 0){
			timer_setState(0);
			lidar_mgr.rxStatus = 0;
			lidar_status.dataBf.lineStatus = 1;
			lidar_fsmState[LIDAR_GETTER_MODE] = LIDAR_GETTER_FSM_END_STATE;
			retVal = LIDAR_MAIN_DATA_CHECK_STATE;
		}
	} else{
		timer_setState(0);
		lidar_mgr.rxStatus = 0;
		retVal = LIDAR_MAIN_DATA_CHECK_STATE;
	}
	return retVal;
}

static uint8_t lidar_mainDataCheckSHandler(){
	uint8_t retVal = LIDAR_MAIN_DATA_REQ_STATE;
	static uint8_t checkData = 0;
	static uint8_t checkFcn = 0;
	static uint8_t check = 0;

	__asm__("nop");
	if (lidar_status.dataBf.lineStatus){
		retVal = LIDAR_MAIN_ERROR_STATE;
	} else {
		uint8_t savedLidarState = lidar_status.dataBf.lidarStatus;//Werte von Bit 7,3,2,1 gespeichert
		checkData = !(savedLidarState&0x80);//check bit 7: Data plausible
		checkFcn = (savedLidarState&0x07)<3;//check bit 1,2,3: Lidar Functionality state
		check = lidar_checkRXData((uint8_t*)lidar_rxBuffer,lidar_rxBufferIdx,lidar_tempChecksumVal);
		if (check&&checkData&&checkFcn){
			uint16_t configData = (lidar_rxBuffer[LIDAR_PROTOCOL_CMD_BYTE_POS + 1]) | (lidar_rxBuffer[LIDAR_PROTOCOL_CMD_BYTE_POS+2] << 8);
			check = ((configData&LIDAR_SET_CONFIG_BM) == LIDAR_SET_CONFIG);
			if (check){
				//STX (1 Byte), Addr (1 Byte), Len (2 Bytes), CMD (1 Byte), DataConfig (2 Bytes), Status (1 Byte)
				(*lidar_rxBufferStrLen) = lidar_rxBufferIdx - 6 - 2;
				lidar_status.dataBf.lidarStatus = lidar_rxBuffer[lidar_rxBufferIdx-1];
				//Status-Byte Exklusiv
				memmove((uint8_t*)lidar_rxBuffer,(uint8_t*)&(lidar_rxBuffer[LIDAR_PROTOCOL_CMD_BYTE_POS+1+2]),lidar_rxBufferIdx-6-2);
			} else{
				retVal = LIDAR_MAIN_SYNC_STATE;
				(*lidar_rxBufferStrLen) = 0;
				lidar_status.dataBf.configStatus = 1;
			}
		} else{
			(*lidar_rxBufferStrLen) = 0;
			lidar_status.dataBf.lineStatus = !check;
			lidar_status.dataBf.devStatus = !(checkData&&checkFcn);
			retVal = LIDAR_MAIN_ERROR_STATE;
		}
	}
	if (lidar_ioStreamCheckAvai()){
		lidar_ioStream->val = (1<<STREAM_LIDAR_STATUS_BIT_POS);
	}
	lidar_rxBufferIdx = 0;
	return retVal;
}

static uint8_t lidar_mainResetSHandler(){
	lidar_status.dataBf.sync = 0;
	lidar_mgr.syncStatus = 1;
	lidar_mgr.resetStatus = 1;
	uint8_t cancelCMD = ASCII_ETX_CHAR;
	lidar_mode = LIDAR_RESET_MODE;
	lidar_fsmState[LIDAR_RESET_MODE] = LIDAR_RESET_FSM_START_STATE;
	timer_setCounter(2000UL);
	timer_setState(1);
	USART_send_Array(lidar_mgr.usartNo,0,&cancelCMD,1);//Stoppen aller Übertragung
	return LIDAR_MAIN_RESET_POLLING_STATE;
}

static uint8_t lidar_mainResetPollingSHandler(){
	uint8_t retVal = LIDAR_MAIN_RESET_POLLING_STATE;
	if (lidar_mgr.resetStatus){
		if (timer_getCounter<=0){
			timer_setState(0);
			lidar_rxBufferIdx = 0;
			lidar_status.dataBf.lineStatus = 1;
			lidar_fsmState[LIDAR_RESET_MODE] = LIDAR_RESET_FSM_END_STATE;//TODO make that atomic
			retVal = LIDAR_MAIN_SYNC_STATE;
		}
	} else{
		timer_setState(0);
		lidar_rxBufferIdx = 0;
		retVal = LIDAR_MAIN_SYNC_STATE;
	}
	return retVal;
}

static uint8_t lidar_mainErrorSHandler(){
	uint8_t retVal = LIDAR_MAIN_DATA_REQ_STATE;
	lidar_rxBufferIdx = 0;
	lidar_tryTime--;
	if (lidar_tryTime < 0){
		lidar_tryTime = 1;
		retVal = LIDAR_MAIN_RESET_STATE;
	}
	return retVal;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//in ISR zurückgerufte Funktionen
static bool lidar_callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length, uint8_t max_length){
	lidar_txTempLength = length;
	lidar_txTempMax_length = max_length;
	//FSM-State-Handler 
	tempState = lidar_fsmState[lidar_mode];
	lidar_fsmState[lidar_mode] = lidar_allFsmLookuptable[lidar_mode][tempState]();
	if (lidar_txTempData[0] != NULL){
		*data = (uint8_t*)lidar_txTempData[0];
	}
	return true;
}

static bool lidar_callbackRx(uint8_t adress, uint8_t data[], uint8_t length){
	lidar_rxTempLength = length;
	lidar_rxTempData = data;
	//FSM-State-Handler 
	tempState = lidar_fsmState[lidar_mode];
	lidar_fsmState[lidar_mode] = lidar_allFsmLookuptable[lidar_mode][tempState]();
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//externe Funktionen
uint8_t lidar_initDev(const usartConfig_t* config, uint16_t crc16Polynom, uint8_t* output_p, uint16_t* outLen_p, reg8Model_t* io_p){
	uint8_t result = NO_ERROR;
	uint8_t check = (config!=NULL) && (output_p!=NULL) && (outLen_p!=NULL) && (io_p != NULL) &&\
					(*outLen_p >= LIDAR_RX_BUFFER_MAX_LEN);
	if (check){
		uint8_t temp = config->usartNo;
		check = check && !USART_init(temp, config->baudrate, config->usartChSize, config->parity,config->stopbit, config->sync, config->mpcm, config->address, config->portMux);
		if (check){
			lidar_mgr.init = 1;
			lidar_mgr.usartNo = temp;
			lidar_checksumPolynom = crc16Polynom;
			lidar_rxBuffer = output_p;
			lidar_rxBufferStrLen = outLen_p;
			lidar_ioStream = io_p;
			lidar_mode = LIDAR_PARAM_MODE;
			//Zuweisung der Rückruf vom Lidar
			USART_set_send_Array_callback_fnc(temp,&lidar_callbackTx);
			USART_set_receive_Array_callback_fnc(temp,&lidar_callbackRx);
			*outLen_p = 0;
			lidar_rxBufferIdx = 0;
		} else{
			result = PROCESS_FAIL;
		}
	} else{
		result = PROCESS_FAIL;
	}
	return result;
}

const lidarStatus_t* lidar_getStatus(){
	return (lidarStatus_t*)&lidar_status;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
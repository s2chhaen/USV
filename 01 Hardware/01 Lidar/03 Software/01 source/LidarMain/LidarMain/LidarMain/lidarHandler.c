/*
 * lidarHandler.c
 *
 * Created: 11/26/2023 4:06:34 PM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.0
 */ 

#include "lidarHandler.h"

/*Konstante Variable von USART-FIFO*/
static const uint16_t usartFIFOMaxLen = _FIFO_max_def - 2;
/*Lokalvariable für Lidar*/
static uint8_t lidar_addr = LIDAR_PROTOCOL_DEFAULT_ADDR;
volatile uint16_t lidar_tempChecksumVal = 0;
//Lokalvariable für Protokoll
volatile uint8_t lidar_protocol[LIDAR_REQ_PROTOCOL_MAX_LEN] = {0};
volatile uint8_t lidar_protocolToHandleBytes = 0;
volatile uint8_t lidar_protocolIdx = 0; //Idx: Index
//Lokalvariable für RX-Buffer (Ausgabe)
volatile uint8_t* lidar_rxBuffer;
volatile uint16_t* lidar_rxBufferStrLen;
volatile uint16_t lidar_rxBufferToHandleBytes = 0;
volatile uint16_t lidar_rxBufferIdx = 0;
volatile uint8_t lidar_expectedRxBytes = 0;
//Lokalvariable für TX-Buffer
static uint8_t lidar_txDataBuffer[10];
//Lokalvariable zur Modul-Verwaltung
volatile lidarMgr_t lidar_mgr = {0};
volatile lidarStatus_t  lidar_status = {0};
volatile usartConfig_t lidar_comParam = {0};
static int8_t lidar_tryTime = 1; 
volatile uint16_t lidar_checksumPolynom = 0;

volatile reg8Model_t* lidar_ioStream = NULL;
volatile uint8_t lidar_programPos = COM_PROGRAMM_NORMAL_POS; 

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

//Gesamte Look-up-Tabelle für Zustand-Handler-Routinen vom allen FSM
lidar_fsmStateHandlerFunc_t* lidar_allFsmLookuptable[LIDAR_MODE_NUM] = {
	(lidar_fsmStateHandlerFunc_t*)&lidar_paramtCbTable,
	(lidar_fsmStateHandlerFunc_t*)&lidar_resetCbTable,
	(lidar_fsmStateHandlerFunc_t*)&lidar_getterCbTable
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//externe FSM- Deklaration
static uint8_t lidar_mainSyncSHandler();
static uint8_t lidar_mainSyncPollingSHandler();
static uint8_t lidar_mainSyncSignalCheckSHandler();
static uint8_t lidar_mainDataReqSHandler();
static uint8_t lidar_mainRspPollingSHandler();
static uint8_t lidar_mainDataCheckSHandler();
static uint8_t lidar_mainResetSHandler();
static uint8_t lidar_mainResetPollingSHandler();
static uint8_t lidar_mainErrorSHandler();

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
	/* siehe "Telegramme zur Konfiguration und Bedienung der Lasermesssysteme LMS2xx-V2.30"
	 * für weitere Informationen*/
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

static inline uint8_t lidar_rspVal(uint8_t cmdOAddr){
	return (cmdOAddr+0x80);
}

static inline void lidar_rxRountine(){
	if (lidar_rxBufferToHandleBytes <= usartFIFOMaxLen){
		lidar_expectedRxBytes = lidar_rxBufferToHandleBytes;
		USART_set_Bytes_to_receive(lidar_comParam.usartNo, lidar_rxBufferToHandleBytes);
	} else{
		lidar_expectedRxBytes = (uint8_t)usartFIFOMaxLen;
		USART_set_Bytes_to_receive(lidar_comParam.usartNo, usartFIFOMaxLen);
	}	
}

static inline uint8_t lidar_checkRXData(uint8_t* data, uint16_t dataLen, uint16_t rxChecksumValue){
	return (lidar_checksum16(data,dataLen)==rxChecksumValue);
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
	int16_t retVal;//keine Optimierung
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
		USART_send_Array(lidar_comParam.usartNo,0,(uint8_t*)lidar_protocol,temp1);
	} else{
		lidar_protocolToHandleBytes -= usartFIFOMaxLen;
		lidar_protocolIdx = usartFIFOMaxLen;
		USART_send_Array(lidar_comParam.usartNo,0,(uint8_t*)lidar_protocol,usartFIFOMaxLen);
	}
}

static inline uint8_t lidar_ioStreamStatusAvai(){
	return !(lidar_ioStream->val & (1 << STREAM_LIDAR_STATUS_BIT_POS));
}

static inline uint8_t lidar_ioStreamDataAvai(){
	return !(lidar_ioStream->val & (1 << STREAM_LIDAR_DATA_BIT_POS));
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
/* Parametrierung/Synchronisierung */
static uint8_t lidar_paramStartSHandlerFunc(){
	USART_set_Bytes_to_receive(lidar_comParam.usartNo, 1);//checken des Floating-Pins
	return LIDAR_PARAM_FP_HANDLE_STATE;
}

static uint8_t lidar_paramFPHandleSHandlerFunc(){
	uint8_t retVal = LIDAR_PARAM_FP_HANDLE_STATE;
	uint8_t check = (lidar_programPos == COM_PROGRAMM_RX_POS) && (lidar_rxTempLength == 1);
	if (check){
		if (lidar_rxTempData[0] == LIDAR_PROTOCOL_START_SYM){
			lidar_status.reg8[LIDAR_STATUS_MODULE_REG_TYPE] = 0;
			lidarTimer_setState(1);
			lidar_rxBuffer[lidar_rxBufferIdx] = LIDAR_PROTOCOL_START_SYM;
			lidar_rxBufferIdx++;
			retVal = LIDAR_PARAM_FSM_RX_CHECK_1_OHD_STATE;
			USART_set_Bytes_to_receive(lidar_comParam.usartNo, 4);
		} else{
			USART_set_Bytes_to_receive(lidar_comParam.usartNo, 1);
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
	uint8_t check = (lidar_programPos == COM_PROGRAMM_RX_POS) && (lidar_rxTempLength == 4) &&\
					(lidar_rxTempData[LIDAR_ADDR_BYTE_POS-1] == lidar_rspVal(lidar_addr)) &&\
					(lidar_rxTempData[LIDAR_PROTOCOL_CMD_BYTE_POS-1] == lidar_rspVal(LIDAR_INIT_N_RESET));
	if (check){
		lidar_rxBufferToHandleBytes = (lidar_rxTempData[LIDAR_PROTOCOL_LEN_LBYTE_POS-1] |\
									  (lidar_rxTempData[LIDAR_PROTOCOL_LEN_HBYTE_POS-1]<<8)) - 1;//CMD-Eklusiv
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
	uint8_t check = (lidar_programPos == COM_PROGRAMM_RX_POS) &&\
					(lidar_expectedRxBytes == lidar_rxTempLength);
	if (check){
		lidar_rxBufferToHandleBytes -= lidar_rxTempLength;
		memcpy((uint8_t*)&lidar_rxBuffer[lidar_rxBufferIdx], (uint8_t*)lidar_rxTempData,\
			   (uint8_t)lidar_rxTempLength);
		lidar_rxBufferIdx += lidar_rxTempLength;
		if (lidar_rxBufferToHandleBytes){
			lidar_rxRountine();
		} else{
			USART_set_Bytes_to_receive(lidar_comParam.usartNo, 2);
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
	uint8_t check = (lidar_programPos == COM_PROGRAMM_RX_POS) && (lidar_rxTempLength == 2);
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

//Reset- Rücksetzen
static uint8_t lidar_resetStartSHandlerFunc(){
	uint8_t retVal = LIDAR_RESET_FSM_RX_STATE;
	uint8_t check = (lidar_programPos == COM_PROGRAMM_TX_POS);
	if (check){
		lidar_protocolToHandleBytes = lidar_setProtocol(0x10,0);
		lidar_sendProtocol();
	} else{
		retVal = LIDAR_RESET_FSM_END_STATE;
		lidar_mgr.resetStatus = 0;
		lidar_rxBufferIdx = 0;
	}
	return retVal;
}

static uint8_t lidar_resetRxSHandlerFunc(){
	uint8_t retVal = LIDAR_RESET_FSM_TERMINAL_STATE;
	uint8_t check = (lidar_programPos == COM_PROGRAMM_TX_POS);
	if (check){
		lidar_rxBufferToHandleBytes = LIDAR_RESET_RSP_LEN + 1;
		lidar_rxRountine();
	} else{
		retVal = LIDAR_RESET_FSM_END_STATE;
		lidar_mgr.resetStatus = 0;
		lidar_rxBufferIdx = 0;
	}
	lidar_txTempData[0] = NULL;
	return retVal;
}

static uint8_t lidar_resetTerminalSHandlerFunc(){
	lidar_mgr.resetStatus = 0;
	return LIDAR_RESET_FSM_END_STATE;
}

static uint8_t lidar_resetEndSHandlerFunc(){
	return LIDAR_RESET_FSM_END_STATE;
}

//Getter-Daten-RX- Datenempfangen
static uint8_t lidar_getterStartSHandlerFunc(){
	lidar_mode = LIDAR_GETTER_MODE;
	lidar_mgr.rxStatus = 1;
	return LIDAR_GETTER_FSM_TX_STATE;
}

#pragma GCC push_options
#pragma GCC optimize("O3")
static uint8_t lidar_getterTXSHandlerFunc(){
	uint8_t retVal = LIDAR_GETTER_FSM_TX_STATE;
	uint8_t check = (lidar_programPos == COM_PROGRAMM_TX_POS);
	if (check){
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
			USART_set_Bytes_to_receive(lidar_comParam.usartNo,1);//Empfangen ACK/NACK Byte
			lidar_protocolIdx = 0;
			retVal = LIDAR_GETTER_FSM_RX_CHECK_1_OHD_STATE;
		}
	} else{
		lidar_mgr.rxStatus = 0;
		lidar_status.dataBf.lineStatus = 1;
		retVal = LIDAR_GETTER_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t lidar_getterRX1stCheckSHandlerFunc(){
	uint8_t retVal = LIDAR_GETTER_FSM_RX_CHECK_2_OHD_STATE;
	uint8_t check = (lidar_programPos == COM_PROGRAMM_RX_POS) && (lidar_rxTempLength == 1) &&\
					(lidar_rxTempData[0] == ASCII_ACK_CHAR);
	if (check){
		//Empfangen STX (1 Byte), ADDR (1 Byte), LEN (2 Byte), CMD (1 Byte)
		USART_set_Bytes_to_receive(lidar_comParam.usartNo,5);
	} else{
		lidar_mgr.rxStatus = 0;
		lidar_status.dataBf.lineStatus = 1;
		retVal = LIDAR_GETTER_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t lidar_getterRX2ndCheckSHandlerFunc(){
	uint8_t retVal = LIDAR_GETTER_FSM_RX_DATA_STATE;
	uint8_t check = (lidar_programPos == COM_PROGRAMM_RX_POS) && (lidar_rxTempLength == 5);
	if (check){
		check = (lidar_rxTempData[LIDAR_START_BYTE_POS] == LIDAR_PROTOCOL_START_SYM) &&\
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
		lidar_status.dataBf.lineStatus = 1;
		retVal = LIDAR_GETTER_FSM_END_STATE;
	}
	return retVal;
}

static uint8_t lidar_getterRXDataSHandlerFunc(){
	uint8_t retVal = LIDAR_GETTER_FSM_RX_DATA_STATE;
	uint8_t check = (lidar_programPos == COM_PROGRAMM_RX_POS) &&\
					(lidar_expectedRxBytes == lidar_rxTempLength);
	if (check){
		lidar_rxBufferToHandleBytes -= lidar_rxTempLength;
		memcpy((uint8_t*)&lidar_rxBuffer[lidar_rxBufferIdx], (uint8_t*)lidar_rxTempData,\
			   (uint8_t)lidar_rxTempLength);
		lidar_rxBufferIdx += lidar_rxTempLength;
		if (lidar_rxBufferToHandleBytes){
			lidar_rxRountine();
		} else{
			USART_set_Bytes_to_receive(lidar_comParam.usartNo, 2);
			retVal = LIDAR_GETTER_FSM_RX_3_OHD_STATE;
		}
	} else{
		lidar_mgr.rxStatus = 0;
		lidar_status.dataBf.lineStatus = 1;
		retVal = LIDAR_GETTER_FSM_END_STATE;
	}
	return retVal;
} 

static uint8_t lidar_getterRX3rdSHandlerFunc(){
	uint8_t check = (lidar_programPos == COM_PROGRAMM_RX_POS) && (lidar_rxTempLength == 2);
	if (check){
		lidar_tempChecksumVal = (lidar_rxTempData[1]<<8)|lidar_rxTempData[0];
	} else{
		lidar_status.dataBf.lineStatus = 1;
	}
	lidar_mgr.rxStatus = 0;
	return LIDAR_GETTER_FSM_END_STATE;
}

static uint8_t lidar_getterEndSHandlerFunc(){
	return LIDAR_GETTER_FSM_END_STATE;
}
#pragma GCC pop_options


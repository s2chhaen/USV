/*
 * usvDataHandler.c
 *
 * Created: 7/7/2023 8:42:59 AM
 * Author: Thach
 * Version: 1.4
 * Revision: 1.2
 */

#include "usvDataHandler.h"

//Speichern der Max. Elementen vom USART-FIFO
const uint16_t usartFIFOMaxLen = _FIFO_max_def - 1;
//Protocolbereich
volatile uint8_t protocol[MAX_FRAME_LEN] = {0};
volatile uint8_t usv_protocolToHandleBytes = 0;
volatile uint8_t usv_protocolIdx = 0;
//Temp-Buffer zum Speichern der noch nicht gesendeten Daten
volatile uint8_t usv_tempBuffer[370] = {0};
volatile uint16_t usv_tempBufferToHandleBytes = 0;
volatile uint16_t usv_tempBufferIdx = 0; 
//Zeiger zu den externen Daten and dem Status-Register
static const uint8_t* usv_dataBuffer = NULL;
static const uint16_t* usv_dataBufferLen = NULL;
static const uint8_t* usv_statusBuffer = NULL;
static const uint8_t* usv_statusBufferLen = NULL;

//Verwaltungsbereich
volatile usvMgr_t usv_mgr = {0};
volatile usartConfig_t usv_comParam = {0};
volatile uint8_t usv_checksumPolynom = 0;
volatile uint16_t usv_nextReg = 0;
volatile uint8_t usv_savedAddr = 0;
volatile reg8Model_t* usv_ioStream = NULL;
static int8_t usv_tryTime = USV_RETRY_TIME_MAX;
static uint8_t usv_errorSrc = USV_NO_SRC;
volatile uint8_t usv_programPos = COM_PROGRAMM_NORMAL_POS; 

//temp-Var used to saved the param of callback function and bestimmen in welcher Art des Interruptes sind die FSM
//für usart TX - callback Funktion angewendet
volatile uint8_t* usv_txTempData[1] = {0}; 
volatile uint8_t* usv_txTempLength; 
volatile uint8_t usv_txTempMax_length;
//für usart RX - callback Funktion angewendet
volatile uint8_t* usv_rxTempData; 
volatile uint8_t usv_rxTempLength;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Interne FSM-Deklaration

//Info-Senden(Setter)-FSM
static uint8_t fsm_setterStartStateHandlerFunc();
static uint8_t fsm_setterTxStateHandlerFunc();
static uint8_t fsm_setterRxStateHandlerFunc();
static uint8_t fsm_setterEndSHandlerFunc();
volatile uint8_t usv_setterFsmState = USV_FSM_SETTER_START_STATE;
usv_fsmStateHandlerFunc_t usv_setterLookupTable[USV_FSM_STATE_NUM] = {
	&fsm_setterStartStateHandlerFunc,
	&fsm_setterTxStateHandlerFunc,
	&fsm_setterRxStateHandlerFunc,
	&fsm_setterEndSHandlerFunc
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Externe FSM-Deklaration
static uint8_t usv_mainFsmStartSHandlerFunc();
static uint8_t usv_mainFsmStatusTxSHandlerFunc();
static uint8_t usv_mainFsmStatusRspPollSHandlerFunc();
static uint8_t usv_mainFsmStatusRspCheckSHandlerFunc();
static uint8_t usv_mainFsmDataTxSHandlerFunc();
static uint8_t usv_mainFsmDataRspPollSHandlerFunc();
static uint8_t usv_mainFsmDataRspCheckStateFunc();
static uint8_t usv_mainFsmErrorSHandlerFunc();
usv_mainFsmStateHandlerFunc_t usv_mainFsmLookupTable[USV_MAIN_FSM_STATE_NUM] = {
	&usv_mainFsmStartSHandlerFunc,
	&usv_mainFsmStatusTxSHandlerFunc,
	&usv_mainFsmStatusRspPollSHandlerFunc,
	&usv_mainFsmStatusRspCheckSHandlerFunc,
	&usv_mainFsmDataTxSHandlerFunc,
	&usv_mainFsmDataRspPollSHandlerFunc,
	&usv_mainFsmDataRspCheckStateFunc,
	&usv_mainFsmErrorSHandlerFunc,
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//interne verwendete Funktionen
/**
 * \brief die Position des Registers in der Liste suchen
 * 
 * \param reg die Adresse des Registers
 * 
 * \return int8_t -1: kein gefunden, sonst: die Position in der Liste
 */
static inline int16_t getRegLen(uint16_t reg){
	int16_t result = 0;
	switch(reg){
		//Sensorblock
		case SB1_ADD:
			result = 1;//0
			break;
		case SB2_ADD:
			result = 4;//1
			break;
		case SB3_ADD:
			result = 4;//2
			break;
		case SB4_ADD:
			result = 1;//3
			break;
		case SB5_ADD:
			result = 2;//4
			break;
		case SB6_ADD:
			result = 2;//5
			break;
		case SB7_ADD:
			result = 3;//6
			break;
		//Radar
		case SB8_ADD:
			result = 1;//7
			break;
		case SB9_ADD:
			result = 2;//8
			break;
		//Führungsgrößen der Antriebsregelung
		case AF1_ADD:
			result = 8;//9
			break;
		case AF2_ADD:
			result = 8;//10
			break;
		case AF3_ADD:
			result = 2;//11
			break;
		case AF4_ADD:
			result = 2;//12
			break;
		//Stellgrößen der Antriebsregelung
		case AS1_ADD:
			result = 2;//13
			break;
		case AS2_ADD:
			result = 2;//14
			break;
		//lokaler Error Block
		case ER1_ADD:
			result = 1;//15
			break;
		case ER2_ADD:
			result = 1;//16
			break;
		case ER3_ADD:
			result = 1;//17
			break;
		case ER4_ADD:
			result = 2;//18
			break;
		case ER5_ADD:
			result = 1;//19
			break;
		//Lidar
		case ES1_ADD:
			result = 362;//20
			break;
		default:
			result = -1;
			break;
	}
	return result;
}

static inline bool checkRxData(uint8_t* data,uint8_t dataLen, uint8_t rxChecksumValue, uint8_t crc8Polynom){
	return crc8CodeGen(data,(uint16_t)dataLen)==rxChecksumValue;
}

static inline uint8_t usv_ioStreamStatusAvai(){
	return usv_ioStream->val & (1 << STREAM_LIDAR_STATUS_BIT_POS);
}

static inline uint8_t usv_ioStreamDataAvai(){
	return usv_ioStream->val & (1 << STREAM_LIDAR_DATA_BIT_POS);
}

static inline uint8_t usv_setProtocol(uint8_t add, uint16_t reg, uint8_t* input_p, uint8_t length, uint8_t wr){
	protocol[USV_START_BYTE_POS] = USV_PROTOCOL_START_BYTE;
	protocol[USV_OBJ_ID_BYTE_POS] = add;
	protocol[USV_REG_ADDR_AND_WR_LBYTE_POS] = USV_PROTOCOL_SET_SLAVE_ADD_LOW(reg);
	protocol[USV_REG_ADDR_AND_WR_HBYTE_POS] = USV_PROTOCOL_SET_SLAVE_ADD_HIGH(reg,wr);
	protocol[USV_FRAME_LEN_BYTE_POS] = length+PROTOCOL_OVERHEAD_LEN;
	memcpy((uint8_t*)&(protocol[USV_DATA_BEGIN_POS]),input_p,length);
	protocol[USV_DATA_BEGIN_POS+length] = crc8CodeGen(input_p,(uint16_t)length);
	protocol[USV_DATA_BEGIN_POS+1+length] = USV_PROTOCOL_END_BYTE;
	return USV_DATA_BEGIN_POS+1+length+1;//begin bei 0
}

static inline void usv_sendProtocol(){//trigger send new protocol
	uint8_t temp1 = 0;
	usv_protocolIdx = 0;
	if (usv_protocolToHandleBytes < usartFIFOMaxLen){
		temp1 = usv_protocolToHandleBytes;
		usv_protocolToHandleBytes = 0;
		USART_send_Array(usv_comParam.usartNo, 0, (uint8_t*)(&protocol[0]), temp1);
	} else{
		usv_protocolToHandleBytes -= usartFIFOMaxLen;
		usv_protocolIdx = usartFIFOMaxLen;
		USART_send_Array(usv_comParam.usartNo, 0, (uint8_t*)(uint8_t*)(&protocol[0]), usartFIFOMaxLen);
	}
}

static uint8_t usv_setRegister(uint8_t add, uint16_t reg, const uint8_t* input_p, uint16_t length){//Nur Protokoll senden
	uint8_t retVal = NO_ERROR;
	const uint16_t maxLen = getRegLen(USV_LAST_DATA_BLOCK_ADDR) + USV_LAST_DATA_BLOCK_ADDR;
	int16_t regLen = getRegLen(reg);
	uint8_t check = (length != 0) && ((reg+length) <= maxLen) && (regLen > 0);
	if (check){ //TODO test again
		usv_setterFsmState = usv_setterLookupTable[USV_FSM_SETTER_START_STATE]();
		usv_mgr.write = 1;
		usv_protocolIdx = 0;
		usv_savedAddr = add;
		memcpy((uint8_t*)usv_tempBuffer,input_p,length);
		if (length > PROTOCOL_PAYLOAD_PER_FRAME){
			usv_nextReg = reg + PROTOCOL_PAYLOAD_PER_FRAME;
			usv_protocolToHandleBytes = usv_setProtocol(add,reg,(uint8_t*)usv_tempBuffer,PROTOCOL_PAYLOAD_PER_FRAME,USV_PROTOCOL_W_REQ);
			usv_tempBufferIdx += PROTOCOL_PAYLOAD_PER_FRAME;
			usv_tempBufferToHandleBytes = length - PROTOCOL_PAYLOAD_PER_FRAME;
			} else{
			usv_nextReg = 0;
			usv_protocolToHandleBytes = usv_setProtocol(add,reg,(uint8_t*)usv_tempBuffer,length,USV_PROTOCOL_W_REQ);
			usv_tempBufferIdx = 0;
			usv_tempBufferToHandleBytes = 0;
		}
		usv_sendProtocol();
		} else{
		retVal = PROCESS_FAIL;
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//interne-FSM-Zustand-Handler-Implementierung
//Info-Senden(Setter)-FSM
static uint8_t fsm_setterStartStateHandlerFunc(){
	return USV_FSM_SETTER_TX_STATE;
}

static uint8_t fsm_setterTxStateHandlerFunc(){
	uint8_t retVal = USV_FSM_SETTER_TX_STATE;
	if (usv_programPos == COM_PROGRAMM_TX_POS){
		if (usv_protocolToHandleBytes){
			usv_txTempData[0] = (uint8_t*)&(protocol[usv_protocolIdx]);
			if (usv_protocolToHandleBytes < usv_txTempMax_length){
				*usv_txTempLength =  usv_protocolToHandleBytes;
				usv_protocolToHandleBytes = 0;
			} else{
				*usv_txTempLength = usv_txTempMax_length;
				usv_protocolToHandleBytes -= usv_txTempMax_length;
				usv_protocolIdx += usv_txTempMax_length;
			}
		} else{
			usv_protocolIdx = 0;
			retVal = USV_FSM_SETTER_RX_STATE;
			USART_set_Bytes_to_receive(usv_comParam.usartNo,1);
		}
	} else{
		usv_mgr.write = 0;
		usv_mgr.res = 1;
		retVal = USV_FSM_SETTER_END_STATE;
	}
	usv_rxTempLength = 0;
	return retVal;
}

static uint8_t fsm_setterRxStateHandlerFunc(){
	uint8_t retVal = USV_FSM_SETTER_END_STATE;
	uint8_t check = (usv_programPos == COM_PROGRAMM_RX_POS) && (usv_rxTempLength == 1) &&\
					(usv_rxTempData[0] == USV_PROTOCOL_ACK_BYTE);
	if (check){
		if (usv_tempBufferToHandleBytes){
			if (usv_tempBufferToHandleBytes < PROTOCOL_PAYLOAD_PER_FRAME){
				usv_protocolToHandleBytes = usv_setProtocol(usv_savedAddr, usv_nextReg,\
															(uint8_t*) (&usv_tempBuffer[usv_tempBufferIdx]),\
															usv_tempBufferToHandleBytes, USV_PROTOCOL_W_REQ);
				usv_tempBufferToHandleBytes = 0;
				usv_tempBufferIdx = 0;
			} else{
				usv_protocolToHandleBytes = usv_setProtocol(usv_savedAddr, usv_nextReg,\
															(uint8_t*) (&usv_tempBuffer[usv_tempBufferIdx]),\
															PROTOCOL_PAYLOAD_PER_FRAME,USV_PROTOCOL_W_REQ);
				usv_nextReg += PROTOCOL_PAYLOAD_PER_FRAME;
				usv_tempBufferIdx += PROTOCOL_PAYLOAD_PER_FRAME;
				usv_tempBufferToHandleBytes -= PROTOCOL_PAYLOAD_PER_FRAME;		
			}
			retVal = USV_FSM_SETTER_TX_STATE;
			usv_sendProtocol();
		} else{
			usv_mgr.write = 0;
			usv_mgr.res = 0;
		}
	} else{
		usv_mgr.write = 0;
		usv_mgr.res = 1;
		usv_tempBufferToHandleBytes = 0;
		usv_protocolToHandleBytes = 0;
	}
	usv_rxTempLength = 0;
	return retVal;
}

static uint8_t fsm_setterEndSHandlerFunc(){
	return USV_FSM_SETTER_END_STATE;
}

//externe-FSM-Implementierung
static uint8_t usv_mainFsmStartSHandlerFunc(){
	uint8_t retVal = USV_MAIN_FSM_START_STATE;
	uint8_t checkStatus = usv_ioStreamStatusAvai();
	uint8_t checkData = usv_ioStreamDataAvai();
	uint8_t check = (usv_mgr.init) && (checkStatus || checkData);
	usv_errorSrc = USV_NO_SRC;
	
	if (check){
		if (checkStatus){
			retVal = USV_MAIN_FSM_STATUS_TX_STATE;
		}
		if (checkData && (!checkStatus)){
			retVal = USV_MAIN_FSM_DATA_TX_STATE;
		}
	}
	return retVal;
}

static uint8_t usv_mainFsmStatusTxSHandlerFunc(){
	uint16_t regAdd = ER4_ADD;
	uint8_t regLen = *usv_statusBufferLen;
	uint8_t usvAddr = usv_mgr.usvAddr;
	usv_setRegister(usvAddr,regAdd,usv_statusBuffer,regLen);
	uint32_t timeOut_ms = (USV_BYTE_TRANSFER_TIME_US*(regLen+1)+USV_DST_PROG_WORK_TIME_US)/1000 +\
						  USV_TOLERANCE_MS;
	usvTimer_setCounter(timeOut_ms);
	usvTimer_setState(1);
	return USV_MAIN_FSM_STATUS_RSP_POLLING_STATE;
}

static uint8_t usv_mainFsmStatusRspPollSHandlerFunc(){
	uint8_t retVal = USV_MAIN_FSM_STATUS_RSP_POLLING_STATE;
	if (usv_mgr.write){
		if (usvTimer_getCounter() < 0){
			usvTimer_setState(0);
			usv_mgr.write = 0;
			usv_mgr.res = 1;
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				USART_flushRXFIFO(usv_comParam.usartNo);
				usv_setterFsmState = USV_FSM_SETTER_END_STATE;
			}
			retVal = USV_MAIN_FSM_STATUS_RSP_CHECK_STATE;
		}
	} else{
		usvTimer_setState(0);
		retVal = USV_MAIN_FSM_STATUS_RSP_CHECK_STATE;
	}
	return retVal;
}

static uint8_t usv_mainFsmStatusRspCheckSHandlerFunc(){
	uint8_t retVal = USV_MAIN_FSM_START_STATE;
	if (usv_mgr.res){
		usv_errorSrc = USV_STATUS_TX_SRC;
		retVal = USV_MAIN_FSM_ERROR_STATE;
	} else{
		PORTD.OUT &= ~(1<<2);//auschalten PD2, da Daten erfolgreich gesendet sind
		usv_ioStream->val &= ~(1<<STREAM_LIDAR_STATUS_BIT_POS);
	}
	return retVal;
}

static uint8_t usv_mainFsmDataTxSHandlerFunc(){
	uint16_t regAdd = ES1_ADD;
	uint16_t regLen = *usv_dataBufferLen;
	uint8_t usvAddr = usv_mgr.usvAddr;
	uint32_t timeOut_ms = (USV_BYTE_TRANSFER_TIME_US*(regLen+1)+USV_DST_PROG_WORK_TIME_US)/1000 +\
						  USV_TOLERANCE_MS;
	usvTimer_setCounter(timeOut_ms);
	usv_setRegister(usvAddr,regAdd,usv_dataBuffer,regLen);
	usvTimer_setState(1);
	return USV_MAIN_FSM_DATA_RSP_POLLING_STATE;
}

static uint8_t usv_mainFsmDataRspPollSHandlerFunc(){
	uint8_t retVal = USV_MAIN_FSM_DATA_RSP_POLLING_STATE;
	if (usv_mgr.write){
		if (usvTimer_getCounter() < 0){
			usvTimer_setState(0);
			usv_mgr.write = 0;
			usv_mgr.res = 1;
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				USART_flushRXFIFO(usv_comParam.usartNo);
				usv_setterFsmState = USV_FSM_SETTER_END_STATE;
			}
			retVal = USV_MAIN_FSM_DATA_RSP_CHECK_STATE;
		}
	} else{
		usvTimer_setState(0);
		retVal = USV_MAIN_FSM_DATA_RSP_CHECK_STATE;
	}
	return retVal;
}

static uint8_t usv_mainFsmDataRspCheckStateFunc(){
	uint8_t retVal = USV_MAIN_FSM_START_STATE;
	if (usv_mgr.res){
		usv_errorSrc = USV_DATA_TX_SRC;
		retVal = USV_MAIN_FSM_ERROR_STATE;
	} else{
		PORTD.OUT &= ~(1<<2);//auschalten PD2, da Daten erfolgreich gesendet sind
		usv_ioStream->val &= ~(1<<STREAM_LIDAR_DATA_BIT_POS);
	}
	return retVal;
}

static uint8_t usv_mainFsmErrorSHandlerFunc(){
	usv_mgr.res = 0;
	usv_tryTime--;
	if (usv_tryTime < 0){
		usv_tryTime = USV_RETRY_TIME_MAX;
		PORTD.OUT |= (1<<2);//Fehler-Leitung
	}
	switch (usv_errorSrc){
		case USV_NO_SRC:
			break;
		case USV_STATUS_TX_SRC:
			usv_ioStream->val &= ~(1<<STREAM_LIDAR_STATUS_BIT_POS);
			break;
		case USV_DATA_TX_SRC:
			usv_ioStream->val &= ~(1<<STREAM_LIDAR_DATA_BIT_POS);
			break;
		default:
			break;
	}
	return USV_MAIN_FSM_START_STATE;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//in ISR zurückgerufte Funktionen
static bool usartCallbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length, uint8_t max_length){
	usv_txTempLength = length;
	usv_txTempMax_length = max_length;
	usv_programPos = COM_PROGRAMM_TX_POS;
	usv_setterFsmState = usv_setterLookupTable[usv_setterFsmState]();
	if (usv_txTempData[0]!=NULL){
		*data = (uint8_t*)usv_txTempData[0];
	}
	
	usv_programPos = COM_PROGRAMM_NORMAL_POS;
	return true;
}

static bool usartCallbackRx(uint8_t adress, uint8_t data[], uint8_t length){
	usv_rxTempLength = length;
	usv_rxTempData = data;
	usv_programPos = COM_PROGRAMM_RX_POS;
	usv_setterFsmState = usv_setterLookupTable[usv_setterFsmState]();
	usv_programPos = COM_PROGRAMM_NORMAL_POS;
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//externe Funktionen

/**
 * \brief Initalisierung des Handlers
 * 
 * \param dev_p Zeiger zum zu initalisierenden Handler 
 * \param inputRxFunc_p der Zeiger zur Datenempfangen Funktion
 * \param inputTxFunc_p der Zeiger zur Datensenden Funktion
 * \param inputWaitFunc_p der Zeiger zur Warte Funktion
 * \param inputCrc8 der Checksum-CRC8 Polynom
 * 
 * \return uint8_t 0: keinen Fehler, sonst Fehler
 */
uint8_t usv_initDev(const usartConfig_t* config, uint8_t crc8Polynom, reg8Model_t* io_p,\
					const uint8_t* dataBuffer_p, const uint16_t* dataBufferLen_p,\
					const uint8_t* statusBuffer_p, const uint8_t* statusBufferLen_p){
	uint8_t result = NO_ERROR;
	uint8_t check = (config != NULL) && (io_p != NULL) && (dataBuffer_p != NULL) &&\
					(dataBufferLen_p != NULL) && (statusBuffer_p != NULL) &&\
					(statusBufferLen_p != NULL);
	if (check){
		uint8_t temp = config->usartNo;
		check = check && !USART_init(temp,config->baudrate, config->usartChSize, config->parity,\
									 config->stopbit, config->sync, config->mpcm, config->address,\
									 config->portMux);
		if (check){
			usv_mgr.init = 1;
			usv_comParam = (*config);
			usv_checksumPolynom = crc8Polynom;
			usv_ioStream = io_p;
			usv_dataBuffer = dataBuffer_p;
			usv_dataBufferLen = dataBufferLen_p;
			usv_statusBuffer = statusBuffer_p;
			usv_statusBufferLen = statusBufferLen_p;
			crc8Init(crc8Polynom);
			USART_set_send_Array_callback_fnc(temp,&usartCallbackTx);
			USART_set_receive_Array_callback_fnc(temp,&usartCallbackRx);
		} else{
			result = PROCESS_FAIL;
		}
	} else{
		result = PROCESS_FAIL;
	}
	return result;
}






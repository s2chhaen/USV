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







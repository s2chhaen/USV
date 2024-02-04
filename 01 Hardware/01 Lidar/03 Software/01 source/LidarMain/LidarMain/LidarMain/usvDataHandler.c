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






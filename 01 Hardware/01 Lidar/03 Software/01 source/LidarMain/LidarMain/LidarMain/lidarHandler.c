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


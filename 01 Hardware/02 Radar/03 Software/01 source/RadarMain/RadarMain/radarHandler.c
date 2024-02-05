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

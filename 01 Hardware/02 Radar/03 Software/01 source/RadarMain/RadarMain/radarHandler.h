/*
 * lidarHandler.h
 *
 * Created: 12/12/2023 11:53:20 AM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.0
 */ 


#ifndef RADARHANDLER_H_
#define RADARHANDLER_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include "ATMegaXX09/USART/USART.h"
#include "ATMegaXX09/FIFO/FIFO.h"
#include "errorList.h"
#include "timerUnit.h"
#include "asciiTable.h"
#include "registerModel.h"
#include "comConfig.h"

//Radar-bezogene Parameter
#define RADAR_SYNC_MSG "{\"Reset\":\"Board was reset.\x20\x20\'?R\' returns details\x22}\r\n"
#define RADAR_PROTOCOL_MAX_LEN 3
#define RADAR_DATA_REQ_CMD "PP"
#define RADAR_REQ_PROTOCOL_LEN (sizeof(RADAR_DATA_REQ_CMD)/sizeof(uint8_t) - 1)
#define RADAR_RESET_CMD "P!"
#define RADAR_RESET_PROTOCOL_LEN (sizeof(RADAR_RESET_CMD)/sizeof(uint8_t) - 1)
#define RADAR_DATA_REQ_MAX_PACK 2
#define RADAR_SYNC_PROTOCOL_START_SYM 0x7b
#define RADAR_DATA_PROTOCOL_START_SYM 0x22
#define RADAR_DATA_PROTOCOL_NEAR_END_SYM 0x0D
#define RADAR_PROTOCOL_END_SYM 0x0a

#define RADAR_VEL_RSP_LEN 11
#define RADAR_DIS_RSP_LEN 9
//Ausgabe Param für Main
#define RADAR_OUTPUT_IDEAL_LEN 3
//Arbeitszeit auf Radar-Seite
#define RADAR_TOLERANCE_MS 20
#define RADAR_WRK_TIME_MS 400
//config 1 Startbit, 8 Databit, 0 Paritybit, 1 Endbit, Baud = 38,4kBaud
#define RADAR_TX_TIME_US 521UL

/*enum: Arbeitsmodus vom Lidar*/
enum radar_mode{
	RADAR_SYNC_MODE,
	RADAR_GETTER_MODE
};
#define RADAR_LAST_MODE RADAR_GETTER_MODE
#define RADAR_MODE_NUM (RADAR_LAST_MODE+1)

/*Zustand-Deklaration für FSM*/
//Synchronisation-FSM
enum radar_syncFsmState{
	RADAR_SYNC_FSM_START_STATE,
	RADAR_SYNC_FSM_RESET_MSG_BEGIN_STATE,
	RADAR_SYNC_FSM_RESET_MSG_RX_STATE,
	RADAR_SYNC_FSM_DATA_MSG_BEGIN_STATE,
	RADAR_SYNC_FSM_DATA_MSG_RX_STATE,
	RADAR_SYNC_FSM_END_STATE
};
#define RADAR_SYNC_FSM_LAST_STATE RADAR_SYNC_FSM_END_STATE
#define RADAR_SYNC_FSM_STATE_NUM (RADAR_SYNC_FSM_LAST_STATE+1)

//Getter (Datenempfangen)-FSM
enum radar_dataReqFsmState{
	RADAR_DATA_FSM_START_STATE,
	RADAR_DATA_FSM_RX_PREP_STATE,
	RADAR_DATA_FSM_RX_BEGIN_STATE,
	RADAR_DATA_FSM_RX_STATE,
	RADAR_DATA_FSM_RX_TERMINAL_STATE,
	RADAR_DATA_FSM_END_STATE
};
#define RADAR_DATA_FSM_LAST_STATE RADAR_DATA_FSM_END_STATE
#define RADAR_DATA_FSM_STATE_NUM (RADAR_DATA_FSM_LAST_STATE+1)

//Handler-Funktion-Zeiger-Deklaration
typedef uint8_t (*const radar_fsmStateHandlerFunc_t)();

//in main-Funktion verwendete FSM
enum radar_mainFsmState{
	RADAR_MAIN_FSM_SYNC_STATE,
	RADAR_MAIN_FSM_SYNC_POLLING_STATE,
	RADAR_MAIN_FSM_SYNC_DATA_CHECK_STATE,
	RADAR_MAIN_FSM_DATA_REQ_STATE,
	RADAR_MAIN_FSM_RSP_POLLING_STATE,
	RADAR_MAIN_FSM_DATA_CHECK_STATE,
	RADAR_MAIN_FSM_RESET_STATE,
	RADAR_MAIN_FSM_ERROR_STATE
};
#define RADAR_MAIN_FSM_LAST_STATE RADAR_MAIN_FSM_ERROR_STATE
#define RADAR_MAIN_FSM_STATE_NUM (RADAR_MAIN_FSM_LAST_STATE+1)

//Handler-Funktion-Zeiger-Deklaration
typedef uint8_t (*const radar_mainFsmSHandlerFunc_t)();

//Daten-Buffer von Radar
#define RADAR_RX_STR_FIFO_LEN_BITS 1
#define RADAR_RX_STR_FIFO_LEN (1<<RADAR_RX_STR_FIFO_LEN_BITS)
#define RADAR_RX_STR_MAX_LEN 53
typedef struct{
	uint8_t data[RADAR_RX_STR_FIFO_LEN][RADAR_RX_STR_MAX_LEN];
	uint8_t dataStrIdx[RADAR_RX_STR_FIFO_LEN];
	uint8_t rPtr:RADAR_RX_STR_FIFO_LEN_BITS;
	uint8_t wPtr:RADAR_RX_STR_FIFO_LEN_BITS;
	uint8_t full:1;
	uint8_t empty:1;
}radarRxDataFIFO_t;

typedef union{
	uint8_t reg8;
	struct{
		uint8_t sync:1;//bit 0 => 0: nicht sync, 1: sync
		uint8_t fmwDevStatus:1;//bit 1 => 0: Gerät oder Firmware stimmt, 1: stimmt das Gerät und Firmware nicht
		uint8_t lineStatus:1;// bit 2 => 0: kein Fehler, 1: Fehler bei der Kommunikation
		uint8_t timeOut:1;// bit 3 => 0: kein Timeout-Fehler, 1: Timeout-Fehler
		uint8_t :1;// bit 4
		uint8_t :1;// bit 5
		uint8_t :1;// bit 6
		uint8_t :1;// bit 7
	}dataBf;
}radarStatus_t;

typedef struct{
	uint8_t init:1;
	uint8_t syncStatus:1;//0: nicht aktiv/fertig, 1: aktiv
	uint8_t rxStatus:1;//0: nicht aktiv/fertig, 1: aktiv
}radarMgr_t;

#endif /* RADARHANDLER_H_ */
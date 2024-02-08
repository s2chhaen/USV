/*
 * radarHandler.h
 *
 * Created: 12/12/2023 11:53:20 AM
 *  Author: Thach
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

//msg: message
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
//IO-Stream Bits
#define STREAM_RADAR_STATUS_BIT_POS 0
#define STREAM_RADAR_DATA_BIT_POS 1
//Nachkomma (Fraction-Part)-Param
#define VEL_FIXED_POINT_BIT 4
#define DIS_FIXED_POINT_BIT 4

enum radar_mode{
	RADAR_SYNC_MODE,
	RADAR_GETTER_MODE
};
#define RADAR_LAST_MODE RADAR_GETTER_MODE
#define RADAR_MODE_NUM (RADAR_LAST_MODE+1)

/*Zustand-Deklaration für FSM*/
//Sync-FSM
//FB: floating byte
enum radar_syncFsmState{
	RADAR_SYNC_FSM_START_STATE,//OK
	RADAR_SYNC_FSM_RESET_MSG_BEGIN_STATE,//OK
	RADAR_SYNC_FSM_RESET_MSG_RX_STATE,//OK
	RADAR_SYNC_FSM_DATA_MSG_BEGIN_STATE,//OK
	RADAR_SYNC_FSM_DATA_MSG_RX_STATE,//OK
	RADAR_SYNC_FSM_END_STATE //OK
};
#define RADAR_SYNC_FSM_LAST_STATE RADAR_SYNC_FSM_END_STATE
#define RADAR_SYNC_FSM_STATE_NUM (RADAR_SYNC_FSM_LAST_STATE+1)

//Getter-FSM
enum radar_dataReqFsmState{ //Prep: preparation/Vorbereitung
	RADAR_DATA_FSM_START_STATE,//OK OK
	RADAR_DATA_FSM_RX_PREP_STATE,//OK OK
	RADAR_DATA_FSM_RX_BEGIN_STATE,//OK OK
	RADAR_DATA_FSM_RX_STATE,//OK OK
	RADAR_DATA_FSM_RX_TERMINAL_STATE,//OK OK
	RADAR_DATA_FSM_END_STATE //OK
};
#define RADAR_DATA_FSM_LAST_STATE RADAR_DATA_FSM_END_STATE
#define RADAR_DATA_FSM_STATE_NUM (RADAR_DATA_FSM_LAST_STATE+1)

//Handler-Funktion-Zeiger-Deklaration
typedef uint8_t (*const radar_fsmStateHandlerFunc_t)();//Func: Function/Funktion

enum radar_mainFsmState{
	RADAR_MAIN_FSM_SYNC_STATE,//OK
	RADAR_MAIN_FSM_SYNC_POLLING_STATE,//OK
	RADAR_MAIN_FSM_SYNC_DATA_CHECK_STATE,//OK
	RADAR_MAIN_FSM_DATA_REQ_STATE,//OK
	RADAR_MAIN_FSM_RSP_POLLING_STATE,//OK
	RADAR_MAIN_FSM_DATA_CHECK_STATE,//OK
	RADAR_MAIN_FSM_RESET_STATE,//OK
	RADAR_MAIN_FSM_ERROR_STATE //OK
};
#define RADAR_MAIN_FSM_LAST_STATE RADAR_MAIN_FSM_ERROR_STATE
#define RADAR_MAIN_FSM_STATE_NUM (RADAR_MAIN_FSM_LAST_STATE+1)

//Handler-Funktion-Zeiger-Deklaration
typedef uint8_t (*const radar_mainFsmSHandlerFunc_t)();

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
		uint8_t sync:1;// bit 0 => 0: not sync, 1: sync
		uint8_t fmwDevStatus:1;// bit 1 => 0: Gerät stimmt nicht, 1: stimmt das Gerät
		uint8_t lineStatus:1;// bit 2 => 0: no error, 1: no active line or disturbance
		uint8_t :1;// bit 3
		uint8_t :1;// bit 4
		uint8_t :1;// bit 5
		uint8_t :1;// bit 6
		uint8_t :1;// bit 7
	}dataBf;//Bf: bitfield
}radarStatus_t;

/*Struktur für Konfiguration der USART-Einheit*/
typedef struct{
	uint8_t usartNo:2;
	uint32_t baudrate;
	uint8_t usartChSize;
	uint8_t parity;
	uint8_t stopbit;
	uint8_t sync:1;
	uint8_t mpcm:1;
	uint8_t address;
	uint8_t portMux:2;
}usartConfig_t;

typedef struct{
	uint8_t init:1;
	uint8_t usartNo:2;
	uint8_t lock:1;
	uint8_t syncStatus:1;// 0: not active, 1: in process
	uint8_t rxStatus:1;// 0: not active, 1: active
}radarMgr_t;

//extern
extern radar_mainFsmSHandlerFunc_t radar_mainFsmLookupTable[RADAR_MAIN_FSM_STATE_NUM];
//Vel: Velocity, Dis: Distance
extern uint8_t radar_initDev(const usartConfig_t* config, int8_t* outVel_p, uint16_t* outDis_p, reg8Model_t* io_p);
extern const radarStatus_t* radar_getStatus();



#endif /* RADARHANDLER_H_ */
/*
 * lidarHandler.h
 *
 * Created: 11/26/2023 4:03:53 PM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.1
 */ 


#ifndef LIDARHANDLER_H_
#define LIDARHANDLER_H_

#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include "ATMegaXX09/USART/USART.h"
#include "ATMegaXX09/FIFO/FIFO.h"
#include "Math/checksum.h"
#include "errorList.h"
#include "lidarHandlerProtocol.h"
#include "timerUnit.h"
#include "registerModel.h"
#include "comConfig.h"

/*Abkürzungen: wrk: working, rsp: response, Func: Function,
 *FP: floating-pin, OHD: Overhead, pos: position, len: length,
 *Prot:protocol, REQ: Request, fnc: functionality, bf: Bitfield
 */
/*Lidar-bezogene Parameter Macros*/
#define LIDAR_PROTOCOL_START_SYM 0x02
#define LIDAR_PROTOCOL_DEFAULT_ADDR 0x00
#define LIDAR_PROTOCOL_CANCEL_SYM 0x03
#define LIDAR_RESET_PROTOCOL_LEN 7
#define LIDAR_TOLERANCE_MS 20
#define LIDAR_WRK_TIME_MS 100
//config 1 Startbit, 8 Databit, 0 Paritybit, 1 Endbit, Baud = 38,4kBaud
#define LIDAR_TX_TIME_US 261UL
#define LIDAR_RSP_PROTOCOL_OFFSET 0x80
#define LIDAR_RSP_ADDR (LIDAR_PROTOCOL_DEFAULT_ADDR + LIDAR_RSP_PROTOCOL_OFFSET)
#define LIDAR_RSP_CMD(cmd) (cmd+0x80)
#define LIDAR_REQ_PROTOCOL_MAX_LEN 20
#define LIDAR_RESET_RSP_LEN 8
//Zwischenbuffer-Parameter
#define LIDAR_RX_BUFFER_MAX_LEN 735UL
//Lidar-Status
#define LIDAR_STATUS_REG_LEN_BYTE 1

/*enum: Arbeitsmodus vom Lidar*/
enum lidarMode{
	LIDAR_PARAM_MODE,
	LIDAR_RESET_MODE,
	LIDAR_GETTER_MODE
};
#define LIDAR_LAST_MODE LIDAR_GETTER_MODE
#define LIDAR_MODE_NUM (LIDAR_LAST_MODE+1)

/*Byte-Positionen im Protokoll*/
//Generell
enum lidarProtocolBytesPos{
	LIDAR_START_BYTE_POS, //byte 0
	LIDAR_ADDR_BYTE_POS, //byte 1
	LIDAR_PROTOCOL_LEN_LBYTE_POS, //byte 2
	LIDAR_PROTOCOL_LEN_HBYTE_POS, //byte 3
	LIDAR_PROTOCOL_CMD_BYTE_POS //byte 4
};
//Meldung-nach-ON-Protokoll
enum lidarPromptProtocolDataBytePos{
	LIDAR_INFO_TYPE_BYTE_POS = 0,
	LIDAR_INFOR_VER_BYTE_POS = 14
};

/*Zustand-Deklaration für FSM*/
//Parametrierung-FSM
enum lidar_paramFsmState{
	LIDAR_PARAM_FSM_START_STATE,
	LIDAR_PARAM_FP_HANDLE_STATE,
	LIDAR_PARAM_FSM_RX_CHECK_1_OHD_STATE,
	LIDAR_PARAM_FSM_RX_STATE,
	LIDAR_PARAM_FSM_RX_CHECK_2_OHD_STATE,
	LIDAR_PARAM_FSM_END_STATE
};
#define LIDAR_PARAM_FSM_LAST_STATE LIDAR_PARAM_FSM_END_STATE
#define LIDAR_PARAM_FSM_STATE_NUM (LIDAR_PARAM_FSM_LAST_STATE+1)

//Rücksetzen-FSM
enum lidar_resetFsmState{
	LIDAR_RESET_FSM_START_STATE,
	LIDAR_RESET_FSM_RX_STATE,
	LIDAR_RESET_FSM_TERMINAL_STATE,
	LIDAR_RESET_FSM_END_STATE
};
#define LIDAR_RESET_FSM_LAST_STATE LIDAR_RESET_FSM_END_STATE
#define LIDAR_RESET_FSM_STATE_NUM (LIDAR_RESET_FSM_LAST_STATE+1)

//Datenempfangen-FSM
enum lidar_getterFsmState{
	LIDAR_GETTER_FSM_START_STATE,
	LIDAR_GETTER_FSM_TX_STATE,
	LIDAR_GETTER_FSM_RX_CHECK_1_OHD_STATE,
	LIDAR_GETTER_FSM_RX_CHECK_2_OHD_STATE,
	LIDAR_GETTER_FSM_RX_DATA_STATE,
	LIDAR_GETTER_FSM_RX_3_OHD_STATE,
	LIDAR_GETTER_FSM_END_STATE
};
#define LIDAR_GETTER_FSM_LAST_STATE LIDAR_GETTER_FSM_END_STATE
#define LIDAR_GETTER_FSM_STATE_NUM (LIDAR_GETTER_FSM_LAST_STATE+1)

//Handler-Funktion-Zeiger-Deklaration
typedef uint8_t (*const lidar_fsmStateHandlerFunc_t)();

//in main-Funktion verwendete FSM
enum lidar_mainFsmState{
	LIDAR_MAIN_SYNC_STATE,
	LIDAR_MAIN_SYNC_POLLING_STATE,
	LIDAR_MAIN_SYNC_DATA_CHECKING_STATE,
	LIDAR_MAIN_DATA_REQ_STATE,
	LIDAR_MAIN_RSP_POLLING_STATE,
	LIDAR_MAIN_DATA_CHECK_STATE,
	LIDAR_MAIN_RESET_STATE,
	LIDAR_MAIN_RESET_POLLING_STATE,
	LIDAR_MAIN_ERROR_STATE
};
#define LIDAR_MAIN_FSM_LAST_STATE LIDAR_MAIN_ERROR_STATE
#define LIDAR_MAIN_FSM_STATE_NUM (LIDAR_MAIN_FSM_LAST_STATE+1)

//Handler-Funktion-Zeiger-Deklaration
typedef uint8_t (*const lidar_mainFsmSHandlerFunc_t)();

/*interne Verwaltungsregister*/
//Generell
typedef struct{
	uint8_t init:1;
	uint8_t syncStatus:1;//0: nicht aktive/fertig, 1: aktive
	uint8_t rxStatus:1;// 0: nicht aktive/fertig, 1: aktive
	uint8_t resetStatus:1;//0: nicht aktiv/fertig, 1: aktive
}lidarMgr_t;

//Prozessverwaltungsregister
typedef union{
	uint8_t  reg8;
	struct{ 
		uint8_t sync:1;//bit 0=> 0: nicht sync, 1: sync
		uint8_t fmwDevStatus:1;//bit 01=> 0: Gerät oder Firmware stimmt nicht, 1: stimmt das Gerät und Firmware
		uint8_t lineStatus:1;//bit 2=> 0: kein Fehler, 1: Fehler bei der Kommunikation
		uint8_t configStatus:1;//bit 3 => 0: kein Fehler, 1: falsche Konfiguration
		uint8_t devStatus:1;//bit 4 => 0: kein Fehler, 1: Fehler, siehe lidarStatus-Reg
		uint8_t timeOut:1;//bit 05 => 0: kein Timeout-Fehler, 1: Timeout-Fehler
		uint8_t :1;//bit 06 
		uint8_t :1;//bit 07
	}dataBf;
}lidarStatus_t;

/*Definition der in main-verwendete FSM*/
extern lidar_mainFsmSHandlerFunc_t lidar_mainFsmLookupTable[LIDAR_MAIN_FSM_STATE_NUM];

/*Extern verwendeten Funktion*/
extern uint8_t lidar_initDev(const usartConfig_t* config, uint16_t crc16Polynom, uint8_t* output_p, uint16_t* outLen_p, reg8Model_t* io_p);
extern const lidarStatus_t* lidar_getStatus();
extern void lidar_setAddr(uint8_t input);

#endif /* LIDARHANDLER_H_ */
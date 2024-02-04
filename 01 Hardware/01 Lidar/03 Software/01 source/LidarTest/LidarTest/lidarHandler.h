/*
 * lidarHandler.h
 *
 * Created: 11/26/2023 4:03:53 PM
 *  Author: Thach
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

/*Lidar-bezogene Parameter Macros*/
#define LIDAR_PROTOCOL_START_SYM 0x02
#define LIDAR_PROTOCOL_DEFAULT_ADDR 0x00
#define LIDAR_PROTOCOL_CANCEL_SYM 0x03
#define LIDAR_RESET_PROTOCOL_LEN 7
//wrk: working
#define LIDAR_TOLERANCE_MS 20
#define LIDAR_WRK_TIME_MS 90
//config 1 Startbit, 8 Databit, 0 Paritybit, 1 Endbit, Baud = 38,4kBaud
#define LIDAR_TX_TIME_US 261UL
//rsp: response
#define LIDAR_RSP_PROTOCOL_OFFSET 0x80
#define LIDAR_RSP_ADDR (LIDAR_PROTOCOL_DEFAULT_ADDR + LIDAR_RSP_PROTOCOL_OFFSET)
#define LIDAR_RSP_CMD(cmd) (cmd+0x80)
#define LIDAR_REQ_PROTOCOL_MAX_LEN 20
#define LIDAR_RESET_RSP_LEN 8
//Zwischenbuffer-param
#define LIDAR_RX_BUFFER_MAX_LEN 735UL
//IO-Bit-Position
#define STREAM_LIDAR_STATUS_BIT_POS 0
#define STREAM_LIDAR_DATA_BIT_POS 1


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

/*enum: Arbeitsmodus vom Lidar*/
enum lidar_mode{
	LIDAR_PARAM_MODE,
	LIDAR_RESET_MODE,
	LIDAR_GETTER_MODE
};
#define LIDAR_LAST_MODE LIDAR_GETTER_MODE
#define LIDAR_MODE_NUM (LIDAR_LAST_MODE+1)

/*Byte-Positionen im Protokoll*/
//Generell
enum lidarProtocolBytesPos{//pos: position
	LIDAR_START_BYTE_POS, //byte 0
	LIDAR_ADDR_BYTE_POS, //byte 1
	LIDAR_PROTOCOL_LEN_LBYTE_POS, //byte 2, len: length => len = data bytes len
	LIDAR_PROTOCOL_LEN_HBYTE_POS, //byte 3
	LIDAR_PROTOCOL_CMD_BYTE_POS //byte 4
};
//Meldung-nach-ON-Protokoll
enum lidarPromptProtocolDataBytePos{
	LIDAR_INFO_TYPE_BYTE_POS = 0,
	LIDAR_INFOR_VER_BYTE_POS = 14
};

/*Zustand-Deklaration für FSM*/
//Parameterierung-FSM
enum lidar_paramFsmState{
	//Besetzen das Lock und Checken das 1. Byte
	LIDAR_PARAM_FSM_START_STATE,
	//Eliminieren der Floating Bytes,wenn vorhanden
	LIDAR_PARAM_FP_HANDLE_STATE,//FP: Floating-pin
	//Warten, bis erst 4 Bytes ankommt und checken, empfangen weiter n Bytes (n in Länge-Bytes)
	LIDAR_PARAM_FSM_RX_CHECK_1_OHD_STATE,//ohd: overhead
	//Empfangen weitere 2 Bytes (CRC16-Byte)
	LIDAR_PARAM_FSM_RX_STATE,
	//checken mit CRC
	LIDAR_PARAM_FSM_RX_CHECK_2_OHD_STATE,
	//Ende des Checkens
	LIDAR_PARAM_FSM_END_STATE
};
#define LIDAR_PARAM_FSM_LAST_STATE LIDAR_PARAM_FSM_END_STATE
#define LIDAR_PARAM_FSM_STATE_NUM (LIDAR_PARAM_FSM_LAST_STATE+1)

//Reset-FSM
enum lidar_resetFsmState{//Prot:protocol
	LIDAR_RESET_FSM_START_STATE,//send 0x03 to cancel all communication
	LIDAR_RESET_FSM_RX_STATE,//send Reset Protocol and set to rx the response
	LIDAR_RESET_FSM_TERMINAL_STATE,//check if true then rx first byte and change it again into meldung nach on
	LIDAR_RESET_FSM_END_STATE
};
#define LIDAR_RESET_FSM_LAST_STATE LIDAR_RESET_FSM_END_STATE
#define LIDAR_RESET_FSM_STATE_NUM (LIDAR_RESET_FSM_LAST_STATE+1)

//Getter-FSM
enum lidar_getterFsmState{
	//Checken-Lock OK
	LIDAR_GETTER_FSM_START_STATE,
	//TX-Zustand => Am Ende empfangen 1 Byte OK
	LIDAR_GETTER_FSM_TX_STATE,
	//RX-Checken 1. Overhead - ACK oder nicht => Am Ende empfangen 4 Byte OK
	LIDAR_GETTER_FSM_RX_CHECK_1_OHD_STATE,
	//RX-Checken 2. Overhead - STX (1 Byte), Addr (1 Byte), Länge (2 Bytes)
	//=> Am Ende empfangen n Byte (von Länge-Bytes) OK
	LIDAR_GETTER_FSM_RX_CHECK_2_OHD_STATE,
	//Rx-Daten => Am Ende empfangen 2 Bytes OK
	LIDAR_GETTER_FSM_RX_DATA_STATE,
	//Checksum OK
	LIDAR_GETTER_FSM_RX_3_OHD_STATE,
	LIDAR_GETTER_FSM_END_STATE
};
#define LIDAR_GETTER_FSM_LAST_STATE LIDAR_GETTER_FSM_END_STATE
#define LIDAR_GETTER_FSM_STATE_NUM (LIDAR_GETTER_FSM_LAST_STATE+1)

//Handler-Funktion-Zeiger-Deklaration
typedef uint8_t (*const lidar_fsmStateHandlerFunc_t)();//Func: Function/Funktion

//in main-Funktion verwendete FSM
enum lidar_mainFsmState{//PSV: Passive, ACTV: active, RSP: response
	LIDAR_MAIN_SYNC_STATE,//OK OK
	LIDAR_MAIN_SYNC_POLLING_STATE,//OK OK
	LIDAR_MAIN_SYNC_DATA_CHECKING_STATE,//OK
	LIDAR_MAIN_DATA_REQ_STATE,//OK OK
	LIDAR_MAIN_RSP_POLLING_STATE,//OK OK
	LIDAR_MAIN_DATA_CHECK_STATE,//OK OK
	LIDAR_MAIN_RESET_STATE,//OK OK
	LIDAR_MAIN_RESET_POLLING_STATE,//OK OK
	LIDAR_MAIN_ERROR_STATE //OK OK
};
#define LIDAR_MAIN_FSM_LAST_STATE LIDAR_MAIN_ERROR_STATE
#define LIDAR_MAIN_FSM_STATE_NUM (LIDAR_MAIN_FSM_LAST_STATE+1)

//Handler-Funktion-Zeiger-Deklaration
typedef uint8_t (*const lidar_mainFsmSHandlerFunc_t)();

/*interne Verwaltungsregister*/
//Generell
typedef struct{
	uint8_t init:1;
	uint8_t usartNo:2;
	uint8_t lock:1;
	uint8_t syncStatus:1;//0: not active, 1: active TODO replace it in Bachelorarbeit
	uint8_t rxStatus:1;// 0: not active, 1: active
	uint8_t resetStatus:1;//0: nicht aktiv/fertig, 1: Im Prozess
}lidarMgr_t;

//Prozessverwaltung
enum lidarStatusRegType{
	LIDAR_STATUS_MODULE_REG_TYPE,
	LIDAR_STATUS_SENSOR_REG_TYPE
};

typedef union{
	uint16_t reg16;
	uint8_t  reg8[2];
	struct{ //fnc: functionality
		uint8_t sync:1;//bit 00 => 0: not sync, 1: sync
		uint8_t fmwDevStatus:1;//bit 01 => 0: Gerät oder Firmware stimmt nicht, 1: stimmt das Gerät und Firmware
		uint8_t lineStatus:1;//bit 02 => 0: no error, 1: no active line or disturbance
		uint8_t configStatus:1;//bit 03 => 0: no error, 1: false config
		uint8_t devStatus:1;//bit 04 => 0: no error, 1: Data not plausible or Error, siehe lidarStatus-Reg
		uint8_t :1;//bit 05
		uint8_t :1;//bit 06 
		uint8_t :1;//bit 07 
		uint8_t lidarStatus:8;/*bit 8-15 => siehe "Telegramme zur Konfiguration und Bedienung der 
							   *Lasermesssysteme LMS2xx-V2.30" S106*/
	}dataBf;//bf: Bitfield
}lidarStatus_t;

/*Definition der in main-verwendete FSM*/
extern lidar_mainFsmSHandlerFunc_t lidar_mainFsmLookupTable[LIDAR_MAIN_FSM_STATE_NUM];

/*Funktion-Prototype*/
extern uint8_t lidar_initDev(const usartConfig_t* config, uint16_t crc16Polynom, uint8_t* output_p, uint16_t* outLen_p, reg8Model_t* io_p);
extern const lidarStatus_t* lidar_getStatus();

#endif /* LIDARHANDLER_H_ */
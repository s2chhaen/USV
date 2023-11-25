/*
 * usvDataHandler.h
 *
 * Created: 7/7/2023 5:25:53 AM
 * Author: Thach
 * Version: 1.3
 * Revision: 1.1
 */ 

//Refaktorisierung in Bearbeitung

#ifndef USVDATAHANDLER_H_
#define USVDATAHANDLER_H_

#define F_CPU 20000000
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ATMegaXX09/USART/USART.h"
#include "ATMegaXX09/FIFO/FIFO.h"
#include "Math/checksum.h"
#include "timerUnit.h"
#include "usvDataHandlerConfig.h"

#define USV_PROTOCOL_W_REQ 8
#define USV_PROTOCOL_R_REQ 4
#define USV_PROTOCOL_SET_SLAVE_ADD_LOW(add) (uint8_t)(add&0xff)
#define USV_PROTOCOL_SET_SLAVE_ADD_HIGH(add,rw) (uint8_t)((add>>8)|(rw<<4))
//Parameter des Datenrahmens
#define MAX_FRAME_LEN_BIT 8UL
#define MAX_FRAME_LEN ((1<<MAX_FRAME_LEN_BIT)-1)
#define PROTOCOL_OVERHEAD_LEN 7
#define PROTOCOL_PAYLOAD_PER_FRAME (MAX_FRAME_LEN - PROTOCOL_OVERHEAD_LEN)

//DST = destination, Arbeitsdauer vom Zielprogramm (MATLAB)
#define BYTE_TRANSFER_TIME_US 44
#define DST_PROG_WORK_TIME_US 150000
//Protokoll
#define USV_PROTOCOL_START_BYTE 0xA5
#define USV_PROTOCOL_END_BYTE 0xA6
#define USV_PROTOCOL_ACK_BYTE 0xA1
//Empfangen Byte
#define USV_1ST_RX_LEN 1
#define USV_2ND_RX_LEN 4

typedef enum {
	//Sensorblock
	SEN_GESB_ADD = 0x00,//00
	SEN_LONGNITUDE_ADD = 0x01,//01
	SEN_LATITUDE_ADD = 0x05,//02
	SEN_SATFIX_ADD = 0x09,//03
	SEN_GPS_VEL_ADD = 0x0A,//04
	SEN_COURSE_ANGLE_ADD = 0x0C,//05
	SEN_TIMESTAMP_ADD = 0x0E,//06
	//Radar
	RADAR_1_ADD = 0x12,//07
	RADAR_2_ADD = 0x13,//08
	//Führungsgrößen der Antriebsregelung
	REF_DRV_CTRL_REF_A_ADD = 0x100,//09
	REF_DRV_CTRL_REF_B_ADD = 0x108,//10
	REF_DRV_CTRL_VEL_ADD = 0x110,//11
	REF_DRV_CTRL_EPS_ADD = 0x112,//12
	//Stellgrößen der Antriebsregelung
	CTRL_DRV_CTRL_THRUST_ADD = 0x120,//13
	CTRL_DRV_CTRL_RUDDER_ANGLE_ADD = 0x122,//14
	//lokaler Error Block
	ESB_GPS_ADD = 0x200,//15
	ESB_COMPASS_ADD = 0x201,//16
	ESB_CTRL_ADD = 0x202,//17
	//Lidar
	LIDAR_VALUE_ADD =0x203//18
}slaveRegAdd_t;

#define USV_LAST_DATA_BLOCK_ADDR LIDAR_VALUE_ADD

typedef uint8_t (*dataRx_t)(uint8_t* data, uint16_t length, uint32_t timeout_us);
typedef uint8_t (*dataTx_t)(uint8_t* data, uint16_t length, uint32_t timeout_us);

enum bytesPosInUSVProtocolHeader{
	USV_START_BYTE_POS,
	USV_OBJ_ID_BYTE_POS,
	USV_REG_ADDR_AND_WR_LBYTE_POS,
	USV_REG_ADDR_AND_WR_HBYTE_POS,
	USV_FRAME_LEN_BYTE_POS,
	USV_DATA_BEGIN_POS
};

typedef struct {
	dataRx_t receiveFunc_p;
	dataTx_t transmitFunc_p;
	uint8_t initState:1;
	uint8_t crc8Polynom;
}usvMonitorHandler_t;

typedef struct{
	uint8_t init:1;
	uint8_t usartNo:2;
	uint8_t lock:1;//0: ready, 1: busy
	uint8_t multiregister:1;
	uint8_t res:1;//res: result: 0: no error, 1: fail
}usvMgr_t;//Mgr: Manager

//Struktur für Konfiguration der USART-Einheit
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

extern uint8_t initDev(usvMonitorHandler_t* dev_p, dataRx_t inputRXFunc_p, dataTx_t inputTxFunc_p, uint8_t inputCrc8);
extern uint8_t setData(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* input_p,uint16_t length);
extern uint8_t getData(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* output_p, uint16_t outputLen);
extern uint8_t getMultiregister(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* output_p, uint16_t outputLen);
extern uint8_t setMultiregister(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* input_p, uint16_t inputLen);

#endif /* USVDATAHANDLER_H_ */
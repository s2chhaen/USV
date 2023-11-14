/*
 * usvMonitorHandlerAPI.h
 *
 * Created: 7/7/2023 5:25:53 AM
 * Author: Thach
 * Version: 1.2
 * Revision: 1.0
 */ 


#ifndef USVMONITORHANDLER_H_
#define USVMONITORHANDLER_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "usVMonitorHandlerAPIConfig.h"

#define USV_PROTOCOL_W_REQ 8
#define USV_PROTOCOL_R_REQ 4
#define USV_PROTOCOL_SET_SLAVE_ADD_LOW(add) (uint8_t)(add&0xff)
#define USV_PROTOCOL_SET_SLAVE_ADD_HIGH(add,rw) (uint8_t)((add>>8)|(rw<<4))
//Parameter f�r Zwischenspeicherbuffer
#define MAX_SIZE_FRAME 400
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
	SEN_GESB_ADD = 0x00,
	SEN_LONGNITUDE_ADD = 0x01,
	SEN_LATITUDE_ADD = 0x05,
	SEN_SATFIX_ADD = 0x09,
	SEN_GPS_VEL_ADD = 0x0A,
	SEN_COURSE_ANGLE_ADD = 0x0C,
	SEN_TIMESTAMP_ADD = 0x0E,
	//Radar
	RADAR_1_ADD = 0x12,
	RADAR_2_ADD = 0x13,
	//F�hrungsgr��en der Antriebsregelung
	REF_DRV_CTRL_REF_A_ADD = 0x100,
	REF_DRV_CTRL_REF_B_ADD = 0x108,
	REF_DRV_CTRL_VEL_ADD = 0x110,
	REF_DRV_CTRL_EPS_ADD = 0x112,
	//Stellgr��en der Antriebsregelung
	CTRL_DRV_CTRL_THRUST_ADD = 0x120,
	CTRL_DRV_CTRL_RUDDER_ANGLE_ADD = 0x122,
	//lokaler Error Block
	ESB_GPS_ADD = 0x200,
	ESB_COMPASS_ADD = 0x201,
	ESB_CTRL_ADD = 0x202,
	//Lidar
	LIDAR_VALUE_ADD =0x203
}slaveRegAdd_t;


typedef uint8_t (*dataRx_t)(uint8_t* data, uint16_t length, uint32_t timeout_us);
typedef uint8_t (*dataTx_t)(uint8_t* data, uint16_t length, uint32_t timeout_us);


typedef struct {
	dataRx_t receiveFunc_p;
	dataTx_t transmitFunc_p;
	uint8_t initState:1;
	uint8_t crc8Polynom;
}usvMonitorHandler_t;

extern uint8_t initDev(usvMonitorHandler_t* dev_p, dataRx_t inputRXFunc_p, dataTx_t inputTxFunc_p, uint8_t inputCrc8);
extern uint8_t setData(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* input_p,uint16_t length);
extern uint8_t getData(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* output_p, uint16_t outputLen);
extern uint8_t getMultiregister(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* output_p, uint16_t outputLen);
extern uint8_t setMultiregister(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* input_p, uint16_t inputLen);

#endif /* USVMONITORHANDLER_H_ */
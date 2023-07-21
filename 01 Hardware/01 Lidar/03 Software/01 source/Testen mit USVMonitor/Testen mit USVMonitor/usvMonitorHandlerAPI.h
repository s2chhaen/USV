/*
 * usvMonitorHandlerAPI.h
 *
 * Created: 7/7/2023 5:25:53 AM
 * Author: Thach
 */ 


#ifndef USVMONITORHANDLER_H_
#define USVMONITORHANDLER_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define UUASL_W_REQ 8
#define UUASL_R_REQ 4
#define GET_SLAVE_ADD_LOW_PART(a) a&0x00ff
#define GET_SLAVE_ADD_HIGH_PART(a) a>>8
#define SET_SLAVE_ADD_LOW_PART(add) (uint8_t)(add&0xff)
#define SET_SLAVE_ADD_HIGH_PART(add,rw) (uint8_t)((add>>8)|(rw<<4))
#define MAX_SIZE_FRAME 400
#define CRC8_POLYNOM  0xD5
#define MAX_RECEIVE_TRY 3

//Fehlererkennt, DEBUGGING ERFORDERLICH
typedef struct{
	uint8_t start;
	uint8_t slaveAdd;
	union rwa{
		uint8_t value[2];
		struct{
			uint8_t slaveRegAddL:8;//low
			uint8_t slaveRegAddH:4;
			uint8_t rw:4;//high
		}value_bf;
	}rwaBytes;
	uint8_t length;
}uuaslProtocolHeader_t;

typedef struct{
	uint8_t checksum;
	uint8_t end;
}uuaslProtocolTail_t;

typedef struct 
{
	uuaslProtocolHeader_t header;
	uint8_t dataLen;
	uuaslProtocolTail_t tail;
}uuaslReadProtocol_t;

typedef enum {
	//Sensorblock
	SEN_GESB_ADD = 0x00,
	SEN_LONGNITUDE_ADD = 0x01,
	SEN_LATITUDE_ADD = 0x05,
	SEN_SATFIX_ADD = 0x09,
	SEN_GPS_VEL_ADD = 0x0A,
	SEN_COURSE_ANGLE_ADD = 0x0C,
	SEN_TIMESTAMP_ADD = 0x0E,
	//Führungsgrößen der Antriebsregelung
	REF_DRV_CTRL_REF_A_ADD = 0x100,
	REF_DRV_CTRL_REF_B_ADD = 0x108,
	REF_DRV_CTRL_VEL_ADD = 0x110,
	REF_DRV_CTRL_EPS_ADD = 0x112,
	//Stellgrößen der Antriebsregelung
	CTRL_DRV_CTRL_THRUST_ADD = 0x120,
	CTRL_DRV_CTRL_RUDDER_ANGLE_ADD = 0x122,
	//lokaler Error Block
	ESB_GPS_ADD = 0x200,
	ESB_COMPASS_ADD = 0x201,
	ESB_CTRL_ADD = 0x202,
	//Lidar
	LIDAR_SEN_ADD = 0x203
}slaveRegAdd_t;

typedef struct{
	slaveRegAdd_t add:12;
	uint16_t len:9;
}slaveReg_t;


typedef uint8_t (*dataRx_t)(uint8_t* data, uint8_t length);
typedef uint8_t (*dataTx_t)(uint8_t* data, uint16_t length);
typedef void (*wait_t)(uint32_t us);

typedef struct {
	dataRx_t receiveFunc_p;
	dataTx_t transmitFunc_p;
	wait_t waitFunc_p;
	uint8_t initState:1;
	uint8_t dynamicWait:1;
	uint8_t crc8Polynom;
}usvMonitorHandler_t;

//extern const slaveReg_t regSet[];


extern uint8_t initDev(usvMonitorHandler_t* dev_p, dataRx_t inputRXFunc_p, dataTx_t inputTxFunc_p, wait_t inputWaitFunc_p, uint8_t inputCrc8);
extern uint8_t setData(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* input_p,uint16_t length);
extern uint8_t getData(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* output_p, uint16_t outputLen);
extern uint8_t getMultiregister(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* output_p, uint16_t outputLen);
extern uint8_t setMultiregister(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* input_p, uint16_t inputLen);

#endif /* USVMONITORHANDLER_H_ */
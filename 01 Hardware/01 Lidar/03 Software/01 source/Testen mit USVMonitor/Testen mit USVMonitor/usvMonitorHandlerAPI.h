/*
 * usvMonitor.h
 *
 * Created: 7/7/2023 5:25:53 AM
 * Author: Thach
 */ 


#ifndef USERUNIT_H_
#define USERUNIT_H_

#include <stdbool.h>
#include <stdint.h>
#include "errorList.h"
#include "ATMegaXX09/USART/USART.h"
#include "Math/checksum.h"

#define UUASL_W_REQ 4
#define UUASL_R_REQ 8
#define GET_SLAVE_ADD_LOW_PART(a) a&0x00ff
#define GET_SLAVE_ADD_HIGH_PART(a) a>>8
#define MAX_SIZE_FRAME 256
#define NO_OF_RX_BUFFER 2
#define MAX_BYTE_SEND 31
#define CRC8_POLYNOM  0xD5

typedef struct{
	uint8_t start;
	uint8_t slaveRegAdd;
	union rwa{
		uint8_t value[2];
		struct{
			uint8_t slaveAddL:8;//low
			uint8_t slaveAddH:4;
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
}uuaslReadProtocol;

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
	ESB_CTRL_ADD = 0x202
}slaveRegAdd_t;

typedef struct{
	slaveRegAdd_t add:12;
	uint8_t len:4;
}slaveReg_t;

typedef bool (*callbackTx_t)(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length);
typedef bool (*callbackRx_t)(uint8_t adress, uint8_t data[], uint8_t length);
typedef uint8_t (*dataRx_t)(uint8_t* data, uint16_t* length);
typedef uint8_t (*dataTx_t)(uint8_t* data,uint16_t length);
typedef void (*wait_t)(uint32_t us);

typedef struct {
	dataRx_t receiveFunc_p;
	dataTx_t transmitFunc_p;
	wait_t waitFunc_p;
	struct rxUnit{
		volatile uint8_t rxBuffer[NO_OF_RX_BUFFER][MAX_SIZE_FRAME];
		volatile uint16_t rxBufferLen[NO_OF_RX_BUFFER];
		volatile uint16_t strReadPtr;
		const uint16_t rxLenMax;
		const uint8_t fifoLenMax;
		volatile uint8_t readFIFOPtr;
		volatile uint8_t writeFIFOPtr;
	}rxObj;
	struct txUnit{
		volatile uint8_t txBuffer[MAX_SIZE_FRAME];
		volatile uint16_t toTxByte;
		volatile uint16_t strReadPtr;
		const uint16_t txLenMax;
	}txObj;
	struct status{
		volatile uint8_t uart:2;
		uint8_t initState:1;
		uint8_t crcActive:1;
		volatile uint8_t rxBufferState:2;//Leer, Belegt, Voll
		uint8_t nextPhase:1;
	}statusObj;
}usvMonitorHandler_t;

typedef struct{
	callbackTx_t tx_p;
	callbackRx_t rx_p;
	uint8_t usartNo;
	uint32_t baudrate;
	USART_CHSIZE_t bits;
	USART_PMODE_t parity; 
	USART_SBMODE_t stopbit;
	bool sync; bool MPCM; 
	uint8_t address;
	PORTMUX_USARTx_t PortMux;
}usvMonitorConfig_t;

//memcpy verwendet, weil es schneller ist
uint8_t initDev(usvMonitorHandler_t* dev, usvMonitorConfig_t* config);
uint8_t setData(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev);
uint8_t getData(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev);
//static uint8_t dataTx(uint8_t* data, uint16_t length);
//static uint8_t dataRx(uint8_t* data, uint16_t length);

#endif /* USERUNIT_H_ */
/*
 * slaveDeviceAPI.h
 *
 * Created: 7/5/2023 10:08:44 PM
 *  Author: Thach
 */ 


#ifndef SLAVEDEVICEAPI_H_
#define SLAVEDEVICEAPI_H_

#include "slaveDeviceConfig.h"

typedef processResult_t (*txData_t)(uint8_t*, uint16_t);
typedef processResult_t (*rxData_t)(uint8_t*, uint16_t*);

typedef struct{
	txData_t dataTx_p;
	rxData_t dataRx_p;
	struct rxUnit{
		volatile uint8_t rxBuffer[NO_OF_RX_BUFFER][RX_BUFFER_LEN];
		volatile uint16_t toRxByte[NO_OF_RX_BUFFER];
		volatile uint16_t strReadPtr;
		const uint16_t rxLenMax;
		const uint8_t fifoLenMax;
		volatile uint8_t readFIFOPtr;
		volatile uint8_t writeFIFOPtr;
	}rxObj;
	struct txUnit{
		volatile uint8_t txBuffer[TX_BUFFER_LEN];
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
}slaveDevice_t;

#endif /* SLAVEDEVICEAPI_H_ */
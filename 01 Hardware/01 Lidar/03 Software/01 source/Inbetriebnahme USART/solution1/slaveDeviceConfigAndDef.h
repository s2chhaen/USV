/*
 * slaveDeviceConfig.h
 *
 * Created: 7/3/2023 10:04:33 PM
 *  Author: Thach
 */ 


#ifndef SLAVEDEVICECONFIG_H_
#define SLAVEDEVICECONFIG_H_

#define MAX_BYTE_SEND 31

#define RX_BUFFER_LEN 800
#define TX_BUFFER_LEN 100
#define NO_OF_RX_BUFFER 2
#define USART_TIME_PRO_BYTE_US 96

//TODO lässt hier bis zum Ende der Dokumentationsphase, dieses Modus ist sehr schwer und nicht
//geeignet für hohe Baudrate
#define ACTIVE_USART_WATCHER 1
#define END_SYM 0x0A

//Version 2 von slaveDevice_t
typedef struct{
	//RX-Buffer
	volatile uint8_t rx_Buffer[NO_OF_RX_BUFFER][RX_BUFFER_LEN];
	volatile uint16_t rxByte[NO_OF_RX_BUFFER];
	volatile uint16_t rx_strReadPtr;
	const uint16_t rx_LenMax;
	const uint8_t rx_fifoLenMax;
	volatile uint8_t rx_readFIFOPtr;
	volatile uint8_t rx_writeFIFOPtr;
	//TX-Buffer
	volatile uint8_t tx_Buffer[TX_BUFFER_LEN];
	volatile uint16_t toTxByte;
	volatile uint16_t tx_strReadPtr;
	const uint16_t tx_LenMax;
	//Zustand
	volatile uint8_t uart:2;
	uint8_t initState:1;
	uint8_t crcActive:1;
	volatile uint8_t rxFIFOState:2;//Leer, Belegt, Voll
	uint8_t nextPhase:1;
}slaveDevice_t;

typedef enum {
	ON,
	OFF
}state_t;

enum fifoState{
	EMPTY=0,
	FILLED,
	FULL
};

#endif /* SLAVEDEVICECONFIG_H_ */
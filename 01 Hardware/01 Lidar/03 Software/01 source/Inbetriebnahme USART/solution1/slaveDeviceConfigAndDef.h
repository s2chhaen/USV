/*
 * slaveDeviceConfigAndDef.h: Alle Konfigurationen und Definitionen für das slaveDevice-Modul
 *
 * Created: 7/3/2023 10:04:33 PM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.0
 */ 


#ifndef SLAVEDEVICECONFIGANDDEF_H_
#define SLAVEDEVICECONFIGANDDEF_H_

#define MAX_BYTE_SEND 31

#define RX_BUFFER_LEN 800
#define TX_BUFFER_LEN 100
#define NO_OF_RX_BUFFER 2
#define BAUDRATE_BAUD 250000UL
#define BIT_PER_SYM 11

#ifndef ACTIVE_USART_WATCHER
#define END_SYM 0x0A
#endif

//Struktur der Verwaltungseinheit
typedef struct{
	struct rxUnit{
		volatile uint8_t rxBuffer[NO_OF_RX_BUFFER][RX_BUFFER_LEN];
		volatile uint16_t rxByte[NO_OF_RX_BUFFER];
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
		volatile uint8_t rxFIFOState:2;//Leer, Belegt, Voll
	}statusObj;
}slaveDevice_t;

//Zustand der FIFO
enum fifoState{
	EMPTY=0,
	FILLED,
	FULL
};

#endif /* SLAVEDEVICECONFIGANDDEF_H_ */
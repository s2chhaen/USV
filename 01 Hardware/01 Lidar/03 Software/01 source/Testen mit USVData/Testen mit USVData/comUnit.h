/*
 * comUnit.h
 *
 * Created: 7/10/2023 4:48:20 AM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.2
 */ 


#ifndef USERUNIT_H_
#define USERUNIT_H_

#include <stdint.h>
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include "errorList.h"
#include "ATMegaXX09/USART/USART.h"
#include "ATMegaXX09/FIFO/FIFO.h"
#include "Math/MinMax.h"
#include "timerUnit.h"

#define RX_BUFFER_LEN 360
#define TX_BUFFER_LEN 360
#define BYTE_RECEIVE_TIME_US 100
#define BUFFER_LEN 360

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

//Struktur für User-Unit
typedef struct{
	struct rxUnit{
		volatile uint8_t rxBuffer[RX_BUFFER_LEN];
		volatile uint8_t toRxByte;//die schon empfangenen Bytes
		volatile uint16_t strPtr;//der Zeiger zum aktuell empfangenen Byte
		const uint16_t rxLenMax;//Max. Länge des Buffers
		const uint16_t usartFIFOMax;//Max. Anzahl der Buffers in einem FIFO, bisher nur 1
	}rxObj;
	struct txUnit{
		volatile uint8_t txBuffer[TX_BUFFER_LEN];
		volatile uint8_t toTxByte;//die noch zu sendenen Byte in Buffer
		volatile uint16_t strPtr;//der Zeiger zum aktuell zu sendenden Byte
		const uint16_t txLenMax;//Max. Länge des Buffers
		const uint16_t usartFIFOMax;//Max. Anzahl der Buffers in einem FIFO, bisher nur 1
	}txObj;
	struct status{
		volatile uint8_t usart:2;//Die angewendeten USART
		uint8_t initState:1;//Bezeichnet, ob dieses Objekt initialisiert oder nicht
		volatile uint8_t send:1;//Bezeichnet, ob die Sendungsvorgang noch im Prozess ist oder nicht
		volatile uint8_t receive:1;//Bezeichnet, ob die Empfangsvorgang noch im Prozess ist oder nicht
	}statusObj;
}userUnit_t;

extern uint8_t initUserUnit(usartConfig_t config);
extern uint8_t usartDataTx(uint8_t* data, uint16_t length);
extern uint8_t usartDataRx(uint8_t* data, uint16_t length);

#endif /* USERUNIT_H_ */
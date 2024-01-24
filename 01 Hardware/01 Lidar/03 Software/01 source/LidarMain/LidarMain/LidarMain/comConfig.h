/*
 * comConfig.h
 *
 * Created: 12/15/2023 10:00:07 AM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.0
 */ 


#ifndef COMCONFIG_H_
#define COMCONFIG_H_

#include <stdint.h>


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

#endif /* COMCONFIG_H_ */
/*
 * radarHandler.c
 *
 * Created: 12/12/2023 11:56:26 AM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.1
 */ 

#include "radarHandler.h"

//Lokalvariable für Radar
//USART-Modul-FIFO-Max-Len
static const uint16_t usartFIFOMaxLen = _FIFO_max_def - 1;
//Protokollbereich
static uint8_t radar_protocol[RADAR_PROTOCOL_MAX_LEN]  = {0};
//Daten-RX-Buffer
volatile radarRxDataFIFO_t radar_fifo = {
	.rPtr = 0, .wPtr = 0, .full = 0, .empty = 1
};

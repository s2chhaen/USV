////////////////////////////////////////////////////////////////////////////////////////////////////
//
// USART.h
//
// Created: 04.09.2020 16:33:50
//  Author: franke
//
//
//		-Implementierung als UART und USART
//
//		rev. 0.1
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _USART_
#define _USART_

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>     // Namen der IO Register
#include <avr/eeprom.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
// globale USART Typendefinitionen
////////////////////////////////////////////////////////////////////////////////////////////////////
enum eUSARTnumber
{
	iUSART0,
	iUSART1,
	iUSART2,
	iUSART3,
};

typedef enum PORTMUX_USARTx_enum
{
	PORTMUX_USARTx_DEFAULT_gc = (0x00<<0),  // USARTx on normal Pins
	PORTMUX_USARTx_ALT1_gc = (0x01<<0),  // USARTx on alternate Pins
	PORTMUX_USARTx_NONE_gc = (0x03<<0),  // Not connected to any pins
}
PORTMUX_USARTx_t;

typedef bool (*USART_receive_Array_callback_fnc_t)(uint8_t adress, uint8_t data[], uint8_t length);
typedef bool (*USART_send_Array_callback_fnc_t)(uint8_t* adress, uint8_t* data[], uint8_t* length, \
	uint8_t max_length);
////////////////////////////////////////////////////////////////////////////////////////////////////
// externe USART Variablen
////////////////////////////////////////////////////////////////////////////////////////////////////
extern EEMEM const uint8_t USARTPort[4];

////////////////////////////////////////////////////////////////////////////////////////////////////
// öffentliche Funktiondeklaration
////////////////////////////////////////////////////////////////////////////////////////////////////
bool USART_init(uint8_t USARTnumber, uint32_t baudrate, USART_CHSIZE_t bits, USART_PMODE_t parity, \
USART_SBMODE_t stopbit, bool synchron, bool MPCM, uint8_t address, PORTMUX_USARTx_t PortMux);
//uint8_t SIRCOMx_Init(uint8_t number, uint32_t baudrate,...);
//uint8_t SPIMASTERx_Init(uint8_t number, uint32_t baudrate,...);

void USART_DeInit(uint8_t USARTnumber);
//MOD added here
void USART_flushRXFIFO(uint8_t USARTnumber);

bool USART_send_Array(uint8_t USARTnumber ,uint8_t adress, uint8_t data[], uint8_t length);
void USART_set_Bytes_to_receive(uint8_t USARTnumber, uint8_t Bytes_to_receive);
bool USART_receive_Array(uint8_t USARTnumber, uint8_t* adress, uint8_t* data[], uint8_t max_length, uint8_t* length);
uint8_t USART_get_RXeventID(uint8_t USARTnumber);
uint8_t USART_get_TXeventID(uint8_t USARTnumber);

void USART_set_receive_Array_callback_fnc(uint8_t USARTnumber, USART_receive_Array_callback_fnc_t USART_receive_Array_callback_in);
void USART_set_send_Array_callback_fnc(uint8_t USARTnumber, USART_send_Array_callback_fnc_t USART_send_Array_callback_in);

#endif // _USART_ //

// EOF //

/*
 * usart1Beta.h
 *
 * Created: 6/27/2023 2:28:38 PM
 *  Author: hathach96
 */ 


#ifndef USART1BETA_H_
#define USART1BETA_H_

#include <avr/io.h>
#include "../ATMegaXX09/FIFO/FIFO.h"
#include <stddef.h>
#include "../error_list.h"
#include <stdbool.h>
#include <avr/interrupt.h>

typedef enum{
	STATUS_en,
	CTRLA_en,
	CTRLB_en,
	CTRLC_en,
	BAUD_en,
	CTRLD_en
}registerUSART_t;

typedef union
{
	uint8_t value;
	struct{
		uint8_t RS485:2;//Bit 0,1, low bit
		uint8_t ABEIE:1;
		uint8_t LBME:1;
		uint8_t RXSIE:1;
		uint8_t DREIE:1;
		uint8_t TXCIE:1;
		uint8_t RXCIE:1;//Bit 7, high bit
	}Register;
}CTRLA_t;

typedef union
{
	uint8_t value;
	struct{
		uint8_t MPCM:1;//low bit
		uint8_t RXMODE:2;
		uint8_t ODME:1;
		uint8_t SFDEN:1;
		uint8_t :1;
		uint8_t TXEN:1;
		uint8_t RXEN:1;//high bit
	}Register;
}CTRLB_t;

typedef union
{
	uint8_t value;
	struct
	{	
		uint8_t CHSIZE:3;//low Bit
		uint8_t SBMODE:1;
		uint8_t PMODE:2;
		uint8_t CMODE:2;//high bit
	}Reg_AsyncMode;
	struct
	{	// Low Bit
		uint8_t :1;
		uint8_t UCPHA:1;
		uint8_t UDORD:1;
		uint8_t :3;
		uint8_t CMODE:2;
	}Reg_MasterSPI;
}CTRLC_t;

typedef union
{
	uint8_t value;
	struct
	{	
		uint8_t :6;// Low Bit
		uint8_t ABW:2;
	}Register;
}CTRLD_t;

typedef union
{
	register8_t value;
	struct
	{	// Low Bit
		uint8_t WFB:1;
		uint8_t BDF:1;
		uint8_t :1;
		uint8_t ISFIF:1;
		uint8_t RXSIF:1;
		uint8_t DREIF:1;
		uint8_t TXCIF:1;
		uint8_t RXCIF:1;
	}Register;
}STATUS_t;

typedef union{
	uint8_t value;
	struct{
		uint8_t bit0:1;
		uint8_t bit1:1;
		uint8_t bit2:1;
		uint8_t bit3:1;
		uint8_t bit4:1;
		uint8_t bit5:1;
		uint8_t bit6:1;
		uint8_t bit7:1;
	}bitField;
}register8Bit_t;

typedef union{
	uint16_t value;
	struct{
		uint8_t bit0:1;
		uint8_t bit1:1;
		uint8_t bit2:1;
		uint8_t bit3:1;
		uint8_t bit4:1;
		uint8_t bit5:1;
		uint8_t bit6:1;
		uint8_t bit7:1;
		uint8_t bit8:1;
		uint8_t bit9:1;
		uint8_t bit10:1;
		uint8_t bit11:1;
		uint8_t bit12:1;
		uint8_t bit13:1;
		uint8_t bit14:1;
		uint8_t bit15:1;
	}bitField;
}register16Bit_t;

typedef struct
{
	volatile sFIFO_t txFIFO;
	volatile USART_t *adr_p;
}usartModuleHAL_t;

extern void initUsart1();


#endif /* USART1BETA_H_ */
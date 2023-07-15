/////////////////////////////////////////////////////////////////////////////////////////
//
// FIFO.h
//
// Created: 10.04.2015 11:16:03
//  Author: franke
//
/////////////////////////////////////////////////////////////////////////////////////////


#ifndef FIFO_H_
#define FIFO_H_

#include <stdint.h>
#include <stdbool.h>

#define _FIFO_pot 5
#define _FIFO_max_def (1<<_FIFO_pot)

#define __const8_init__ *(uint8_t *)&

typedef struct sFIFO_td
{
	volatile uint8_t p_read :_FIFO_pot;
	volatile uint8_t p_write :_FIFO_pot;
	uint8_t p_bit_read :4;
	const uint8_t FIFO_max;
	union uFIFO_t
	{
		struct data_bits_t
		{
			uint8_t bit0 :1;
			uint8_t bit1 :1;
			uint8_t bit2 :1;
			uint8_t bit3 :1;
			uint8_t bit4 :1;
			uint8_t bit5 :1;
			uint8_t bit6 :1;
			uint8_t bit7 :1;
		}data_bits[_FIFO_max_def];
		uint8_t data[_FIFO_max_def];
	}dFIFO;
}sFIFO_t;

#define preinit_FIFO {.p_read = 0, .p_write = 0, .p_bit_read = 0, .FIFO_max = _FIFO_max_def, .dFIFO = {{{0}}} }
#define preinit_FIFO_arr(n) {[0 ... n-1].p_read = 0, [0 ... n-1].p_write = 0, [0 ... n-1].p_bit_read = 0, [0 ... n-1].FIFO_max = _FIFO_max_def, [0 ... n-1].dFIFO = {{{0}}} }

void init_FIFO(sFIFO_t* FIFO) __attribute__((deprecated));

static inline bool FIFO_chk_for_readdata(sFIFO_t* FIFO)
{
	return (FIFO->p_read != FIFO->p_write);
}

static inline bool FIFO_is_empty(sFIFO_t* FIFO)
{
	return (FIFO->p_read != FIFO->p_write);
}

static inline bool FIFO_chk_for_writedata(sFIFO_t* FIFO)
{
	return ( ((FIFO->p_read - FIFO->p_write) + _FIFO_max_def) > 1);
}

static inline uint8_t FIFO_diff_readdata(sFIFO_t* FIFO)
{
	return (((FIFO->p_write - FIFO->p_read) + _FIFO_max_def)%_FIFO_max_def);
}

static inline uint8_t FIFO_diff_writedata(sFIFO_t* FIFO)
{
	return (((FIFO->p_read - FIFO->p_write) + (_FIFO_max_def-1))%_FIFO_max_def);
}

static inline void FIFO_flush(sFIFO_t* FIFO)
{
	FIFO->p_write = 0x00;
	FIFO->p_read = 0x00;
}

bool FIFO_writebyte(sFIFO_t* FIFO, uint8_t byte);
bool FIFO_readbyte(sFIFO_t* FIFO, uint8_t* byte);

void FIFO_copy_to_lin(uint8_t* lin_space, sFIFO_t* FIFO, uint8_t lenght);
void FIFO_copy_from_lin(sFIFO_t* FIFO, uint8_t* lin_space, uint8_t lenght);

#endif /* FIFO_H_ */

// EOF //

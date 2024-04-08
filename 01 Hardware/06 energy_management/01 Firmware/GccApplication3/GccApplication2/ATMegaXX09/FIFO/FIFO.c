/////////////////////////////////////////////////////////////////////////////////////////
//
// FIFO.c
//
// Created: 10.04.2015 11:16:22
//  Author: Franke
//
//
// rev 1.1 runtime optimisation; 2023.07.20
//
/////////////////////////////////////////////////////////////////////////////////////////

#include "FIFO.h"
#include <string.h>
#include <util/atomic.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#pragma GCC push_options
#pragma GCC optimize ("O3")
void init_FIFO(sFIFO_t* FIFO)
{
	if(FIFO != NULL)
	{
		FIFO->p_write = 0;
		FIFO->p_read = 0;
		//FIFO->p_bit_read = 0;
		__const8_init__ FIFO->FIFO_max = _FIFO_max_def;
	}
}

// bool FIFO_writebyte(sFIFO_t* FIFO, uint8_t byte)
// {
// 	if(FIFO_chk_for_writedata(FIFO))
// 	{
// 		FIFO->dFIFO.data[FIFO->p_write] = byte;
// 		FIFO->p_write = _modFIFO_max(FIFO->p_write+1);
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}
// }

// bool FIFO_readbyte(sFIFO_t* FIFO, uint8_t* byte)
// {
// 	if(FIFO_chk_for_readdata(FIFO))
// 	{
// 		*byte = FIFO->dFIFO.data[FIFO->p_read];
// 		FIFO->p_read = _modFIFO_max(FIFO->p_read+1);
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}
// }

uint8_t FIFO_copy_to_linC(uint8_t* lin_space, sFIFO_t* FIFO, uint8_t lenght)
{
	uint8_t* p_read_start;
	uint8_t copy_lenght, max_read_lenght;
	register uint8_t read_length_block;
	
	// lese aus dem Ringpuffer den Lesezeiger aus
	max_read_lenght = FIFO_diff_readdata(FIFO);
	copy_lenght = MIN(lenght, max_read_lenght);
	// Berechne die Länge für die zwei Kopieranweisungen
	read_length_block = MIN((copy_lenght),(FIFO->FIFO_max - FIFO->p_read));
	if (read_length_block != 0)
	{
		p_read_start = &(FIFO->dFIFO.data[FIFO->p_read]);
		// lese aus dem Ringpuffer
		memcpy(lin_space, p_read_start, read_length_block);
		// Berechne die Länge für die zwei Kopieranweisungen
		p_read_start = lin_space + read_length_block;
		read_length_block = copy_lenght - read_length_block;
		if(read_length_block != 0)
			memcpy(p_read_start, FIFO->dFIFO.data, read_length_block);
		// Lesezeiger anpassen
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			FIFO->p_read = _modFIFO_max(FIFO->p_read + copy_lenght);
		}
	}
	return copy_lenght;
}

uint8_t FIFO_copy_from_lin(sFIFO_t* FIFO, uint8_t* lin_space, uint8_t lenght)
{
	uint8_t* p_write_start;
	uint8_t copy_lenght, FIFO_length;
	register uint8_t write_lenght_block;
	
	// berechne die maximale Schreiblänge
	FIFO_length = FIFO_diff_writedata(FIFO);
	copy_lenght = MIN(lenght, FIFO_length);
	// Berechne die Länge für die erste Kopieranweisungen
	write_lenght_block = MIN((copy_lenght),(FIFO->FIFO_max - FIFO->p_write));
	// schreibe in den Ringpuffer
	if(write_lenght_block != 0)
	{
		// lese aus dem Ringpuffer den Schreibzeiger aus
		p_write_start = &(FIFO->dFIFO.data[FIFO->p_write]);
		memcpy(p_write_start, lin_space, write_lenght_block);
		// Berechne die Länge für die zweite Kopieranweisungen
		p_write_start = lin_space + write_lenght_block;
		write_lenght_block = copy_lenght - write_lenght_block;
		if(write_lenght_block != 0)
			memcpy(FIFO->dFIFO.data, p_write_start, write_lenght_block);
		// Schreibzeiger anpassen
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			FIFO->p_write = _modFIFO_max(FIFO->p_write+copy_lenght);
		}
	}
	return copy_lenght;
}
#pragma GCC pop_options

// EOF

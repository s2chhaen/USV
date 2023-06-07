/////////////////////////////////////////////////////////////////////////////////////////
//
// FIFO.c
//
// Created: 10.04.2015 11:16:22
//  Author: franke
//
/////////////////////////////////////////////////////////////////////////////////////////

#include "FIFO.h"
#include <string.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


void init_FIFO(sFIFO_t* FIFO)
{
	if(FIFO != NULL)
	{
		FIFO->p_write = 0;
		FIFO->p_read = 0;
		FIFO->p_bit_read = 0;
		__const8_init__ FIFO->FIFO_max = _FIFO_max_def;
	}
}

bool FIFO_writebyte(sFIFO_t* FIFO, uint8_t byte)
{
	if(FIFO_chk_for_writedata(FIFO))
	{
		FIFO->dFIFO.data[FIFO->p_write++] = byte;
		return true;
	}
	else
	{
		return false;
	}
}

bool FIFO_readbyte(sFIFO_t* FIFO, uint8_t* byte)
{
	if(FIFO_chk_for_readdata(FIFO))
	{
		*byte = FIFO->dFIFO.data[FIFO->p_read++];
		return true;
	}
	else
	{
		return false;
	}
}

void FIFO_copy_to_lin(uint8_t* lin_space, sFIFO_t* FIFO, uint8_t lenght)
{
	// lese aus dem Ringpuffer den Lesezeiger aus
	uint8_t* p_read_start = &(FIFO->dFIFO.data[FIFO->p_read]);
	uint8_t max_read_lenght = FIFO_diff_readdata(FIFO);
	uint8_t copy_lenght = MIN(lenght, max_read_lenght);
	// Berechne die Länge für die zwei Kopieranweisungen
	uint8_t read_lenght_block1 = MIN((copy_lenght),(FIFO->FIFO_max - FIFO->p_read));
	uint8_t read_lenght_block2 = copy_lenght - read_lenght_block1;
	// lese aus dem Ringpuffer
	memcpy(lin_space, p_read_start, read_lenght_block1);
	memcpy(lin_space+read_lenght_block1, FIFO->dFIFO.data, read_lenght_block2);
	// Lesezeiger anpassen
	FIFO->p_read += copy_lenght;
}

void FIFO_copy_from_lin(sFIFO_t* FIFO, uint8_t* lin_space, uint8_t lenght)
{
	// berechne die maximale Schreiblänge
	uint8_t i;
	uint8_t max_write_lenght = FIFO_diff_writedata(FIFO);
	for(i = 0; i<MIN(lenght, max_write_lenght); i++)
	{
		FIFO->dFIFO.data[FIFO->p_write++] = lin_space[i];
	}
}

// void FIFO_copy_from_lin(sFIFO_t* FIFO, uint8_t* lin_space, uint8_t lenght)
// {
// 	// lese aus dem Ringpuffer den Schreibzeiger aus
// 	uint8_t* p_write_start = &(FIFO->dFIFO.data[FIFO->p_write]);
// 	// berechne die maximale Schreiblänge
// 	uint8_t max_write_lenght = FIFO_diff_writedata(FIFO);
// 	uint8_t copy_lenght = MIN(lenght, max_write_lenght);
// 	// Berechne die Länge für die zwei Kopieranweisungen
// 	uint8_t write_lenght_block1 = MIN((copy_lenght),(FIFO->FIFO_max - FIFO->p_write));
// 	uint8_t write_lenght_block2 = copy_lenght - write_lenght_block1;
// 	// schreibe in dee Ringpuffer
// 	memcpy(p_write_start, lin_space, write_lenght_block1);
// 	memcpy(FIFO->dFIFO.data, lin_space + write_lenght_block1, write_lenght_block2);
// 	// Schreibzeiger anpassen
// 	FIFO->p_write += copy_lenght;
// }

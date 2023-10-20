/////////////////////////////////////////////////////////////////////////////////////////
//
// FIFO.h
//
// Created: 10.04.2015 11:16:03
//  Author: Franke
//
//
// rev 1.1 runtime optimisation; 2023.07.20
//
///////////////////////////////////////////////////////////////////////////////////////// 


#ifndef FIFO_H_
#define FIFO_H_

/////////////////////////////////////////////////////////////////////////////////////////
// configuration
/////////////////////////////////////////////////////////////////////////////////////////
#define _FIFO_pot 5

/////////////////////////////////////////////////////////////////////////////////////////
// runtime parameter
/////////////////////////////////////////////////////////////////////////////////////////
#define _FIFO_max_def (1<<_FIFO_pot)
#define _FIFO_bits (_FIFO_max_def-1)

#define _p_read     0
#define _p_write    1
#define _p_FIFO_max 2
#define _p_data     3

/////////////////////////////////////////////////////////////////////////////////////////
// c definitions
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef __ASSEMBLER__

	#include <stdint.h>
	#include <stdbool.h>
	
	#define _modFIFO_max(_val) ((_val)&(_FIFO_bits))
	#define __const8_init__ *(uint8_t *)&
	/////////////////////////////////////////////////////////////////////////////////////////
	//	FIFO structure
	/////////////////////////////////////////////////////////////////////////////////////////
	typedef struct sFIFO_td
	{
		volatile uint8_t p_read;
		volatile uint8_t p_write;
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

	#define preinit_FIFO(val) {.p_read = 0, .p_write = 0, /*.p_bit_read = 0,*/ .FIFO_max = _FIFO_max_def, .dFIFO = {{{0}}} }
	#define preinit_FIFO_arr(n) {[0 ... n-1].p_read = 0, [0 ... n-1].p_write = 0, [0 ... n-1].FIFO_max = _FIFO_max_def, [0 ... n-1].dFIFO = {{{0}}} }
	
	
	/////////////////////////////////////////////////////////////////////////////////////////
	// void init_FIFO(sFIFO_t* FIFO) __attribute__((deprecated));
	//  use preinit_FIFO() macro
	/////////////////////////////////////////////////////////////////////////////////////////
	void init_FIFO(sFIFO_t* FIFO) __attribute__((deprecated));
	
	/////////////////////////////////////////////////////////////////////////////////////////
	// bool FIFO_chk_for_readdata(sFIFO_t* FIFO)
	//  check is FIFO empty
	/////////////////////////////////////////////////////////////////////////////////////////
	static inline bool FIFO_is_empty(sFIFO_t* FIFO)
	{
		//return (FIFO->p_read == FIFO->p_write);
		bool ret;
		__asm__ __volatile__(
			"LDD  r24, z+%2\n\t"
			"LDD  r25, z+%3\n\t"
			"CP r25, r24   \n\t"
			"LDI %w0, 1    \n\t"
			"BREQ .+2      \n\t"
			"LDI %w0, 0    \n\t"
			:"=w" (ret)
			:"z" (FIFO), "i" (_p_read), "i" (_p_write)
			: "r25"
		);
		return ret;
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////
	// bool FIFO_chk_for_readdata(sFIFO_t* FIFO)
	//  check for readable data
	/////////////////////////////////////////////////////////////////////////////////////////
	static inline __attribute__((always_inline)) bool FIFO_chk_for_readdata(sFIFO_t* FIFO)
	{
		//return (FIFO->p_read != FIFO->p_write);
		bool ret;
		__asm__ __volatile__(
			"LDD  r24, z+%2\n\t"
			"LDD  r25, z+%3\n\t"
			"CP r25, r24   \n\t"
			"LDI %w0, 0    \n\t"
			"BREQ .+2      \n\t"
			"LDI %w0, 1    \n\t"
			:"=w" (ret)
			:"z" (FIFO), "i" (_p_read), "i" (_p_write)
			: "r25"
		);
		return ret;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	// bool FIFO_chk_for_writedata(sFIFO_t* FIFO)
	//  check for writeable data
	/////////////////////////////////////////////////////////////////////////////////////////
	static inline __attribute__((always_inline)) bool FIFO_chk_for_writedata(sFIFO_t* FIFO) 
	{
		// return ( (FIFO->p_read != _modFIFO_max((uint8_t)FIFO->p_write+(uint8_t)0x01)) );
		bool ret;
		__asm__ __volatile__(
			"LDD  r24, z+%2\n\t"
			"LDD  r25, z+%3\n\t"
			"INC  r25      \n\t"
			"ANDI r25, %4  \n\t"
			"CP r25, r24   \n\t"
			"LDI %w0, 0    \n\t"
			"BREQ .+2      \n\t"
			"LDI %w0, 1    \n\t"
			:"=w" (ret)
			:"z" (FIFO), "i" (_p_read), "i" (_p_write), "i" ((uint8_t)(_FIFO_bits))
			: "r25"
		);
		return ret;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	// uint8_t FIFO_diff_readdata(sFIFO_t* FIFO)
	//  calculate the number of readable data
	/////////////////////////////////////////////////////////////////////////////////////////
	static inline __attribute__((always_inline)) uint8_t FIFO_diff_readdata(sFIFO_t* FIFO)
	{
		//return _modFIFO_max((FIFO->p_write + _FIFO_max_def ) - FIFO->p_read );
		uint8_t ret;
		__asm__ __volatile__(
			"LDD  %w0, z+%3\n\t"
			"SUBI %w0, %5  \n\t"
			"LDD  r25, z+%2\n\t"
			"SUB  %w0, r25 \n\t"
			"ANDI %w0, %4  \n\t"
			:"=w" (ret)
			:"z" (FIFO), "i" (_p_read), "i" (_p_write), "i" ((uint8_t)(_FIFO_bits)), "i" ((uint8_t)(~(_FIFO_bits)))
			: "r25"
		);
		return ret;
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////
	// uint8_t FIFO_diff_writedata(sFIFO_t* FIFO)
	//  calculate the number of writeable data
	/////////////////////////////////////////////////////////////////////////////////////////
	static inline __attribute__((always_inline)) uint8_t FIFO_diff_writedata(sFIFO_t* FIFO)
	{
		//return _modFIFO_max((FIFO->p_read + (_FIFO_max_def-1))- FIFO->p_write );
		uint8_t ret;
		__asm__ __volatile__(
			"LDD  %w0, z+%2\n\t"
			"SUBI %w0, %5  \n\t"
			"LDD  r25, z+%3\n\t"
			"SUB  %w0, r25 \n\t"
			"ANDI %w0, %4  \n\t"
			:"=w" (ret)
			:"z" (FIFO), "i" (_p_read), "i" (_p_write), "i" ((uint8_t)(_FIFO_bits)), "i" ((uint8_t)(~(_FIFO_bits))+1)
			: "r25"
		);
		return ret;
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////
	// void FIFO_flush(sFIFO_t* FIFO)
	//  delete data in FIFO
	/////////////////////////////////////////////////////////////////////////////////////////
	static inline void FIFO_flush(sFIFO_t* FIFO)
	{
		__asm__ __volatile__(
			"STD  z+%1,r1  \n\t"
			"STD  z+%2,r1  \n\t"
			:
			:"z" (FIFO), "i" (_p_read), "i" (_p_write)
			: "r25"
		);
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////
	// bool FIFO_writebyte(sFIFO_t* FIFO, uint8_t byte)
	//  write new byte to FIFO
	/////////////////////////////////////////////////////////////////////////////////////////
	bool FIFO_writebyte(sFIFO_t* FIFO, uint8_t byte) __attribute__((used));
	
	/////////////////////////////////////////////////////////////////////////////////////////
	// bool FIFO_readbyte(sFIFO_t* FIFO, uint8_t* byte)
	//  read last byte from FIFO
	/////////////////////////////////////////////////////////////////////////////////////////
	bool FIFO_readbyte(sFIFO_t* FIFO, uint8_t* byte) __attribute__((used));
	
	/////////////////////////////////////////////////////////////////////////////////////////
	// uint8_t FIFO_copy_to_lin(uint8_t* lin_space, sFIFO_t* FIFO, uint8_t lenght)
	//  get array from FIFO
	//  return value is number of bytes copied to 'lin_space'
	/////////////////////////////////////////////////////////////////////////////////////////
	uint8_t FIFO_copy_to_lin(uint8_t* lin_space, sFIFO_t* FIFO, uint8_t lenght) __attribute__((used));
	
	/////////////////////////////////////////////////////////////////////////////////////////
	// uint8_t FIFO_copy_from_lin(sFIFO_t* FIFO, uint8_t* lin_space, uint8_t lenght)
	//  write array to FIFO
	//  return value is number of bytes copied to FIFO
	/////////////////////////////////////////////////////////////////////////////////////////
	uint8_t FIFO_copy_from_lin(sFIFO_t* FIFO, uint8_t* lin_space, uint8_t lenght) __attribute__((used));
	
#endif
#endif /* FIFO_H_ */

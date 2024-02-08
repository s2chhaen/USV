/*
 * registerModel.h
 * Description: Struktur eines 8-Bits-langen Registers
 * Created: 12/15/2023 2:16:11 AM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.2
 */ 


#ifndef REGISTERMODEL_H_
#define REGISTERMODEL_H_

#include <stdint.h>

typedef union{
	uint8_t val;
	struct{
		uint8_t bit0:1;
		uint8_t bit1:1;
		uint8_t bit2:1;
		uint8_t bit3:1;
		uint8_t bit4:1;
		uint8_t bit5:1;
		uint8_t bit6:1;
		uint8_t bit7:1;
	}valBf;
}reg8Model_t;


#endif /* REGISTERMODEL_H_ */
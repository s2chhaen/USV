/*
 * checksum.c
 *
 * Created: 7/7/2023 9:13:10 AM
 * Author: Thach
 * Version: 1.1
 */ 

#include "checksum.h"

static uint8_t crc8Polynom = 0;
volatile uint8_t crc8LookupTable[ASCII_MAX_LEN] = {0};
volatile uint16_t crc16Polynom = 0;
volatile uint16_t crc16LookupTable[ASCII_MAX_LEN];

void crc8Init(uint8_t polynom){
	uint8_t temp;
	crc8Polynom = polynom;
	for (volatile int i = ASCII_MAX_LEN; i; i--){
		temp = ASCII_MAX_LEN - i;
		for (volatile uint8_t j = 8; j; j--){
			if (temp & 0x80){
				temp = (temp << 1) ^ crc8Polynom;
			} else{
				temp = temp << 1;
			}
			
		}
		crc8LookupTable[ASCII_MAX_LEN-i] = temp;
	}
}

uint8_t crc8CodeGen(uint8_t *data, uint16_t len){
	uint8_t retVal = 0;
	uint8_t temp = 0;
	if ((data != NULL) && len){
		for (volatile uint16_t i = 0; i < len; i++){
			temp = data[i] ^ retVal;
			retVal = crc8LookupTable[temp];
		}
	}
	return retVal;
}

uint16_t reserver(uint16_t value){
	value = ((value & 0xAAAA) >> 1) | ((value & 0x5555) << 1);
	value = ((value & 0xCCCC) >> 2) | ((value & 0x3333) << 2);
	value = ((value & 0xF0F0) >> 4) | ((value & 0x0F0F) << 4);
	value = (value >> 8) | (value << 8);
	return value;
}


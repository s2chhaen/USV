/*
 * checksum.c
 *
 * Created: 7/7/2023 9:13:10 AM
 *  Author: Thach
 */ 

#include "checksum.h"

static uint8_t crc8Polynom = 0;
volatile uint8_t crc8LookupTable[ASCII_MAX_LEN] = {0};

void crc8Init(uint8_t polynom){
	uint8_t temp;
	crc8Polynom = polynom;
	for (volatile int i = ASCII_MAX_LEN; i; i--){
		temp = ASCII_MAX_LEN - i;
		for (volatile uint8_t j = 8; j; j--){
			if (temp&0x80){
				temp = (temp<<1) ^ crc8Polynom;
			} else{
				temp = temp << 1;
			}
			
		}
		crc8LookupTable[ASCII_MAX_LEN-i] = temp;
	}
}

static inline uint8_t crc8OneByte(uint8_t input){
	return crc8LookupTable[input];
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

uint16_t crc16(uint8_t* input, uint16_t length, uint16_t polynom){
	uint16_t uCrc16=0;
	uint8_t temp[]={0,0};
	for (uint32_t i=0;i<length;i++){
		temp[1]=temp[0];
		temp[0]=input[i];
		if (uCrc16&0x8000){
			uCrc16 = (uCrc16&0x7fff)<<1;
			uCrc16 ^= polynom;
			} else{
			uCrc16<<=1;
		}
		uCrc16 ^= (temp[0]|(temp[1]<<8));
	}
	return uCrc16;
}
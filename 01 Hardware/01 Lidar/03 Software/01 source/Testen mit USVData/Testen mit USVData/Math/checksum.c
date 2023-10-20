/*
 * checksum.c
 *
 * Created: 7/7/2023 9:13:10 AM
 *  Author: Thach
 */ 

#include "checksum.h"

uint8_t crc8(uint8_t *data, uint16_t len, uint8_t polynom){
	uint8_t crc = 0;
	uint8_t mix;
	uint8_t inbyte;
	while (len--){
		inbyte = *data++;
		for (uint8_t i = 8; i; i--){
			mix = ( crc ^ inbyte ) & 0x80;
			crc <<= 1;
			if (mix){
				crc ^= polynom;
			}
			inbyte <<= 1;
		}
	}
	return crc;
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
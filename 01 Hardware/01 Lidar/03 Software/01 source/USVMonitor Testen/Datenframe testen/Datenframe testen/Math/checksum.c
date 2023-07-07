/*
 * checksum.c
 *
 * Created: 7/7/2023 9:13:10 AM
 *  Author: Thach
 */ 

#include "checksum.h"

uint8_t crc8(uint8_t *data, uint8_t len, uint8_t polynom){
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
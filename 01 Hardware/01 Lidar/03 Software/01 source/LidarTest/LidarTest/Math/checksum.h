/*
 * checksum.h
 *
 * Created: 7/7/2023 9:12:38 AM
 * Author: Thach
 * Version: 1.1
 */ 


#ifndef CHECKSUM_H_
#define CHECKSUM_H_

#include <stdint.h>
#include <stddef.h>

#define ASCII_MAX_LEN_BIT 8
#define ASCII_MAX_LEN (1<<ASCII_MAX_LEN_BIT)

extern void crc8Init(uint8_t polynom);
extern uint8_t crc8CodeGen(uint8_t* data, uint16_t len);

#endif /* CHECKSUM_H_ */
#ifndef CRCCALCULATOR_H_INCLUDED
#define CRCCALCULATOR_H_INCLUDED

#include <stdint.h>

extern uint8_t crc8(uint8_t *data, uint16_t len, uint8_t polynom);
extern uint16_t crc16(uint8_t* input, uint16_t length, uint16_t polynom);

#endif // CRCCALCULATOR_H_INCLUDED

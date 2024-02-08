/**
 * crcCalculator.h
 * Description: Header-Datei für Checksum-Berechnung
 * warning: crc16 wird spezifisch für Lidar LMS2xx implementiert
 * Author: Thach
 * Created: 10/08/2023
 * Version: 1.0
 * Revision: 1.0
 */

#ifndef CRCCALCULATOR_H_INCLUDED
#define CRCCALCULATOR_H_INCLUDED

#include <stdint.h>

extern uint8_t crc8(uint8_t *data, uint16_t len, uint8_t polynom);
extern uint16_t crc16(uint8_t* input, uint16_t length, uint16_t polynom);

#endif // CRCCALCULATOR_H_INCLUDED

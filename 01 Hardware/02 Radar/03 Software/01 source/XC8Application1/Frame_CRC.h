/*
 * Frame_CRC.h
 *
 * Created: 12.03.2023 19:57:06
 *  Author: Admin
 */ 


#ifndef FRAME_CRC_
#define FRAME_CRC_

uint8_t crc8_Berechnung(uint8_t *byte, uint8_t len);
void generate_frame(uint8_t adress, uint8_t *data, uint8_t len, uint8_t *frame);


#endif /* FRAME_CRC_ */
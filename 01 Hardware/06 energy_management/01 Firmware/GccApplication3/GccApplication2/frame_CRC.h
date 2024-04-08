/*
 * IncFile1.h
 *
 * Created: 2023/12/12 16:05:53
 *  Author: 芋泥啵啵奶茶
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_
uint8_t crc8_Berechnung(uint8_t *byte, uint8_t len);
void generate_frame(uint8_t adress, uint8_t *data, uint8_t len, uint8_t *frame);





#endif /* INCFILE1_H_ */
/*
 * conversion.h
 *
 * Created: 12.03.2023 01:22:23
 *  Author: Admin
 */ 


#ifndef CONVERSION_H_
#define CONVERSION_H_

void speed_conversion (char *data, uint8_t *arr);
uint8_t range_conversion (char *data);
void string_from_Frame(uint8_t *data, uint8_t length, char *str_speed, char *str_range);


#endif /* CONVERSION_H_ */
#ifndef DATAPREP_H_
#define DATAPREP_H_

#include "error_list.h"

extern processResult_t bytes_to_values(uint8_t input[], uint16_t inLength, uint8_t output[], uint16_t outLength);
extern processResult_t round_values(uint8_t data[],uint16_t length);
extern processResult_t cm_to_m (uint8_t data[],uint16_t length);



#endif
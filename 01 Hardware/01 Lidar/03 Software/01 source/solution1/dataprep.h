#ifndef DATAPREP_H_
#define DATAPREP_H_

#define DEBUGGING 1

extern processVal_t bytes_to_values(uint8_t input[], uint16_t inLength, uint8_t output[], uint16_t outLength);
extern processVal_t round_values(uint8_t data[],uint16_t length);
extern processVal_t cm_to_m (uint8_t data[],uint16_t length);

typedef enum {NO_ERROR,NULL_POINTER,NOT_ENOUGH_OUTPUT_SPACE}processVal_t;

#endif
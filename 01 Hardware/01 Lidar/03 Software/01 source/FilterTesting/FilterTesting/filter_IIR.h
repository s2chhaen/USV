#ifndef FILTER_IIR_H_INCLUDED
#define FILTER_IIR_H_INCLUDED

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include "filterConfig.h"

#define IIR_FILTER_ORDER 2
#define OLD_VALUES_BUFFER_LEN_BITS (IIR_FILTER_ORDER/4 + 2)
#define OLD_VALUES_BUFFER_LEN (1<<OLD_VALUES_BUFFER_LEN_BITS)
#define OUTPUT_MAX_LEN 512

typedef struct{
    int32_t data[OLD_VALUES_BUFFER_LEN];
    uint8_t beginIdx :OLD_VALUES_BUFFER_LEN_BITS;
    uint8_t endIdx   :OLD_VALUES_BUFFER_LEN_BITS;
}ffOldBuffer_t;

typedef struct{
    double data[OLD_VALUES_BUFFER_LEN];
    uint8_t beginIdx :OLD_VALUES_BUFFER_LEN_BITS;
    uint8_t endIdx   :OLD_VALUES_BUFFER_LEN_BITS;
}ffOldBufferFloat_t;

enum iirType{
    IIR_MAXIMALLY_FLAT = 0x00
};

void iir_init(int16_t* inputFFCofs, uint16_t ffLen, int16_t* inputFBCofs, uint16_t fbLen);
void iir_runFiP(int32_t* data, int32_t* output, uint16_t len, uint8_t type);

#endif // FILTER_IIR_H_INCLUDED

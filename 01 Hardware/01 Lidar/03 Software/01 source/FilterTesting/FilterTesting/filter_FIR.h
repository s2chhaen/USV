#ifndef FILTER_FIR_H_INCLUDED
#define FILTER_FIR_H_INCLUDED

#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include "filterConfig.h"

#define FIR_FILTER_ORDER 3U
#define FIR_OLD_VALUES_BUFFER_LEN_BITS ((FIR_FILTER_ORDER-1U)/2U + 1U)
#define FIR_OLD_VALUES_BUFFER_LEN (1<<FIR_OLD_VALUES_BUFFER_LEN_BITS)

typedef struct{
    int32_t data[FIR_OLD_VALUES_BUFFER_LEN];
    uint8_t beginIdx :FIR_OLD_VALUES_BUFFER_LEN_BITS;
    uint8_t endIdx   :FIR_OLD_VALUES_BUFFER_LEN_BITS;
}ffOldBufferFIR_t;


#endif // FILTER_FIR_H_INCLUDED

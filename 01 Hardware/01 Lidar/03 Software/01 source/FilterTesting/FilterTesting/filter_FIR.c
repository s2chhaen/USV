#include "filter_FIR.h"

static uint8_t init = 0;
static int32_t ffCofs[FIR_FILTER_ORDER+1]={0};
static ffOldBufferFIR_t old = {0};

static int32_t qFormatARM2TI(int32_t input, uint16_t bits){
    int32_t result = 0;
    static const uint16_t bitLenMax = 20;
    const int32_t cFactorARM = (1<<bits) -1 ;
    if(bits<bitLenMax){
        result = (int32_t)((int64_t)input<<bits)/((int64_t)cFactorARM);
    }
    return result;
}

void fir_init(int16_t* inputFFCofs, uint16_t ffLen){
}

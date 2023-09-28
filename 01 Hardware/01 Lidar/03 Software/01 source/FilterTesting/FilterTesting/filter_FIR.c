#include "filter_FIR.h"

static uint8_t init = 0;
static int32_t ffCofs[FIR_FILTER_ORDER+1]={0};
static ffOldBufferFIR_t old = {0};

static double ffCofsFloat[FIR_FILTER_ORDER+1]={0};
static ffOldBufferFIRFloat_t oldFLoat = {0};

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
    uint8_t checkFf = ffLen==(FIR_FILTER_ORDER+1);

    if(!checkFf){
        printf("Die Ordnung von Filterpolynom nicht gleich der vom Filter");
    } else{
        //TODO Implementiert es mit memcpy von string.h
        for(int i = 0; i< ffLen; i++){
            ffCofs[i] = qFormatARM2TI((int32_t)inputFFCofs[i],FIXED_POINT_BITS);
            ffCofsFloat[i] = inputFFCofs[i]*1.0000/CONVERT_FACTOR_INT;
        }
        init = 1;
        old.beginIdx = 0;
        old.endIdx = FIR_FILTER_ORDER-1;
        oldFLoat.beginIdx = 0;
        oldFLoat.endIdx = FIR_FILTER_ORDER-1;
    }
}

void fir_runFiP(int32_t* data, int32_t* output, uint16_t len){

}

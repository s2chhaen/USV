#include "filter_IIR.h"

static uint8_t init = 0;
static int32_t ffCofs[IIR_FILTER_ORDER+1]={0};
static int32_t fbCofs[IIR_FILTER_ORDER+1]={0};
static ffOldBuffer_t old={0};

void iir_init(int16_t* inputFFCofs, uint16_t ffLen, int16_t* inputFBCofs, uint16_t fbLen){
    uint8_t checkFf = ffLen==(IIR_FILTER_ORDER+1);
    uint8_t checkFb = fbLen==(IIR_FILTER_ORDER+1);
    if(!(checkFf&&checkFb)){
        printf("Die Ordnung von Filterpolynom nicht gleich der vom Filter");
    } else{
        //TODO Implementiert es mit memcpy von string.h
        for(int i = 0; i< ffLen; i++){
            ffCofs[i] = (int32_t)inputFFCofs[i]&0xffff;//zu vermeiden von Missverstehen die erste Bit als Zeichenbit
            fbCofs[i] = (int32_t)inputFBCofs[i]&0xffff;//zu vermeiden von Missverstehen die erste Bit als Zeichenbit
        }
        init = 1;
        old.beginIdx = 0;
        old.endIdx = IIR_FILTER_ORDER;
    }
}

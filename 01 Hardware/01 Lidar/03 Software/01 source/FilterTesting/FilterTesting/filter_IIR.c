#include "filter_IIR.h"

static uint8_t init = 0;
static int32_t ffCofs[IIR_FILTER_ORDER+1]={0};
static int32_t fbCofs[IIR_FILTER_ORDER+1]={0};
static ffOldBuffer_t old = {0};

static double ffCofsFloat[IIR_FILTER_ORDER+1]={0};
static double fbCofsFloat[IIR_FILTER_ORDER+1]={0};
static ffOldBufferFloat_t oldFLoat = {0};

void iir_init(int16_t* inputFFCofs, uint16_t ffLen, int16_t* inputFBCofs, uint16_t fbLen){
    uint8_t checkFf = ffLen==(IIR_FILTER_ORDER+1);
    uint8_t checkFb = fbLen==(IIR_FILTER_ORDER+1);
    if(!(checkFf&&checkFb)){
        printf("Die Ordnung von Filterpolynom nicht gleich der vom Filter");
    } else{
        //TODO Implementiert es mit memcpy von string.h
        for(int i = 0; i< ffLen; i++){
            ffCofs[i] = (int32_t)inputFFCofs[i]&0xffff;//zu vermeiden von Missverstehen die erste Bit als Zeichenbit
            ffCofsFloat[i] = inputFFCofs[i]/CONVERT_FACTOR_INT;
            fbCofs[i] = (int32_t)inputFBCofs[i]&0xffff;//zu vermeiden von Missverstehen die erste Bit als Zeichenbit
            fbCofsFloat[i] = inputFBCofs[i]/CONVERT_FACTOR_INT;
        }
        init = 1;
        old.beginIdx = 0;
        old.endIdx = IIR_FILTER_ORDER;
    }
}

void iir_runFiP(int32_t* data, int32_t* output, uint16_t len, uint8_t type){
    int32_t ffValue = 0;
    uint8_t idxPtr = 0;
    uint8_t phaseShift_sample = 0;
    int32_t tempBuff[OUTPUT_MAX_LEN] = {0};
    int32_t temp = 0;
    memcpy(tempBuff,data,len*sizeof(tempBuff[0])/sizeof(uint8_t));
    const int32_t cFactor = (1<<FIXED_POINT_BITS) - 1;
    switch(type){
        case IIR_MAXIMALLY_FLAT:
            phaseShift_sample = 1;
            //kann nur Array mit max. (512 - shifted samples) Mitglieder bearbeiten
            len = ((OUTPUT_MAX_LEN-phaseShift_sample)>len)?len:(OUTPUT_MAX_LEN-phaseShift_sample);
            temp = tempBuff[len-1];
            for(int i = 0; i < phaseShift_sample; i++){
                tempBuff[len+i] = temp;
            }
            len += phaseShift_sample;
            for(int i = 0; i < len; i++){
                idxPtr = old.endIdx;
                //v(n) = x(n) - a1*v(n-1) - a2*v(n-2) - ... - au*v(n-u), u=Ordnung von Filter, a0 = 1
                ffValue = ((int64_t)fbCofs[0])*((int64_t)data[i])/cFactor;//TODO später betrachtet
                for(int j = 1; j <= IIR_FILTER_ORDER; j++){
                    ffValue -= ((int64_t)fbCofs[j])*((int64_t)old.data[idxPtr-j])/cFactor;
                }
                //y(n) = b0*v(n) + b1*v(n-1) + b2*v(n-2) + ... + bu*v(n-u), u wie oben
                tempBuff[i] = (int32_t)((int64_t)ffCofs[0])*((int64_t)ffValue)/cFactor;
                for(int j = 1; j <= IIR_FILTER_ORDER; j++){
                    tempBuff[i] += ((int64_t)ffCofs[j])*((int64_t)old.data[idxPtr-j])/cFactor;
                }
                //Aktualisieren der alten Werte
                old.beginIdx--;
                old.endIdx--;
                old.data[old.beginIdx] = ffValue;
            }
            len -= phaseShift_sample;
            memcpy(output,&tempBuff[phaseShift_sample],len*sizeof(tempBuff[0])/sizeof(uint8_t));
            break;
        default:
            break;
    }
}

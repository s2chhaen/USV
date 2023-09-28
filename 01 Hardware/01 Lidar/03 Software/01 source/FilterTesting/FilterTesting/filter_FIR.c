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
    uint8_t idxPtr = 0;
    uint8_t phaseShift_sample = 0;
    int32_t tempBuff[OUTPUT_MAX_LEN] = {0};
    int32_t tempOut[OUTPUT_MAX_LEN] = {0};
    int32_t temp = 0;
    const uint8_t bufferLen = FIR_OLD_VALUES_BUFFER_LEN;
    //TODO zu testen
    //memcpy(tempBuff,data,len*sizeof(tempBuff[0])/sizeof(uint8_t));
    for(int i = 0; i<len; i++)
    {
        tempBuff[i] = data[i];
    }
    phaseShift_sample = FIR_FILTER_ORDER/2 + 1;
    //kann nur Array mit max. (512 - shifted samples) Mitglieder bearbeiten
    len = ((OUTPUT_MAX_LEN-phaseShift_sample)>len)?len:(OUTPUT_MAX_LEN-phaseShift_sample);
    temp = tempBuff[len-1];
    for(int i = 0; i < phaseShift_sample; i++)
    {
        tempBuff[len+i] = temp;
    }
    len += phaseShift_sample;
    for(int i = 0; i < len; i++)
    {
        idxPtr = old.endIdx;
        tempOut[i] = ((int64_t)ffCofs[0])*((int64_t)tempBuff[i])>>FIXED_POINT_BITS;
        for(int j = 1; j <= FIR_FILTER_ORDER; j++) ///OK
        {
            tempOut[i] += ((int64_t)ffCofs[j])*((int64_t)old.data[(idxPtr-j+1+bufferLen)%bufferLen])>>FIXED_POINT_BITS;///OK
        }
        //Aktualisieren der alten Werte
        old.beginIdx++;///OK
        old.endIdx++;///OK
        old.data[old.endIdx] = tempBuff[i];///OK
    }

    for(int i = phaseShift_sample; i<len; i++) ///OK
    {
        output[i-phaseShift_sample] = tempOut[i];///OK
        printf("output[%d] = %" PRIi32 "\n",i-phaseShift_sample,tempOut[i]);///TODO löschen nach dem Testen
    }
    ///TODO zu testen
    //memcpy(output,&tempBuff[phaseShift_sample],len*sizeof(tempBuff[0])/sizeof(uint8_t));
}

#include "filter_IIR.h"

static uint8_t init = 0;
static int32_t ffCofs[IIR_FILTER_ORDER+1]={0};
static int32_t fbCofs[IIR_FILTER_ORDER+1]={0};
static ffOldBufferIIR_t old = {0};

static double ffCofsFloat[IIR_FILTER_ORDER+1]={0};
static double fbCofsFloat[IIR_FILTER_ORDER+1]={0};
static ffOldBufferIIRFloat_t oldFLoat = {0};

static int32_t qFormatARM2TI(int32_t input, uint16_t bits){
    int32_t result = 0;
    static const uint16_t bitLenMax = 20;
    const int32_t cFactorARM = (1<<bits) -1 ;
    if(bits<bitLenMax){
        result = (int32_t)((int64_t)input<<bits)/((int64_t)cFactorARM);
    }
    return result;
}

void iir_init(int16_t* inputFFCofs, uint16_t ffLen, int16_t* inputFBCofs, uint16_t fbLen){
    uint8_t checkFf = ffLen==(IIR_FILTER_ORDER+1);
    uint8_t checkFb = fbLen==(IIR_FILTER_ORDER+1);
    //int16_t temp = 0;
    if(!(checkFf&&checkFb)){
        printf("Die Ordnung von Filterpolynom nicht gleich der vom Filter");
    } else{
        //TODO Implementiert es mit memcpy von string.h
        for(int i = 0; i< ffLen; i++){
            ffCofs[i] = qFormatARM2TI((int32_t)inputFFCofs[i],FIXED_POINT_BITS);
            ffCofsFloat[i] = inputFFCofs[i]*1.0000/CONVERT_FACTOR_INT;
            fbCofs[i] = qFormatARM2TI((int32_t)inputFBCofs[i],FIXED_POINT_BITS);
            fbCofsFloat[i] = inputFBCofs[i]*1.0000/CONVERT_FACTOR_INT;
        }
        init = 1;
        old.beginIdx = 0;
        old.endIdx = IIR_FILTER_ORDER-1;
        oldFLoat.beginIdx = 0;
        oldFLoat.endIdx = IIR_FILTER_ORDER-1;
    }
}

void iir_runFiP(int32_t* data, int32_t* output, uint16_t len)
{
    int32_t ffValue = 0;
    uint8_t idxPtr = 0;
    uint8_t phaseShift_sample = 0;
    int32_t tempBuff[OUTPUT_MAX_LEN] = {0};
    int32_t tempOut[OUTPUT_MAX_LEN] = {0};
    int32_t temp = 0;
    const uint8_t bufferLen = IIR_OLD_VALUES_BUFFER_LEN;
    //TODO zu testen
    //memcpy(tempBuff,data,len*sizeof(tempBuff[0])/sizeof(uint8_t));
    for(int i = 0; i<len; i++)
    {
        tempBuff[i] = data[i];
    }
    phaseShift_sample = IIR_FILTER_ORDER/2;
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
        ffValue = ((int64_t)fbCofs[0])*((int64_t)tempBuff[i])>>FIXED_POINT_BITS;///OK
        for(int j = 1; j <= IIR_FILTER_ORDER; j++) ///OK
        {
            ffValue -= ((int64_t)fbCofs[j])*((int64_t)old.data[(idxPtr-j+1+bufferLen)%bufferLen])>>FIXED_POINT_BITS;///OK
        }

        tempOut[i] = (int32_t)((int64_t)ffCofs[0])*((int64_t)ffValue)>>FIXED_POINT_BITS;///OK
        for(int j = 1; j <= IIR_FILTER_ORDER; j++) ///OK
        {
            tempOut[i] += ((int64_t)ffCofs[j])*((int64_t)old.data[(idxPtr-j+1+bufferLen)%bufferLen])>>FIXED_POINT_BITS;///OK
        }
        //Aktualisieren der alten Werte
        old.beginIdx++;///OK
        old.endIdx++;///OK
        old.data[old.endIdx] = ffValue;///OK
    }

    for(int i = phaseShift_sample; i<len; i++) ///OK
    {
        output[i-phaseShift_sample] = tempOut[i];///OK
        ///printf("output[%d] = %" PRIi32 "\n",i-phaseShift_sample,tempOut[i]);///TODO löschen nach dem Testen
    }
    ///TODO zu testen
    //memcpy(output,&tempBuff[phaseShift_sample],len*sizeof(tempBuff[0])/sizeof(uint8_t));
}

void iir_runFlP(int32_t* data, double* output, uint16_t len)
{
    double ffValue = 0;
    uint8_t idxPtr = 0;
    double tempBuff[OUTPUT_MAX_LEN] = {0};
    double dataFl[OUTPUT_MAX_LEN] = {0};
    double temp = 0;
    const int32_t cFactor = (1<<FIXED_POINT_BITS);
    const uint8_t bufferLen = IIR_OLD_VALUES_BUFFER_LEN;
    uint8_t phaseShift_sample = 0;
    for(int i = 0; i<len; i++)
    {
        dataFl[i] = data[i]*1.0000/cFactor;
        ///printf("input[%d] = %lf \n",i,dataFl[i]);///TODO löschen nach dem Testen
    }
    phaseShift_sample = IIR_FILTER_ORDER/2;
    len = ((OUTPUT_MAX_LEN-phaseShift_sample)>len)?len:(OUTPUT_MAX_LEN-phaseShift_sample);
    temp = dataFl[len-1];
    for(int i = 0; i < phaseShift_sample; i++)
    {
        dataFl[len+i] = temp;
    }
    len += phaseShift_sample;
    for(int i = 0; i<len; i++)
    {
        ///printf("input[%d] = %lf \n",i,dataFl[i]);///TODO löschen nach dem Testen
    }
    for(int i = 0; i < len; i++)
    {
        idxPtr = oldFLoat.endIdx;
        ffValue = fbCofsFloat[0]*dataFl[i];///OK
        for(int j = 1; j <= IIR_FILTER_ORDER; j++) ///OK
        {
            ffValue -= fbCofsFloat[j]*oldFLoat.data[(idxPtr-j+1+bufferLen)%bufferLen];///OK
        }
        tempBuff[i] = ffCofsFloat[0]*ffValue;///OK
        for(int j = 1; j <= IIR_FILTER_ORDER; j++) ///OK
        {
            tempBuff[i] += ffCofsFloat[j]*oldFLoat.data[(idxPtr-j+1+bufferLen)%bufferLen];///OK
        }
        //Aktualisieren der alten Werte
        oldFLoat.beginIdx++;///OK
        oldFLoat.endIdx++;///OK
        oldFLoat.data[oldFLoat.endIdx] = ffValue;///OK
    }
    for(int i = phaseShift_sample; i<len; i++) ///OK
    {
        output[i-phaseShift_sample] = tempBuff[i];///OK
        ///printf("input[%d] = %lf \n",i-1,tempBuff[i]);///TODO löschen nach dem Testen
    }
}

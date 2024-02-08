#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include "ioModul.h"
#include "protocol.h"
#include "filter_FIR.h"
#include "FilterCoefs/wr_qFormFilterCof.h"

int main()
{
    uint8_t data[1024] = {0};
    uint16_t dataLen = sizeof(data)/sizeof(uint8_t);
//#define SEG_0_ACTIVE
#ifdef SEG_0_ACTIVE
    readFile("31h-LMS-Status-anfordern.txt");
    getData(data,&dataLen);
    checkConfig(data,dataLen);
    dataLen = sizeof(data)/sizeof(uint8_t);
#endif // SEG_0_ACTIVE

#define SEG_1_ACTIVE
#ifdef SEG_1_ACTIVE
    uint8_t protocolCheck = 0;
    int32_t measuredVal[800] = {0};
    uint16_t measuredValLen = sizeof(measuredVal)/sizeof(int32_t);
    int64_t output[800] = {0};
    //Messwerte
    readFile("30h-01h-Messwerte-anfordern.txt");
    getData(data,&dataLen);
    protocolCheck = checkData(data,dataLen);
    dataLen = sizeof(data)/sizeof(uint8_t);
    if(protocolCheck){
        getExtractedData(measuredVal,&measuredValLen);
    }
    for(int i = 0; i < measuredValLen; i++){
        measuredVal[i] <<= FIXED_POINT_BITS;

    }
    writeFileInt32((uint8_t*)"../Lidar-Algorithmus/lidarValues_raw", \
                   sizeof("../Lidar-Algorithmus/lidarValues_raw")/sizeof(uint8_t), \
                   TEXT_FORM, measuredVal,measuredValLen);
    fir_init((int16_t*)wr_num,wr_numLen);
    fir_runFiP(measuredVal,output,measuredValLen);
    writeFileInt64((uint8_t*)"../Lidar-Algorithmus/lidarValues_filtered", \
                   sizeof("../Lidar-Algorithmus/lidarValues_filtered")/sizeof(uint8_t), \
                   TEXT_FORM, output,measuredValLen);
#endif // SEG_1_ACTIVE
    return 0;
}

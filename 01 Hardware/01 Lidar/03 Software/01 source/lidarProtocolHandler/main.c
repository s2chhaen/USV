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
#define SEG_0_ACTIVE
#ifdef SEG_0_ACTIVE
    readFile("31h-LMS-Status-anfordern.txt");
    getData(data,&dataLen);
    checkConfig(data,dataLen);
    dataLen = sizeof(data)/sizeof(uint8_t);
#endif // SEG_0_ACTIVE
    return 0;
}

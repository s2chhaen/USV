#include "main.h"

#include <stdint.h>
#include "filter_IIR.h"
#include "ioModul.h"
#include "filterCoefs/mf_qFormFilterCof.h"

double a = 1*1.00000;

int main()
{
    int32_t temp[BUFFER_MAX_LENGTH]={0};
    int32_t filtered[BUFFER_MAX_LENGTH]={0};
    uint16_t len = sizeof(temp)/sizeof(int32_t);
    readFile();
    getData(temp,&len);
    //Test OK
    iir_init((int16_t*)mf_num,mf_numLen,(int16_t*)mf_den,mf_denLen);
    return 0;
}

#include "main.h"

#include <stdint.h>
#include "filter_IIR.h"
#include "ioModul.h"
#include "filterCoefs/mf_qFormFilterCof.h"

double a = 1*1.00000;

int main()
{
    int32_t temp[BUFFER_MAX_LENGTH]={0};
#define SEG_1_MAIN
#ifdef SEG_1_MAIN
    int32_t filtered[BUFFER_MAX_LENGTH]={0};
#endif // SEG_1_MAIN

#define SEG_2_MAIN
#ifdef SEG_2_MAIN
    double dFilterd[BUFFER_MAX_LENGTH]={0};
#endif // SEG_2_MAIN
    uint16_t len = sizeof(temp)/sizeof(int32_t);
    readFile();
    getData(temp,&len);
    iir_init((int16_t*)mf_num,mf_numLen,(int16_t*)mf_den,mf_denLen);

#ifdef SEG_1_MAIN
    iir_runFiP(temp,filtered,len);
    writeFile((uint8_t*)"output_mf_fixed_c",sizeof("output_mf_fixed_c"),TEXT_FORM,filtered,len);
    writeFile((uint8_t*)"output_mf_fixed_c",sizeof("output_mf_fixed_c"),CSV_FORM,filtered,len);
#endif // SEG_1_MAIN

#ifdef SEG_2_MAIN
    iir_runFlP(temp,dFilterd,len);
#endif // SEG_2_MAIN
    return 0;
}

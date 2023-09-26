#include "filter_IIR.h"

static uint8_t init = 0;
static int32_t ffCofs[IIR_FILTER_ORDER+1]={0};
static int32_t fbCofs[IIR_FILTER_ORDER+1]={0};
static ffOldBuffer_t old={0};

void iir_init(int16_t* inputFFCofs, uint16_t ffLen, int16_t* inputFBCofs, uint16_t fbLen){
}

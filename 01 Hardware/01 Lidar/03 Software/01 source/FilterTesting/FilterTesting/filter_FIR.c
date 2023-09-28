#include "filter_FIR.h"

static uint8_t init = 0;
static int32_t ffCofs[FIR_FILTER_ORDER+1]={0};
static ffOldBufferFIR_t old = {0};

void fir_init(int16_t* inputFFCofs, uint16_t ffLen){
}

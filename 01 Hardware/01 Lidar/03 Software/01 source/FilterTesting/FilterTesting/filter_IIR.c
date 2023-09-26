#include "filter_IIR.h"

static uint8_t init = 0;
static int32_t ffCofs[IIR_FILTER_ORDER+1]={0};
static int32_t fbCofs[IIR_FILTER_ORDER+1]={0};
static ffOldBuffer_t old={0};

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include "ioModul.h"
#include "protocol.h"

int main()
{
    uint8_t data[1024] = {0};
    uint16_t dataLen = sizeof(data)/sizeof(uint8_t);
    printf("Hello world!\n");
    return 0;
}

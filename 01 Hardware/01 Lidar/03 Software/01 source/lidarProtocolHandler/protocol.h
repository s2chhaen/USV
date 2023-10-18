#ifndef PROTOCOL_H_INCLUDED
#define PROTOCOL_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "lib/crcCalculator.h"

#define ADDR_DEFAULT 0
#define MAX_DATA_LIDAR 361
#define DEFAULT_CRC16_POLYNOM 0x8005

enum reqProtocolHeader{
    REQ_START_BYTE,
    REQ_ADDR_BYTE,
    REQ_LENGTH_L_BYTE,
    REQ_LENGTH_H_BYTE,
    REQ_CMD_BYTE
};
#endif // PROTOCOL_H_INCLUDED

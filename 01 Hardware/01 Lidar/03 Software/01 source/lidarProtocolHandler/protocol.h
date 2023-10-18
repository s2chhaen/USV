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

enum cmd30hBlockAComp_bm{
    UNIT_BIT_BM = (0x03<<14),
    SCAN_TYPE_BM = (0x01<<13),//Standard or Interlace
    PARTIAL_SCAN_RES_BM = (0x03<<11),
    VAL_NO_BM = (0x3FF<<0)
};

enum reqProtocolHeader{
    REQ_START_BYTE,
    REQ_ADDR_BYTE,
    REQ_LENGTH_L_BYTE,
    REQ_LENGTH_H_BYTE,
    REQ_CMD_BYTE
};

enum rspProtocolHeader{
    RSP_TX_RSL_BYTE,
    RSP_START_BYTE,
    RSP_ADDR_BYTE,
    RSP_LENGTH_L_BYTE,
    RSP_LENGTH_H_BYTE,
    RSP_CMD_BYTE
};
#define WORD 2U
#define DWORD 4U
#define BYTE 1U
#define CHAR 1U
#define OFFSET_BYTES 6

#define CONFIG_BLK_A (7*CHAR)
#define CONFIG_BLK_B CHAR
#define CONFIG_BLK_C CHAR
#define CONFIG_BLK_D WORD
#define CONFIG_BLK_E CHAR
#define CONFIG_BLK_F (8*WORD)
#define CONFIG_BLK_G (4*WORD)
#define CONFIG_BLK_H (8*WORD)
#define CONFIG_BLK_I (4*WORD)
#define CONFIG_BLK_J WORD
#define CONFIG_BLK_K WORD
#define CONFIG_BLK_L WORD
#define CONFIG_BLK_M WORD
#define CONFIG_BLK_N WORD
#define CONFIG_BLK_O WORD
#define CONFIG_BLK_P WORD
#define CONFIG_BLK_Q WORD
#define CONFIG_BLK_R WORD
#define CONFIG_BLK_S WORD
#define CONFIG_BLK_T WORD
#define CONFIG_BLK_U WORD
#define CONFIG_BLK_V WORD
#define CONFIG_BLK_W WORD
#define CONFIG_BLK_X WORD
#define CONFIG_BLK_Y WORD
#define CONFIG_BLK_Z WORD
#define CONFIG_BLK_A1 BYTE
#define CONFIG_BLK_A2 BYTE
#define CONFIG_BLK_A3 WORD
#define CONFIG_BLK_A4 WORD
#define CONFIG_BLK_A5 WORD
#define CONFIG_BLK_A6 WORD
#define CONFIG_BLK_A7 BYTE
#define CONFIG_BLK_A8 WORD
#define CONFIG_BLK_A9 BYTE
#define CONFIG_BLK_B1 BYTE
#define CONFIG_BLK_B2 WORD
#define CONFIG_BLK_B3 BYTE
#define CONFIG_BLK_B4 BYTE
#define CONFIG_BLK_B5 BYTE
#define CONFIG_BLK_B6 BYTE
#define CONFIG_BLK_B7 BYTE
#define CONFIG_BLK_B8 BYTE
#define CONFIG_BLK_B9 7*CHAR
#define CONFIG_BLK_C1 DWORD
#define CONFIG_BLK_C2 DWORD
#define CONFIG_BLK_C3 DWORD
#define CONFIG_BLK_C4 DWORD
#define CONFIG_BLK_C5 WORD
#define CONFIG_BLK_C6 WORD
#define CONFIG_BLK_C7 WORD
#define CONFIG_STATUS_LMS 1U

extern uint8_t checkData(uint8_t* data, uint16_t dataLen);
extern void getExtractedData(int32_t* data, uint16_t* dataLen);
extern void checkConfig(uint8_t* data, uint16_t dataLen);
extern void hwdInitAndResetReq(uint8_t* protocol, uint16_t* len);
#endif // PROTOCOL_H_INCLUDED

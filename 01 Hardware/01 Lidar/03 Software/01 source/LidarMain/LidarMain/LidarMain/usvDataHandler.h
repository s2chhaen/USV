/*
 * usvDataHandler.h
 *
 * Created: 7/7/2023 5:25:53 AM
 * Author: Thach
 * Version: 1.4
 * Revision: 1.1
 */ 

#ifndef USVDATAHANDLER_H_
#define USVDATAHANDLER_H_

#include <util/atomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ATMegaXX09/USART/USART.h"
#include "ATMegaXX09/FIFO/FIFO.h"
#include "Math/checksum.h"
#include "timerUnit.h"
#include "comConfig.h"
#include "registerModel.h"

//Protocol-Parameter
#define USV_PROTOCOL_W_REQ 8
#define USV_PROTOCOL_R_REQ 4
#define USV_PROTOCOL_SET_SLAVE_ADD_LOW(add) (uint8_t)(add&0xff)
#define USV_PROTOCOL_SET_SLAVE_ADD_HIGH(add,rw) (uint8_t)((add>>8)|(rw<<4))
//Parameter des Datenrahmens
#define MAX_FRAME_LEN_BIT 8UL
#define MAX_FRAME_LEN ((1<<MAX_FRAME_LEN_BIT)-1)
#define PROTOCOL_OVERHEAD_LEN 7
#define PROTOCOL_PAYLOAD_PER_FRAME (MAX_FRAME_LEN - PROTOCOL_OVERHEAD_LEN)

//Zeit für Zielprogramm (MATLAB)
#define USV_BYTE_TRANSFER_TIME_US 45
#define USV_DST_PROG_WORK_TIME_US 1000000
#define USV_TOLERANCE_MS 20
//Protokoll
#define USV_PROTOCOL_START_BYTE 0xA5
#define USV_PROTOCOL_END_BYTE 0xA6
#define USV_PROTOCOL_ACK_BYTE 0xA1
//Empfangen Byte
#define USV_1ST_RX_LEN 1
#define USV_2ND_RX_LEN 4
//Anzahl der Probe
#define USV_RETRY_TIME_MAX 1


#endif /* USVDATAHANDLER_H_ */
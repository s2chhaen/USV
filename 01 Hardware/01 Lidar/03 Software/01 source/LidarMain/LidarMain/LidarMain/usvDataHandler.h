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

#endif /* USVDATAHANDLER_H_ */
/*
 * lidarHandler.h
 *
 * Created: 12/12/2023 11:53:20 AM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.0
 */ 


#ifndef RADARHANDLER_H_
#define RADARHANDLER_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include "ATMegaXX09/USART/USART.h"
#include "ATMegaXX09/FIFO/FIFO.h"
#include "errorList.h"
#include "timerUnit.h"
#include "asciiTable.h"
#include "registerModel.h"
#include "comConfig.h"

//Radar-bezogene Parameter
#define RADAR_SYNC_MSG "{\"Reset\":\"Board was reset.\x20\x20\'?R\' returns details\x22}\r\n"
#define RADAR_PROTOCOL_MAX_LEN 3
#define RADAR_DATA_REQ_CMD "PP"
#define RADAR_REQ_PROTOCOL_LEN (sizeof(RADAR_DATA_REQ_CMD)/sizeof(uint8_t) - 1)
#define RADAR_RESET_CMD "P!"
#define RADAR_RESET_PROTOCOL_LEN (sizeof(RADAR_RESET_CMD)/sizeof(uint8_t) - 1)
#define RADAR_DATA_REQ_MAX_PACK 2
#define RADAR_SYNC_PROTOCOL_START_SYM 0x7b
#define RADAR_DATA_PROTOCOL_START_SYM 0x22
#define RADAR_DATA_PROTOCOL_NEAR_END_SYM 0x0D
#define RADAR_PROTOCOL_END_SYM 0x0a

#define RADAR_VEL_RSP_LEN 11
#define RADAR_DIS_RSP_LEN 9
//Ausgabe Param für Main
#define RADAR_OUTPUT_IDEAL_LEN 3
//Arbeitszeit auf Radar-Seite
#define RADAR_TOLERANCE_MS 20
#define RADAR_WRK_TIME_MS 400
//config 1 Startbit, 8 Databit, 0 Paritybit, 1 Endbit, Baud = 38,4kBaud
#define RADAR_TX_TIME_US 521UL


#endif /* RADARHANDLER_H_ */
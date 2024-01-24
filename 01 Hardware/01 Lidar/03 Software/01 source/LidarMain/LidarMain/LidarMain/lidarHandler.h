/*
 * lidarHandler.h
 *
 * Created: 11/26/2023 4:03:53 PM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.1
 */ 


#ifndef LIDARHANDLER_H_
#define LIDARHANDLER_H_

#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include "ATMegaXX09/USART/USART.h"
#include "ATMegaXX09/FIFO/FIFO.h"
#include "Math/checksum.h"
#include "errorList.h"
#include "lidarHandlerProtocol.h"
#include "timerUnit.h"
#include "registerModel.h"
#include "comConfig.h"

/*Lidar-bezogene Parameter Macros*/
#define LIDAR_PROTOCOL_START_SYM 0x02
#define LIDAR_PROTOCOL_DEFAULT_ADDR 0x00
#define LIDAR_PROTOCOL_CANCEL_SYM 0x03
#define LIDAR_RESET_PROTOCOL_LEN 7
#define LIDAR_TOLERANCE_MS 20
#define LIDAR_WRK_TIME_MS 100
//config 1 Startbit, 8 Databit, 0 Paritybit, 1 Endbit, Baud = 38,4kBaud
#define LIDAR_TX_TIME_US 261UL
#define LIDAR_RSP_PROTOCOL_OFFSET 0x80
#define LIDAR_RSP_ADDR (LIDAR_PROTOCOL_DEFAULT_ADDR + LIDAR_RSP_PROTOCOL_OFFSET)
#define LIDAR_RSP_CMD(cmd) (cmd+0x80)
#define LIDAR_REQ_PROTOCOL_MAX_LEN 20
#define LIDAR_RESET_RSP_LEN 8
//Zwischenbuffer-Parameter
#define LIDAR_RX_BUFFER_MAX_LEN 735UL
//Lidar-Status
#define LIDAR_STATUS_REG_LEN_BYTE 2

/*enum: Arbeitsmodus vom Lidar*/
enum lidarMode{
	LIDAR_PARAM_MODE,
	LIDAR_RESET_MODE,
	LIDAR_GETTER_MODE
};
#define LIDAR_LAST_MODE LIDAR_GETTER_MODE
#define LIDAR_MODE_NUM (LIDAR_LAST_MODE+1)

/*Byte-Positionen im Protokoll*/
//Generell
enum lidarProtocolBytesPos{
	LIDAR_START_BYTE_POS, //byte 0
	LIDAR_ADDR_BYTE_POS, //byte 1
	LIDAR_PROTOCOL_LEN_LBYTE_POS, //byte 2
	LIDAR_PROTOCOL_LEN_HBYTE_POS, //byte 3
	LIDAR_PROTOCOL_CMD_BYTE_POS //byte 4
};
//Meldung-nach-ON-Protokoll
enum lidarPromptProtocolDataBytePos{
	LIDAR_INFO_TYPE_BYTE_POS = 0,
	LIDAR_INFOR_VER_BYTE_POS = 14
};

/*Zustand-Deklaration für FSM*/
//Parametrierung-FSM
enum lidar_paramFsmState{
	LIDAR_PARAM_FSM_START_STATE,
	LIDAR_PARAM_FP_HANDLE_STATE,
	LIDAR_PARAM_FSM_RX_CHECK_1_OHD_STATE,
	LIDAR_PARAM_FSM_RX_STATE,
	LIDAR_PARAM_FSM_RX_CHECK_2_OHD_STATE,
	LIDAR_PARAM_FSM_END_STATE
};
#define LIDAR_PARAM_FSM_LAST_STATE LIDAR_PARAM_FSM_END_STATE
#define LIDAR_PARAM_FSM_STATE_NUM (LIDAR_PARAM_FSM_LAST_STATE+1)

//Rücksetzen-FSM
enum lidar_resetFsmState{
	LIDAR_RESET_FSM_START_STATE,
	LIDAR_RESET_FSM_RX_STATE,
	LIDAR_RESET_FSM_TERMINAL_STATE,
	LIDAR_RESET_FSM_END_STATE
};
#define LIDAR_RESET_FSM_LAST_STATE LIDAR_RESET_FSM_END_STATE
#define LIDAR_RESET_FSM_STATE_NUM (LIDAR_RESET_FSM_LAST_STATE+1)

#endif /* LIDARHANDLER_H_ */
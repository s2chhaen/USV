/*
 * lidarHandler.h
 *
 * Created: 11/26/2023 4:03:53 PM
 *  Author: Thach
 */ 


#ifndef LIDARHANDLER_H_
#define LIDARHANDLER_H_

#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include "ATMegaXX09/USART/USART.h"
#include "ATMegaXX09/FIFO/FIFO.h"
#include "Math/checksum.h"

#define LIDAR_PROTOCOL_START_SYM 0x02
#define LIDAR_PROTOCOL_DEFAULT_ADDR 0x00
#define LIDAR_PROTOCOL_CANCEL_SYM 0x03
//rsp: response
#define LIDAR_PROTOCOL_RESP_CONVERT_COEF 0x80
#define LIDAR_REQ_PROTOCOL_MAX_LEN 20

enum lidarProtocolBytesPos{//pos: position
	LIDAR_START_BYTE_POS,
	LIDAR_ADDR_BYTE_POS,
	LIDAR_PROTOCOL_LEN_BYTE_POS, //len: length => len = data bytes len + 2-checksum (crc16) bytes
	LIDAR_DATA_INCL_CMD_BYTE_POS //cmd: command, incl: include
};

#endif /* LIDARHANDLER_H_ */
/*
 * lidarHandlerProtocol.h
 *
 * Created: 10/8/2023 3:13:45 PM
 * Author: Thach
 * Version: 1.0
 * Revison: 1.0
 */ 


#ifndef LIDARHANDLERPROTOCOL_H_
#define LIDARHANDLERPROTOCOL_H_

#include "asciiTable.h"

//Ver: Version, RESO: resolution, BM: bitmap
#define LIDAR_TYPE_STR "LMS200"
#define LIDAR_TYPE_STR_LEN (sizeof(LIDAR_TYPE_STR)/sizeof(uint8_t) - 1)
#define LIDAR_VER_STR "V01.10"
#define LIDAR_VER_STR_LEN (sizeof(LIDAR_VER_STR)/sizeof(uint8_t) - 1)
#define LIDAR_DATA_FRAME_CM_CONGIG (0x00 << 14)
#define LIDAR_DATA_FRAME_SCAN_TYPE_CONFIG (0x00 << 11)
#define LIDAR_SET_CONFIG (LIDAR_DATA_FRAME_CM_CONGIG|LIDAR_DATA_FRAME_SCAN_TYPE_CONFIG)
#define LIDAR_SET_CONFIG_BM ((0x03<<14)|(0x07<<11))


//siehe "Telegramme zur Konfiguration und Bedienung der Lasermesssysteme LMS2xx-V2.30"-S38-39
enum lidarAns{
	RESET_ACK = 0x91,
	NACK_FALSE_CMD = 0x92
};

#endif /* LIDARHANDLERPROTOCOL_H_ */
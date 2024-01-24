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

//siehe "Telegramme zur Konfiguration und Bedienung der Lasermesssysteme LMS2xx-V2.30"-S36-37
enum lidarCmd{
	LIDAR_INIT_N_RESET = 0x10,
	OP_MODE_SEL = 0x20,
	MEASURED_DATA_REQ = 0x30,
	STATUS_REQ = 0x031,//Nur für LMS2xx möglich
	ERROR_OR_TEST_TELEGRAM_REQ = 0x32,
	OP_DATA_COUNTER_REQ = 0x35,
	AVG_MEASURED_DATA_REQ = 0x36,
	SEG_MEASURED_DATA_REQ = 0x37,
	LIDAR_TYPE_REQ = 0x3A,
	MEASURED_CONFIG_CHANGE = 0x3B,
	MEASURED_DATA_WITH_FIELD_DATA_REQ = 0x3E,
	SEG_AVG_MEASURED_DATA_REQ = 0x3F,
	FIELD_ABC_CONFIG = 0x40,
	ACTIVE_FIELD_SET_CHANGE = 0x41,
	PWD_CHANGE = 0x42,
	SEG_MEASURED_DATA_AND_REFLECTANCE_REQ = 0x44,
	FIELD_REQ = 0x45,
	LEARNING_MODE_START = 0x46,
	FIELDS_STATUS_OUT_REQ = 0x4A,
	BAUDRATE_OR_LIDAR_TYPE_DEF = 0x66,
	ANGLE_RNG_POSITIONING_SUP = 0x69,
	LIDAR_CONFIG_P1_REQ = 0x74,//Nur für LMS2xx möglich
	MEASURED_DATA_WITH_REFLECTANCE_REQ = 0x75,
	MEASURED_DATA_IN_XY_COORD_REQ = 0x76,
	LIDAR_CONFIG_P1 = 0x77,//Nur für LMS2xx möglich
	LIDAR_CONFIG_P2_REQ = 0x7B,//Nur für LMS2xx möglich
	LIDAR_CONFIG_P2 = 0x7C//Nur für LMS2xx möglich
};

//Relative Position der Blöcke im Antwortprotokoll
enum ansProtocolBlockPos{
	BLOCK_A, BLOCK_B, BLOCK_C, BLOCK_D, BLOCK_E, BLOCK_F, BLOCK_G, BLOCK_H,
	BLOCK_I, BLOCK_J, BLOCK_K, BLOCK_M, BLOCK_N, BLOCK_O, BLOCK_P, BLOCK_Q,
	BLOCK_R, BLOCK_S, BLOCK_T, BLOCK_U, BLOCK_V, BLOCK_W, BLOCK_X, BLOCK_Y, BLOCK_Z,
	BLOCK_A1, BLOCK_A2, BLOCK_A3, BLOCK_A4, BLOCK_A5, BLOCK_A6, BLOCK_A7, BLOCK_A8, BLOCK_A9,
	BLOCK_B1, BLOCK_B2, BLOCK_B3, BLOCK_B4, BLOCK_B5, BLOCK_B6, BLOCK_B7, BLOCK_B8, BLOCK_B9,
	BLOCK_C1, BLOCK_C2, BLOCK_C3, BLOCK_C4, BLOCK_C5, BLOCK_C6, BLOCK_C7,
};

//siehe "Telegramme zur Konfiguration und Bedienung der Lasermesssysteme LMS2xx-V2.30"-S38-39
enum lidarAns{
	RESET_ACK = 0x91,
	NACK_FALSE_CMD = 0x92
};

//Subkommandos von OP_MODE_SEL siehe "Telegramme zur Konfiguration und Bedienung der Lasermesssysteme LMS2xx-V2.30"-S40-44
enum opModeSelSubcmd{
	OP_MODE_SEL_GRP_A_SETUP_MODE = 0x00,//Gruppe A: Einrichtmodus
	OP_MODE_SEL_GRP_A_DIAG_MODE = 0x10,//Gruppe A: Diagnosemodus
	OP_MODE_SEL_GRP_B_WRKG_MODE_0 = 0x20,//Gruppe B: Arbeitsmodus
	OP_MODE_SEL_GRP_B_WRKG_MODE_1 = 0x21,
	OP_MODE_SEL_GRP_B_WRKG_MODE_2 = 0x22,
	OP_MODE_SEL_GRP_B_WRKG_MODE_3 = 0x23,
	OP_MODE_SEL_GRP_B_WRKG_MODE_4 = 0x24,
	OP_MODE_SEL_GRP_B_WRKG_MODE_5 = 0x25,
	OP_MODE_SEL_GRP_B_WRKG_MODE_6 = 0x26,
	OP_MODE_SEL_GRP_B_WRKG_MODE_7 = 0x27,
	OP_MODE_SEL_GRP_B_WRKG_MODE_8 = 0x28,
	OP_MODE_SEL_GRP_B_WRKG_MODE_9 = 0x29,
	OP_MODE_SEL_GRP_B_WRKG_MODE_10 = 0x2A,
	OP_MODE_SEL_GRP_B_WRKG_MODE_11 = 0x2B,
	OP_MODE_SEL_GRP_B_WRKG_MODE_12 = 0x2C,
	OP_MODE_SEL_GRP_B_WRKG_MODE_13 = 0x2E,
	OP_MODE_SEL_GRP_B_WRKG_MODE_14 = 0x50,
	OP_MODE_SEL_GRP_C_PWD = 0x30,//Gruppe C: Test-Passwort
	OP_MODE_SEL_GRP_D_DTR_CONF_1 = 0x40,//Gruppe D: Data-Transfer-Rate(DTR)/Datenübertragungsrate = 38,4kBd
	OP_MODE_SEL_GRP_D_DTR_CONF_2 = 0x41,//Gruppe D: DTR = 19,2kBd
	OP_MODE_SEL_GRP_D_DTR_CONF_3 = 0x42,//Gruppe D: DTR = 9,6kBd
	OP_MODE_SEL_GRP_D_DTR_CONF_4 = 0x48,//Gruppe D: DTR = 500kBd
};

//Datenteil von Antwortprotokoll von OP_MODE_SEL siehe "Telegramme zur Konfiguration und Bedienung der Lasermesssysteme LMS2xx-V2.30" - S45
enum opModeSelAns{
	OP_MODE_SEL_SUCCESS = 0x00,//Moduswechsel erfolgreich
	OP_MODE_SEL_FALSE_PWD = 0x01,//nicht möglich, da falsches Passwort
	OP_MODE_SEL_ERROR = 0x02//nicht möglich, da Fehler im Lidar
};

//Datenteil von Antwortprotokoll von MEASURED_DATA_REQ siehe "Telegramme zur Konfiguration und Bedienung der Lasermesssysteme LMS2xx-V2.30" - S49
enum measuredDataReqAnsBlockA{
	MEASURED_DATA_REQ_UNIT_CM = (0x00 << 14),
	MEASURED_DATA_REQ_UNIT_MM = (0x01 << 14),
	MEASURED_DATA_REQ_ALL = (0x00 << 13),
	MEASURED_DATA_REQ_PARTIAL = (0x01 << 13),
	MEASURED_DATA_REQ_PARTIAL_1_00 = (0x00 << 11),
	MEASURED_DATA_REQ_PARTIAL_0_25 = (0x01 << 11),
	MEASURED_DATA_REQ_PARTIAL_0_50 = (0x10 << 11),
	MEASURED_DATA_REQ_PARTIAL_0_75 = (0x11 << 11),
};

#endif /* LIDARHANDLERPROTOCOL_H_ */
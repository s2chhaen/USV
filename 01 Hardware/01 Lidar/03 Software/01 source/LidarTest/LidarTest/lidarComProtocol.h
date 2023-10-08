/*
 * lidarComProtocol.h
 *
 * Created: 10/8/2023 3:13:45 PM
 *  Author: Thach
 */ 


#ifndef LIDARCOMPROTOCOL_H_
#define LIDARCOMPROTOCOL_H_

#define STX_SYMBOL 0x02
#define LIDAR_DEFAULT_ADDR 0x00
#define ETX_SYMBOL 0x03
#define REQ_2_ANS_CMD_CONVERT_COEF 0x80

//siehe "Telegramme zur Konfiguration und Bedienung der Lasermesssysteme LMS2xx-V2.30"-S36-37
enum lidarCmd{
	INIT_AND_RESET = 0x10,
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

#endif /* LIDARCOMPROTOCOL_H_ */
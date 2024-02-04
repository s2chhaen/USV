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

typedef enum {
	//Sensorblock
	SB1_ADD = 0x00,//00-SB1
	SB2_ADD = 0x01,//01-SB2
	SB3_ADD = 0x05,//02-SB3
	SB4_ADD = 0x09,//03-SB4
	SB5_ADD = 0x0A,//04-SB5
	SB6_ADD = 0x0C,//05-SB6
	SB7_ADD = 0x0E,//06-SB7
	//Radar
	SB8_ADD = 0x12,//07-SB8
	SB9_ADD = 0x14,//08-SB9
	//Führungsgrößen der Antriebsregelung
	AF1_ADD = 0x100,//09-AF1
	AF2_ADD = 0x108,//10-AF2
	AF3_ADD = 0x110,//11-AF3
	AF4_ADD = 0x112,//12-AF4
	//Stellgrößen der Antriebsregelung
	AS1_ADD = 0x120,//13-AS1
	AS2_ADD = 0x122,//14-AS2
	//lokaler Error Block
	ER1_ADD = 0x200,//15-ER1
	ER2_ADD = 0x201,//16-ER2
	ER3_ADD = 0x202,//17-ER3
	ER4_ADD = 0x203,//18-ER4 noch zu aktualisieren im USV-Dokument
	ER5_ADD = 0x205,//19-ER5 noch zu aktualisieren im USV-Dokument
	//Lidar
	ES1_ADD =0x220//20-ES1
}usvRegAdd_t;
#define USV_LAST_DATA_BLOCK_ADDR ES1_ADD

enum usvBytesPosInProtocolHeader{
	USV_START_BYTE_POS,
	USV_OBJ_ID_BYTE_POS,
	USV_REG_ADDR_AND_WR_LBYTE_POS,
	USV_REG_ADDR_AND_WR_HBYTE_POS,
	USV_FRAME_LEN_BYTE_POS,
	USV_DATA_BEGIN_POS
};

/*Arbeitsmodus vom USV*/
enum usvMode{
	USV_GETTER_MODE,
	USV_SETTER_MODE
};//Nur Setter implementiert, da getter in diesem Modul nicht notwendig ist
#define USV_LAST_MODE USV_SETTER_MODE
#define USV_MODE_NUM (USV_LAST_MODE+1)


#endif /* USVDATAHANDLER_H_ */
/*
 * main.h
 *
 * Created: 15.06.2023 13:07:06
 *  Author: Thach
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "./ATMegaXX09/ATMegaXX09Clock.h"
#include "./ATMegaXX09/USART/USART.h"
#include "slaveDevice.h"
#include "timerUnit.h"

//Header
#define HEADER_LENGTH 8
//Symbol in Telegram
#define TELG_LENGTH 732
#define ACK_SYMBOL 0x06
#define NACK_SYMBOL 0xA2
#define STX_SYMBOL 0x02
#define ADR_SLV 0x80
#define LENGTH_LOW 0xD6
#define LENGTH_HIGH 0x02

#define NO_OF_VALUE 361
#define PARTIAL_SCAN_00 0b0
#define PARTIAL_SCAN 0b0
#define CM_U 0b0
//Commands and response
#define DATA_REQ_RESP 0xB0
#define SETUP_MODE 0x0

#define SET_OP_MODE 0x20
#define ALL_VALUE_COUNTINUE 0x24

#define DISBL_MPC_MODE false
#define DISBL_SYNC_TX false
#define SENSOR_ADR 0x80

#define DATA_STREAM_SIZE 361
#define MAX_FRAME_LENGTH 732
#define BYTE_FOR_CRC 2
#define BYTE_FOR_STATUS 1
	
typedef struct{
	uint8_t *datastream;
	uint8_t *filtered_data;
	uint8_t *distances;
	uint8_t *rounded_distances;
	uint8_t *converted_distances;
} lidar_t;

typedef enum baudrate{
	BAUDRATE_SENSOR = 9600,
	BAUDRATE_SLAVE = 250000
}baudrate_t;

typedef	struct header{
		uint8_t receiverAck:8;
		uint8_t adresse:8;
		uint16_t length:16;
		uint8_t commandCode:8;
}header_t;

typedef union headerInst{
	uint8_t data[HEADER_LENGTH];
	header_t header1;
}headerInst_t;

typedef union{
	uint8_t data[TELG_LENGTH];
}responseTelg_t;

typedef union{
}commandTelg_t;


#endif /* MAIN_H_ */
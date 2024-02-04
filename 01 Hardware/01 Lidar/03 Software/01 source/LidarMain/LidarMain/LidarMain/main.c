/*
 * LidarMain.c
 *
 * Created: 12/15/2023 9:08:52 AM
 * Author : Thach
 * Version: 1.1
 * Revision: 1.0
 */ 

#include "main.h"

EMPTY_INTERRUPT(BADISR_vect);

int main(void){
	/**** Deklaration der Variablen ****/
	/* io-stream für Lidar-Handler, Data-Processing, USVData-Handler*/
	volatile reg8Model_t mainStream = {0};
	/* LidarHandler-Variablen */
	volatile uint8_t lidarOutput[LIDAR_RX_BUFFER_MAX_LEN] = {0};
	volatile uint16_t lidarOutputLen = sizeof(lidarOutput)/sizeof(uint8_t);
	volatile uint8_t lidarFSMState = LIDAR_MAIN_SYNC_STATE;
	volatile uint8_t lidarFlagLen = LIDAR_STATUS_REG_LEN_BYTE;
	const lidarStatus_t* lidarFlag_p = lidar_getStatus();
}


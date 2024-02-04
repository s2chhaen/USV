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
}


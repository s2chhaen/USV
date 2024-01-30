/*
 * lidarDataPrc.c
 *
 * Created: 12/1/2023 2:30:54 PM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.0
 */ 

#include "lidarDataPrc.h"

/*Lokalvariablen zum Speichern der Filter-Koeffizienten*/
volatile int32_t fil_coffs[FIL_ORDER+1] = {0};
/*Lokalvariablen zum Speichern der vorherigen Daten der Eingabe*/
volatile filOldValBuffer fil_oldVal = {0};
/*Lokalvariablen zum Speichern des Filter-Zustand*/
volatile filStatus_t fil_mgr = {0};
/*Lokalvariablen zum Speichern der zu filternden Daten*/
volatile int32_t fil_dataBuffer[DATA_BUFFER_LEN] = {0};
volatile uint16_t fil_dataBufferIdx = 0;
static uint16_t fil_dataBufferLen = 0;

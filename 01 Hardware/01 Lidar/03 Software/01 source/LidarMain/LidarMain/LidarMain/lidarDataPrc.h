/*
 * lidarDataPrc.h
 *
 * Created: 12/1/2023 2:30:09 PM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.0
 */ 


#ifndef LIDARFILTER_H_
#define LIDARFILTER_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "errorList.h"
#include "comConfig.h"
#include "registerModel.h"

/*Lokale Variable Param*/
#define FIL_ORDER 3U
#define FIL_OLD_VALUES_BUFFER_LEN_BITS ((FIL_ORDER-1U)/2U + 1U)
#define FIL_OLD_VALUES_BUFFER_LEN (1<<FIL_OLD_VALUES_BUFFER_LEN_BITS)
#define PHASE_SHIFT_SPL_MAX (FIL_ORDER/2 + 1)
#define DATA_SPL_NUM 361
#define DATA_BUFFER_LEN (DATA_SPL_NUM+PHASE_SHIFT_SPL_MAX)
/*Filter-Config*/
//Bits-Anzahl für Nachkommastelle
#define FIXED_POINT_BITS 15
//Umwandlungsfaktor vom "floating-point" zum "fixed-point" Format
#define CONVERT_FACTOR_INT ((1<<FIXED_POINT_BITS) - 1)
/*Lidar-Ausgabe-Param*/
#define LIDAR_OUTPUT_IDEAL_LEN 362
/*Definition der Max. und Min. Grenzwert UQ7.4 Format*/
//Max = 80m
#define MAX_MEASURED_VAL_FP_M 1280UL
//Min = 0,125m
#define MIN_MEASURED_VAL_FP_M 2UL

typedef struct{
	int32_t data[FIL_OLD_VALUES_BUFFER_LEN];
	uint8_t jmpIdx:FIL_OLD_VALUES_BUFFER_LEN_BITS;
	uint8_t currIdx:FIL_OLD_VALUES_BUFFER_LEN_BITS;
}filOldValBuffer;


#endif /* LIDARFILTER_H_ */
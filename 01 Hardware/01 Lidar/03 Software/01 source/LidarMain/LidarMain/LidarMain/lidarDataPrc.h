/*
 * lidarDataPrc.h
 * Beschreibung: Header-Datei für Datenverarbeitungseinheit
 * Created: 12/1/2023 2:30:09 PM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.1
 */ 


#ifndef LIDARFILTER_H_
#define LIDARFILTER_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "errorList.h"
#include "comConfig.h"
#include "registerModel.h"

/* Abkürzungen: FIL:Filter, SPL: sample, FP:Fixed-point, LEN:length,
 * idx: Index, Val: value, jmp: jump/jumping, curr:current,
 * N: And, usml: undersampling
 */
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

typedef struct{
	uint8_t init:1;//1: initialisiert, 0: nicht initialisiert
	uint8_t set:1;//1:Daten bereits, 0:keine Daten
}filStatus_t;

enum filUndersamplingType{
	FIL_EVEN_TYPE,
	FIL_ODD_TYPE
};
#define UNDERSAMPLING_LAST_TYPE FIL_ODD_TYPE
#define UNDERSAMPLING_TYPE_NUM (UNDERSAMPLING_LAST_TYPE+1)

extern uint8_t fil_init(const int16_t* inFilCofs_p, uint8_t inLen, reg8Model_t* io_p);
extern uint8_t fil_setNConvertData(uint8_t* data, uint16_t dataLen);
extern uint8_t fil_run();
extern uint8_t fil_compressNReturn(uint8_t* output_p, uint16_t outLen, uint8_t usmlType,\
uint8_t outFPBit);

#endif /* LIDARFILTER_H_ */
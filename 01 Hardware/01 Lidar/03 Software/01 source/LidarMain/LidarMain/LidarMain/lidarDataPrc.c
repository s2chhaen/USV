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

static inline int32_t fil_qFExtend(int32_t input, uint8_t bits){ //qF: Q-Format
	/*Umwandlung vom U(16-bits).(bits) zum U(32-bits).(bits)-Format*/
	const int32_t cFactorARM = (1<<bits) - 1;//cFactor: converting factor
	return (int32_t)((int64_t)input<<bits)/((int64_t)cFactorARM);
}

uint8_t fil_init(const int16_t* inFilCofs_p, uint8_t inLen, reg8Model_t* io_p){
	uint8_t result = NO_ERROR;
	uint8_t check = (inFilCofs_p != NULL) && (io_p != NULL) && (inLen == (FIL_ORDER+1));
	if (check){
		for (volatile int i = 0; i < inLen; i++){
			fil_coffs[i] = fil_qFExtend((int32_t)inFilCofs_p[i],FIXED_POINT_BITS);
		}
		fil_mgr.init = 1;
	} else{
		result = PROCESS_FAIL;
	}
	return result;
}

static inline void fil_setData(uint8_t* data, uint16_t dataLen){
	/*Umwandlung der Daten von 2 8-Bits-Zahlen in einer 16-Bits-Zahl*/
	for (volatile uint16_t i = 0; i < dataLen/2; i++){
		fil_dataBuffer[i] = (int64_t)(data[2*i]|(data[2*i+1] << 8)) & 0x1FFF;
	}	
}

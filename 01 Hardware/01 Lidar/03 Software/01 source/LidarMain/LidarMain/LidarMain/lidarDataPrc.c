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

static inline void fil_convertData(){
	//Umwandlung in Q(32-FIXED_POINT_BITS).(FIXED_POINT_BITS) Format
	for (int i = 0; i < DATA_SPL_NUM; i++){
		fil_dataBuffer[i] <<= FIXED_POINT_BITS;
	}
}

uint8_t fil_setNConvertData(uint8_t* data, uint16_t dataLen){
	/*Kopieren der Eingabe im internen Buffer und Umwandlung im passenden Format*/
	uint8_t result = NO_ERROR;
	uint16_t tempLen = dataLen/2;
	uint8_t check = (data!=NULL) && (dataLen > 0) && (tempLen <= DATA_SPL_NUM);
	if (check){
		fil_dataBufferLen = tempLen;
		fil_setData(data, 2*tempLen);
		fil_convertData();
		fil_mgr.set = 1;
	} else{
		fil_dataBufferLen = 0;
		result = PROCESS_FAIL;
	}
	return result;
}

static inline void redundacyAdd(int64_t tempVal){
	//Hinzufüge des letzten Wertes "PHASE_SHIFT_SPL_MAX"-Mal für spätere Phasen-Kompensation
	tempVal = fil_dataBuffer[fil_dataBufferLen-1];
	for (volatile uint8_t i = 0; i < PHASE_SHIFT_SPL_MAX; i++){
		fil_dataBuffer[fil_dataBufferLen+i] = tempVal;
	}
	fil_dataBufferLen += PHASE_SHIFT_SPL_MAX;
}

static inline void filtering(int64_t tempVal){
	/* Formel: y(n) = a0*x(n) + a1*x(n-1) + ... + aM*x(n-M)
	 * x(n<0) = 0, M: Ordnung des Filters
	 */
	for (volatile int i = 0; i < fil_dataBufferLen; i++){
		tempVal = ((int64_t)fil_coffs[0]) * ((int64_t)fil_dataBuffer[i]) >> FIXED_POINT_BITS;
		/*Aktualisierung der jmpIdx*/
		fil_oldVal.jmpIdx = fil_oldVal.currIdx;
		for (volatile uint8_t j = 1; j <= FIL_ORDER; j++){
			tempVal += ((int64_t)fil_coffs[j]) * ((int64_t)fil_oldVal.data[fil_oldVal.jmpIdx]) >> FIXED_POINT_BITS;
			fil_oldVal.jmpIdx--;
		}
		/*Kopieren des Wertes in old-Data-Buffer */
		fil_oldVal.currIdx++;
		fil_oldVal.data[fil_oldVal.currIdx] = fil_dataBuffer[i];
		//Kopieren des gefilterten Wertes an Daten-Buffer
		fil_dataBuffer[i] = tempVal;
	}
	fil_dataBufferLen -= PHASE_SHIFT_SPL_MAX;
}

static inline void shiftPhaseCompensation(){
	/*Anzahl vom "PHASE_SHIFT_SPL_MAX" der ersten Werte wird weggeworfen 
	 *für die Phasen-Verschiebung-Kompensation*/
	fil_dataBufferIdx = PHASE_SHIFT_SPL_MAX;
}

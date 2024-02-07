/*
 * lidarDataPrc.c
 * Beschreibung: Quellcode-Datei der lidarDataPrc.h
 * Created: 12/1/2023 2:30:54 PM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.1
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

/**
 * \brief Umwandlung vom U(16-bits).(bits) zum U(32-bits).(bits)-Format
 * 
 * \param input die umzuwandelnde Zahl
 * \param bits die Anzahl des Bits nach der Komma
 * 
 * \return int32_t die umgewandelte Zahl
 */
static inline int32_t fil_qFExtend(int32_t input, uint8_t bits){ //qF: Q-Format
	const int32_t cFactorARM = (1<<bits) - 1;//cFactor: converting factor
	return (int32_t)((int64_t)input<<bits)/((int64_t)cFactorARM);
}

/**
 * \brief Initalisierung der Datenverarbeitungseinheit
 * 
 * \param inFilCofs_p Der Zeiger zu den Koeffizienten von FIR-Filter
 * \param inLen Anzahl der FIR-Filter-Koeffizienten
 * \param io_p Register vom IO-Stream
 * 
 * \return uint8_t 0: erfolgreich, sonst nicht
 */
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

/**
 * \brief Umwandlung der Eingabe von 2 8-Bits-Zahlen in einer 16-Bits-Zahl
 * 
 * \param data der Zeiger zu den Daten
 * \param dataLen die Datenlänge
 * 
 * \return void
 */
static inline void fil_setData(uint8_t* data, uint16_t dataLen){
	for (volatile uint16_t i = 0; i < dataLen/2; i++){
		fil_dataBuffer[i] = (int64_t)(data[2*i]|(data[2*i+1] << 8)) & 0x1FFF;
	}	
}

/**
 * \brief Umwandlung in Q(32-FIXED_POINT_BITS).(FIXED_POINT_BITS) Format
 * 
 * 
 * \return void
 */
static inline void fil_convertData(){
	for (int i = 0; i < DATA_SPL_NUM; i++){
		fil_dataBuffer[i] <<= FIXED_POINT_BITS;
	}
}

/**
 * \brief Kopieren der Eingabe im internen Buffer und Umwandlung im passenden Format
 * 
 * \param data Der Zeiger zur Ausgabe
 * \param dataLen Länge der Ausgabe
 * 
 * \return uint8_t 0: erfolgreich, sonst Fehler
 */
uint8_t fil_setNConvertData(uint8_t* data, uint16_t dataLen){
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

/**
 * \brief Hinzufüge des letzten Wertes "PHASE_SHIFT_SPL_MAX"-Mal für spätere Phasen-Kompensation
 * 
 * \param tempVal der hinzugefügte Wert
 * 
 * \return void
 */
static inline void redundacyAdd(int64_t tempVal){
	tempVal = fil_dataBuffer[fil_dataBufferLen-1];
	for (volatile uint8_t i = 0; i < PHASE_SHIFT_SPL_MAX; i++){
		fil_dataBuffer[fil_dataBufferLen+i] = tempVal;
	}
	fil_dataBufferLen += PHASE_SHIFT_SPL_MAX;
}

/**
 * \brief Durchführung der Datenfilterung durch FIR-Filter
 * 
 * \param tempVal die temporäre Variablen zum Speichern des Wertes
 * 
 * \return void
 */
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

/**
 * \brief Durchführung der Kompensation der FIR-verursachten Phase
 * 
 * 
 * \return void
 */
static inline void shiftPhaseCompensation(){
	/*Anzahl vom "PHASE_SHIFT_SPL_MAX" der ersten Werte wird weggeworfen 
	 *für die Phasen-Verschiebung-Kompensation*/
	fil_dataBufferIdx = PHASE_SHIFT_SPL_MAX;
}

uint8_t fil_run(){
	uint8_t result = NO_ERROR;
	if (fil_mgr.init && fil_mgr.set){
		int64_t tempVal=0;
		redundacyAdd(tempVal);
		filtering(tempVal);
		shiftPhaseCompensation();
		fil_mgr.set = 0;
	} else{
		result = PROCESS_FAIL;
	}
	return result;
}

uint8_t fil_compressNReturn(uint8_t* output_p, uint16_t outLen, uint8_t usmlType, uint8_t outFPBit){
	uint8_t result = NO_ERROR;
	uint8_t check = (output_p != NULL) && (outLen == LIDAR_OUTPUT_IDEAL_LEN);
	usmlType = usmlType%UNDERSAMPLING_TYPE_NUM;
	if (check){
		/*Convertierung in m mit UQ7.4 -> Begrenzung -> Ausgabe*/
		//0° wird seperat behandelt
		volatile uint16_t temp = 0;
		temp = (uint16_t)((fil_dataBuffer[0]/100L) >> (FIXED_POINT_BITS - outFPBit));
		if (temp < MIN_MEASURED_VAL_FP_M){
			temp = MIN_MEASURED_VAL_FP_M;
		} else if(temp > MAX_MEASURED_VAL_FP_M){
			temp = MAX_MEASURED_VAL_FP_M;
		}
		output_p[0] = temp & 0xff;
		output_p[1] = (temp >> 8) & 0xff;
		//Für Messwerte in anderen Bereichen (entweder gerader oder ungerader Bereich)
		for (volatile uint16_t i = 1; i < LIDAR_OUTPUT_IDEAL_LEN/2; i++){
			temp = (uint16_t)((fil_dataBuffer[2*i-usmlType]/100L) >> (FIXED_POINT_BITS - outFPBit));
			if (temp < MIN_MEASURED_VAL_FP_M){
				temp = MIN_MEASURED_VAL_FP_M;
			} else if(temp > MAX_MEASURED_VAL_FP_M){
				temp = MAX_MEASURED_VAL_FP_M;
			}
			output_p[2*i] = temp & 0xff;
			output_p[2*i+1] = (temp >> 8) & 0xff;
		}
	} else{
		result = PROCESS_FAIL;
	}
	return result;
}
/*
 * lidarFilter.c
 *
 * Created: 12/1/2023 2:30:54 PM
 *  Author: Thach
 */ 

#include "lidarFilter.h"
#include "tmwtypes.h"

volatile int32_t fil_coffs[FIL_ORDER+1] = {0};
volatile filOldValBuffer fil_oldVal = {0};
volatile filStatus_t fil_mgr = {0};

volatile int32_t fil_dataBuffer[DATA_BUFFER_LEN] = {0};
volatile uint16_t fil_dataBufferIdx = 0;
volatile uint16_t fil_dataBufferLen = 0;

static inline int32_t fil_qFARM2TI(int32_t input, uint8_t bits){ //qF: Q-Format
	const int32_t cFactorARM = (1<<bits) -1;//cFactor: converting factor
	return (int32_t)((int64_t)input<<bits)/((int64_t)cFactorARM);
}

uint8_t fil_init(const int16_t* inFilCofs_p, uint8_t inLen){
	uint8_t result = NO_ERROR;
	if (inLen == (FIL_ORDER+1) && (FIXED_POINT_BITS < FIXED_POINT_MAX_BITS)){
		for (volatile int i = 0; i < inLen; i++){
			fil_coffs[i] = fil_qFARM2TI((int32_t)inFilCofs_p[i],FIXED_POINT_BITS);//TODO real, used after include
		}
		fil_mgr.init = 1;
	} else{
		result = PROCESS_FAIL;
	}
	return result;
}

static inline void fil_setData(uint8_t* data, uint16_t dataLen){
	for (volatile uint16_t i = 0; i < dataLen/2; i++){
		fil_dataBuffer[i] = (int64_t)(data[2*i]|(data[2*i+1] << 8)) & 0x1FFF;//TODO dokument here: get only 13 of 16 Bits
	}	
}

static inline void fil_convertData(){
	for (int i = 0; i < DATA_SPL_NUM; i++){
		fil_dataBuffer[i] <<= FIXED_POINT_BITS;
	}
}

uint8_t fil_setNConvertData(uint8_t* data, uint16_t* dataLen){
	uint8_t result = NO_ERROR;
	uint16_t tempLen = (*dataLen)/2;
	if ((data!=NULL) && (dataLen!=NULL) && (tempLen <= DATA_SPL_NUM)){
		fil_dataBufferLen = tempLen;
		fil_setData(data, 2*tempLen);
		fil_convertData();
		*dataLen = 0;
		fil_mgr.set = 1;
	} else{
		fil_dataBufferLen = 0;
		result = PROCESS_FAIL;
	}
	return result;
}

static inline void redundacyAdd(int64_t tempVal){
	//Make last value as redundant for phase-shift later
	tempVal = fil_dataBuffer[fil_dataBufferLen-1];
	for (volatile uint8_t i = 0; i < PHASE_SHIFT_SPL_MAX; i++){
		fil_dataBuffer[fil_dataBufferLen+i] = tempVal;
	}
	fil_dataBufferLen += PHASE_SHIFT_SPL_MAX;
}

static inline void filtering(int64_t tempVal){
	for (volatile int i = 0; i < fil_dataBufferLen; i++){
		tempVal = ((int64_t)fil_coffs[0]) * ((int64_t)fil_dataBuffer[i]) >> FIXED_POINT_BITS;
		fil_oldVal.jmpIdx = fil_oldVal.currIdx;
		for (volatile uint8_t j = 1; j <= FIL_ORDER; j++){
			tempVal += ((int64_t)fil_coffs[j]) * ((int64_t)fil_oldVal.data[fil_oldVal.jmpIdx]) >> FIXED_POINT_BITS;
			fil_oldVal.jmpIdx--;
		}
		//Update the old value into old-Data-Buffer,
		fil_oldVal.currIdx++;
		fil_oldVal.data[fil_oldVal.currIdx] = fil_dataBuffer[i];
		//Copy value back into buffer
		fil_dataBuffer[i] = tempVal;
	}
	fil_dataBufferLen -= PHASE_SHIFT_SPL_MAX;
}

static inline void shiftPhaseCompensation(){
	//TODO use index to copy later or use memmove
	fil_dataBufferIdx = PHASE_SHIFT_SPL_MAX;
	//memmove((void*)fil_dataBuffer,(void*)&fil_dataBuffer[PHASE_SHIFT_SPL_MAX],fil_dataBufferLen*sizeof(int32_t)/sizeof(uint8_t));
	//fil_dataBufferIdx = 0;
}

uint8_t fil_run(){
	uint8_t result = NO_ERROR;
	if (fil_mgr.init && fil_mgr.set){
		int64_t tempVal=0;
		redundacyAdd(tempVal);
		filtering(tempVal);
		shiftPhaseCompensation();
	} else{
		result = PROCESS_FAIL;
	}
	return result;
}

//FP:Fix-pointed
uint8_t fil_compressNReturn(uint8_t* output_p, uint16_t outLen, uint8_t outFPBit){
	uint8_t result = NO_ERROR;
	uint16_t tempVal = 0;
	if ((output_p!=NULL) && (outLen >= fil_dataBufferLen)){
		for (volatile int i = 0; i < fil_dataBufferLen; i++){
			//Umwandlung in m mit Q7.1
			tempVal = (uint16_t)((fil_dataBuffer[i+fil_dataBufferIdx]/100) >> (FIXED_POINT_BITS-1));
			//Auswahl mit der Kriterien im Projekt
			if (tempVal > 0xff){
				output_p[i] = 0;
			} else{
				output_p[i] = tempVal?tempVal:1;
			}
			
		}
	} else{
		result =PROCESS_FAIL;
	}
	return result;
}
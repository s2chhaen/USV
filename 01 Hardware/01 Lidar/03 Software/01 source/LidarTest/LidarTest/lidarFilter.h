/*
 * lidarFilter.h
 *
 * Created: 12/1/2023 2:30:09 PM
 *  Author: Thach
 */ 


#ifndef LIDARFILTER_H_
#define LIDARFILTER_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "errorList.h"

//FIL:Filter, SPL: sample
//Lokale Variable Param
#define FIL_ORDER 3U
#define FIL_OLD_VALUES_BUFFER_LEN_BITS ((FIL_ORDER-1U)/2U + 1U)
#define FIL_OLD_VALUES_BUFFER_LEN (1<<FIL_OLD_VALUES_BUFFER_LEN_BITS)
#define PHASE_SHIFT_SPL_MAX (FIL_ORDER/2 + 1)
#define DATA_SPL_NUM 361
#define DATA_BUFFER_LEN (DATA_SPL_NUM+PHASE_SHIFT_SPL_MAX)
//Filter-Config TODO add FIL_ prefix
#define FIXED_POINT_BITS 15
#define FIXED_POINT_MAX_BITS 20
#define CONVERT_FACTOR_INT ((1<<FIXED_POINT_BITS) - 1)

typedef struct{ //idx: Index, Val :value, jmp: jump/jumping, curr:current
	int32_t data[FIL_OLD_VALUES_BUFFER_LEN];
	uint8_t jmpIdx:FIL_OLD_VALUES_BUFFER_LEN_BITS;
	uint8_t currIdx:FIL_OLD_VALUES_BUFFER_LEN_BITS;
}filOldValBuffer;

typedef struct{
	uint8_t init:1;
	uint8_t set:1;//1 = Data is set, 0 = no Data is set TODO after measuring this, if this > t_firstStep then use FIFO, if not then no FIFO anymore
}filStatus_t;

extern uint8_t fil_init(const int16_t* inFilCofs_p, uint8_t inLen);//TODO real, used after include
//extern uint8_t fil_init();//TODO not real, delete after include
extern uint8_t fil_setNConvertData(uint8_t* data, uint16_t* dataLen);
extern uint8_t fil_run();
extern uint8_t fil_compressNReturn(uint8_t* output_p, uint16_t outLen, uint8_t outFPBit);//N: And

#endif /* LIDARFILTER_H_ */
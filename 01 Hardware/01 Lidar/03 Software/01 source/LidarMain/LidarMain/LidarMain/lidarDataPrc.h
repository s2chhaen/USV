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

#endif /* LIDARFILTER_H_ */
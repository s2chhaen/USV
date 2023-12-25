/*
 * errorList.h
 *
 * Created: 21.06.2023 13:12:06
 * Author: Thach
 * Version: 1.1
 * Revision: 1.0
 */ 


#ifndef ERROR_LIST_H_
#define ERROR_LIST_H_

typedef enum {
	NO_ERROR,
	PROCESS_FAIL,
	NULL_POINTER,
	FIFO_EMPTY,
	FIFO_FULL,
	DATA_INVALID,
	NO_INIT,
	TIME_OUT
}processResult_t;

#endif /* ERROR_LIST_H_ */
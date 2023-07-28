/*
 * errorList.h
 *
 * Created: 21.06.2023 13:12:06
 * Author: Thach
 * Version: 1.1
 */ 


#ifndef ERROR_LIST_H_
#define ERROR_LIST_H_

typedef enum {
	NO_ERROR,
	NULL_POINTER,
	FIFO_EMPTY,
	FIFO_FULL,
	DATA_INVALID,
	PROCESS_FAIL,
	NO_INIT,
	TIME_OUT
}processResult_t;

#endif /* ERROR_LIST_H_ */
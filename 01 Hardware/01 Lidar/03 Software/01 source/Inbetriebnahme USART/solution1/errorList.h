/*
 * errorList.h
 *
 * Created: 21.06.2023 13:12:06
 * Author: Thach
 */ 


#ifndef ERROR_LIST_H_
#define ERROR_LIST_H_

typedef enum {
	NO_ERROR,
	LENGTH_INVALID, 
	LENGTH_EXCESS, 
	NULL_POINTER,
	IN_OUT_NOT_EQUAL,
	PROCESS_FAIL,
	ALL_SLOT_FULL,
	FIFO_EMPTY
}processResult_t;

#endif /* ERROR_LIST_H_ */
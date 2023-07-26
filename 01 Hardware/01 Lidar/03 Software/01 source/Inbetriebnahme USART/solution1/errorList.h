/*
 * errorList.h: Die Liste der Prozessrückgabe-Code
 *
 * Created: 21.06.2023 13:12:06
 * Author: Thach
 * Version: 1.1
 * Revision: 1.1
 */ 


#ifndef ERRORLIST_H_
#define ERRORLIST_H_

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

#endif /* ERRORLIST_H_ */
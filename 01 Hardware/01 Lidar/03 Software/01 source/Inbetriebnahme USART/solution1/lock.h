/*
 * Mutex.h
 *
 * Created: 23.06.2023 07:46:17
 *  Author: Thach
 */ 


#ifndef MUTEX_H_
#define MUTEX_H_

#include <stdint.h>

typedef struct  {
	uint8_t id;
	uint8_t lockState:1;
}lock_t;

extern volatile lock_t usart1Lock;

#endif /* MUTEX_H_ */
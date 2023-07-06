/*
 * mutex.c
 *
 * Created: 23.06.2023 08:03:55
 *  Author: Dan
 */ 

#include "lock.h"

volatile lock_t usart1Lock = {.id=0, .lockState=0};
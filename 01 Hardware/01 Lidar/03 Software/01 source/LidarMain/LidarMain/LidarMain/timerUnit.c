/*
 * timerUnit.c
 *
 * Created: 6/29/2023 11:57:34 PM
 * Author: Thach
 * Version: 1.1
 * Revision: 1.0
 */ 

#include "timerUnit.h"


volatile timerStatus_t timer_status = {0};
static uint16_t lidarTimer_res = 0;
static uint16_t usvTimer_res = 0;
volatile int16_t lidarTimer_stepCounter = 0;
volatile int16_t usvTimer_stepCounter = 0;


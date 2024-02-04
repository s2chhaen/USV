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

uint8_t timer_init(uint8_t rezConfig, uint16_t resUSV, uint16_t resLidar){
	uint8_t result = NO_ERROR;
	volatile uint8_t config = 0x00;//keine Optimierung hier
	uint32_t prescalerWConvertFactor = 1;
	uint16_t temp = 0;
	//Ausschalten vor der Einstellung
	TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
	TCB0.CTRLA &= ~TCB_ENABLE_bm;
	TCB1.CTRLA &= ~TCB_ENABLE_bm;
	switch(rezConfig){
		case REZ_US:
			config |= TCA_SINGLE_CLKSEL_DIV16_gc;
			prescalerWConvertFactor = 16*CONVERT_FACTOR_S_2_US;
			resUSV = (resUSV < TIMER_MIN_REZ_US)?TIMER_MIN_REZ_US:resUSV;
			resLidar = (resLidar < TIMER_MIN_REZ_US)?TIMER_MIN_REZ_US:resLidar;
			break;
		case REZ_MS:
			config |= TCA_SINGLE_CLKSEL_DIV1024_gc;
			prescalerWConvertFactor = 1024UL*CONVERT_FACTOR_S_2_MS;
			resUSV = (resUSV > TIMER_MAX_REZ_MS)?TIMER_MAX_REZ_MS:resUSV;
			resLidar = (resLidar > TIMER_MAX_REZ_MS)?TIMER_MAX_REZ_MS:resLidar;
			break;
		case REZ_S:
			config |= TCA_SINGLE_CLKSEL_DIV1024_gc;
			prescalerWConvertFactor = 1024UL;
			resUSV = (resUSV > TIMER_MAX_REZ_S)?TIMER_MAX_REZ_S:resUSV;
			resLidar = (resLidar > TIMER_MAX_REZ_S)?TIMER_MAX_REZ_S:resLidar;
			break;
		default:
			result = PROCESS_FAIL;
			break;
	}
	
	if (result==NO_ERROR){
		timer_status.init = 1;
		//Gesamt-Clock-Quelle für Timer vom USVData und Lidar konfigurieren
		TCA0.SINGLE.CTRLA = config;
		//Timer für USV konfigurieren
		usvTimer_res = resUSV;
		temp = (uint16_t)(CLK_CPU*10/prescalerWConvertFactor*resUSV/10);
		TCB0.CCMP = temp;
		TCB0.CTRLA = TCB_CLKSEL_CLKTCA_gc;
		TCB0.CTRLB = TCB_CNTMODE_INT_gc;
		TCB0.INTCTRL = TCB_CAPT_bm;
		//Timer für Lidar konfigurieren
		lidarTimer_res = resLidar;
		temp = (uint16_t)(CLK_CPU*10/prescalerWConvertFactor*resLidar/10);
		TCB1.CCMP = temp;
		TCB1.CTRLA = TCB_CLKSEL_CLKTCA_gc;
		TCB1.CTRLB = TCB_CNTMODE_INT_gc;
		TCB1.INTCTRL = TCB_CAPT_bm;
	} else{
		timer_status.init = 0;
	}
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
	return result;
}

//Funktion zum Steuern des Timers für USVData
void usvTimer_setState(uint8_t state){
	if (timer_status.init){
		if (state){
			TCB0.CTRLA |= TCB_ENABLE_bm;
		} else{
			TCB0.CTRLA &= ~TCB_ENABLE_bm;
		}
	}
}

void usvTimer_setCounter(uint32_t value){
	usvTimer_stepCounter = (int16_t)(value/(uint32_t)usvTimer_res);
	TCB0.CNT = 0;
}

const int16_t usvTimer_getCounter(){
	return usvTimer_stepCounter;
}

//Funktion zum Steuern des Timers für Lidar
void lidarTimer_setState(uint8_t state){
	if (timer_status.init){
		if (state){
			TCB1.CTRLA |= TCB_ENABLE_bm;
		} else{
			TCB1.CTRLA &= ~TCB_ENABLE_bm;
		}
	}
}

void lidarTimer_setCounter(uint32_t value){
	lidarTimer_stepCounter = (int16_t)(value/(uint32_t)lidarTimer_res);
	TCB1.CNT = 0;
}

const int16_t lidarTimer_getCounter(){
	return lidarTimer_stepCounter;
}

//ISR
/**
 * \brief Interrupt-Service-Routine für Overflow-Interrupt von TCB0
 * \detailed beim Stopuhr: Nach einer Zeit von resolutionUs wird der Wert vom Counter dekrementiert
 *  bis zum 0.
 */
ISR(TCB0_INT_vect){
	usvTimer_stepCounter--;
	TCB0.INTFLAGS |= TCB_CAPT_bm;//Loeschen von Interrupt-Flag
}

/**
 * \brief Interrupt-Service-Routine für Overflow-Interrupt von TCB1
 * \detailed beim Stopuhr: Nach einer Zeit von resolutionUs wird der Wert vom Counter dekrementiert
 *  bis zum 0.
 */
ISR(TCB1_INT_vect){
	lidarTimer_stepCounter--;
	TCB1.INTFLAGS |= TCB_CAPT_bm;//Loeschen von Interrupt-Flag
}

/*
 * timerUnit.c
 *
 * Created: 6/29/2023 11:57:34 PM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.1
 */ 

#include "timerUnit.h"

volatile timerStatus_t timer_status = { .init = 0, .rez = REZ_MS};
volatile uint16_t timer_counter[REZ_MODE_NO] = {0};

uint8_t timerInit(uint8_t rezConfig){
	uint8_t result = NO_ERROR;
	uint8_t config = 0x00;
	uint32_t prescalerWConvertFactor = 1;
	TCA0.SINGLE.INTCTRL &= ~TCA_SINGLE_OVF_bm;//Vorlaeufig deaktiviert wird Overflow-Interrupt
	TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;//Ausschalten vor der Einstellung
	switch(rezConfig){
		case REZ_S:
			config |= TCA_SINGLE_CLKSEL_DIV1024_gc;
			prescalerWConvertFactor = 1024;
			break;
		case REZ_MS:
			config |= TCA_SINGLE_CLKSEL_DIV1_gc;
			prescalerWConvertFactor = CONVERT_FACTOR_S_2_MS;
			break;
		case REZ_US:
			config |= TCA_SINGLE_CLKSEL_DIV1_gc;
			prescalerWConvertFactor = CONVERT_FACTOR_S_2_US;
			break;
		default:
			result = PROCESS_FAIL;
			break;
	}
	
	if (result==NO_ERROR){
		timer_status.rez = rezConfig;
		timer_status.init = 1;
		TCA0.SINGLE.PER = (uint16_t)(CLK_CPU/prescalerWConvertFactor);
		TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;//Aktivieren des OVF - Interrupt
		TCA0.SINGLE.CTRLA = config;//Einschalten des TCA0 immer am Ende
	} else{
		timer_status.init = 0;
	}
	return result;
}

void timer_setState(uint8_t state){
	if (state){
		TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
		} else{
		TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
	}
}

void timer_setCounter(uint16_t value){
	ATOMIC_BLOCK(ATOMIC_FORCEON){
		timer_status.state = 1;
		timer_counter[timer_status.rez] = value;
		TCA0.SINGLE.CNT = 0;
	}
}

const uint16_t* timer_getCounter(){
	return (const uint16_t*)&(timer_counter[timer_status.rez]);
}

/**
 * \brief Verzögerung der Programmausführung in einem bestimmten Zeitraum
 * 
 * \param us die erwünschte Verzögerungszeit in Mikrosekunden
 * 
 * \return void
 */
extern void timer_stopWatch(uint16_t val){
	uint8_t mode = timer_status.rez;
	ATOMIC_BLOCK(ATOMIC_FORCEON){
		timer_status.state = 1;
		timer_counter[mode] = val;
		TCA0.SINGLE.CNT = 0;
	}
	while (timer_counter[mode]);
}

/**
 * \brief Verzögerung der Programmausführung in einem bestimmten Zyklen
 * \warning noch zu verbessern, weil die Zyklen nicht echtzeitig
 *
 * \param cycle die erwünschte Verzögerungszeit in Zyklen
 * 
 * \return void
 */
void waitCycle(uint32_t cycle){
	for (uint32_t i = 0; i<cycle*5;i++);
}

/**
 * \brief Interrupt-Service-Routine für Overflow-Interrupt von TCA0
 * \detailed beim Stopuhr: Nach einer Zeit von resolutionUs wird der Wert vom Counter dekrementiert
 *  bis zum 0.
 */
ISR(TCA0_OVF_vect){
	if (timer_status.state){
		uint8_t temp = timer_status.rez;
		timer_counter[temp]--;
		if (timer_counter[temp]){
			timer_status.state = 0;
		}
	}
	TCA0.SINGLE.INTFLAGS |=  TCA_SINGLE_OVF_bm;//Loeschen von Interrupt-Flag
}


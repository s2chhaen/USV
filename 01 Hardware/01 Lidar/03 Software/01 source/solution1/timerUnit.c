/*
 * timerUnit.c
 *
 * Created: 6/29/2023 11:57:34 PM
 * Author: Thach
 */ 

#include "timerUnit.h"

volatile timer_t objTCA ={
	.adr=&(TCA0),
	.resolutionUs=3
};
volatile tickGenerator counter[NO_OF_SUBTIMER];
volatile uint32_t usartWatcher[NO_OF_USART] = {0}; 

static void resetAllGenerator(){
	for (uint8_t i = 0; i<NO_OF_SUBTIMER;i++)
	{
		counter[i].value = 0;
		counter[i].lock = 0;
	}
	//TODO reset Interrupt Flag, set value in counter register equal 0
}

static int8_t searchFreeGenerator(){
	int result = -1;
	for (uint8_t i = 0;i<NO_OF_SUBTIMER;i++){
		if(!counter[i].lock){
			result = i;
			break;
		}
	}
	return result;
}

static inline void unlockGenerator(uint8_t i){
	counter[i].lock = 0;
}

void timerInit(uint8_t resolutionUs, uint16_t prescaler){
	objTCA.adr->SINGLE.INTCTRL &= ~(1<<0);//Vorlaeufig deaktiviert wird Overflow-Interrupt
	resetAllGenerator();
	for (uint8_t i = 0;i<4;i++){
		usartWatcher[i] = 0;
	}
	TCA0_CTRLA_t configCTRLA;//prescaler = 4, enable timer
	switch(prescaler){
		case 1:
			configCTRLA.valueBitField.CLKSEL = DIV1;
			break;
		case 2:
			configCTRLA.valueBitField.CLKSEL = DIV2;
			break;
		case 4:
			configCTRLA.valueBitField.CLKSEL = DIV4;
			break;
		case 8:
			configCTRLA.valueBitField.CLKSEL = DIV8;
			break;
		case 16:
			configCTRLA.valueBitField.CLKSEL = DIV16;
			break;
		case 64:
			configCTRLA.valueBitField.CLKSEL = DIV64;
			break;
		case 256:
			configCTRLA.valueBitField.CLKSEL = DIV256;
			break;
		case 1024:
			configCTRLA.valueBitField.CLKSEL = DIV1024;
			break;
		default:
			configCTRLA.valueBitField.CLKSEL = DIV1;
			break;
	}
	configCTRLA.valueBitField.ENABLE = 1;
	TCA0_INTCTRL_t configINTCTRL = {.valueBitField.OVF=1};
	objTCA.resolutionUs = resolutionUs;
	//Berechnung des Wertes fuer PER
	objTCA.adr->SINGLE.PER = (uint16_t)(0xff/(CLK_CPU/10000000UL/prescaler*resolutionUs));
	objTCA.initStatus = 1;
	//immer am Ende
	objTCA.adr->SINGLE.CTRLA = configCTRLA.value;
	objTCA.adr->SINGLE.INTCTRL = configINTCTRL.value;//Overflow-Interrupt wird wieder aktiviert 
}

void setUsartWatcherTimeout(uint8_t usartNr, uint32_t us){
	usartNr%=4;
	objTCA.adr->SINGLE.INTCTRL &= ~(1<<0);//Vorlaeufig deaktiviert wird Overflow-Interrupt
	usartWatcher[usartNr] = us/objTCA.resolutionUs + (us%objTCA.resolutionUs)?1:0;
	objTCA.adr->SINGLE.INTCTRL |= (1<<0);//Overflow-Interrupt wird wieder aktiviert 
}

uint8_t waitUs(uint32_t us){
	uint8_t result = NO_ERROR;
	int8_t i = 0;
	i = searchFreeGenerator();//suchen die freie Stelle
	//Falls gefunden, macht weiter sonst gibt Fehler zurueck
	if (i!=-1){
		objTCA.adr->SINGLE.INTCTRL &= ~(1<<0);//Vorlaeufig deaktiviert wird Overflow-Interrupt
		counter[i].lock = 1;
		counter[i].value = us/objTCA.resolutionUs + (us%objTCA.resolutionUs)?1:0;
		objTCA.adr->SINGLE.INTCTRL |= (1<<0);//Overflow-Interrupt wird wieder aktiviert 
		while (counter[i].value);
	} else{
		result = ALL_SLOT_FULL;
	}
	return result;
}

void waitCycle(uint32_t cycle){
	for (uint32_t i = 0; i<cycle;i++);
}

ISR(TCA0_OVF_vect){
	uint8_t loopMax = MAX(NO_OF_SUBTIMER,NO_OF_USART);
	for (int i = 0; i<loopMax;i++){
		if (i<(NO_OF_SUBTIMER-1)){
			if (counter[i].lock){
				counter[i].value--;
			}
			if (!counter[i].value){
				unlockGenerator(i);
			}
		}
		if (i<(NO_OF_USART-1)){
			if (usartWatcher[i]){
				usartWatcher[i]--;
			}	
		}
	}
	//objTCA.adr->SINGLE.INTFLAGS &= ~(1<<1);//Loeschen von Interrupt-Flag
}


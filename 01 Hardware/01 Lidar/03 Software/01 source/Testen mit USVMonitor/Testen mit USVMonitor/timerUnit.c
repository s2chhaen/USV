/*
 * timerUnit.c
 *
 * Created: 6/29/2023 11:57:34 PM
 * Author: Thach
 */ 

#include "timerUnit.h"

volatile timer_t objTCA ={
	.adr=&(TCA0),
	.resolutionUs=1
};

volatile tickGenerator counter[NO_OF_SUBTIMER];

static void resetAllGenerator(){
	for (uint8_t i = 0; i<NO_OF_SUBTIMER;i++)
	{
		counter[i].value = 0;
		counter[i].lock = 0;
	}
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
	if (prescaler==0)
	{
		prescaler = 1;
	} else if ((prescaler>2)&&(prescaler<4))
	{
		prescaler = 4;
	} else if ((prescaler>4)&&(prescaler<8))
	{
		prescaler = 8;
	} else if ((prescaler>8)&&(prescaler<16))
	{
		prescaler = 16;
	} else if ((prescaler>16)&&(prescaler<64))
	{
		prescaler = 64;
	} else if ((prescaler>64)&&(prescaler<256))
	{
		prescaler = 256;
	} else if ((prescaler>256)&&(prescaler<1024))
	{
		prescaler = 1024;
	} else{
		prescaler = 1024;
	}
	TCA0_CTRLA_t configCTRLA;
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
	uint16_t value = (uint16_t)((CLK_CPU/prescaler)*(resolutionUs));
	uint16_t compensation = (CLK_CPU%prescaler)?1:0;
	objTCA.adr->SINGLE.PER = (value+compensation);
	objTCA.initStatus = 1;
	//immer am Ende
	objTCA.adr->SINGLE.CTRLA = configCTRLA.value;
	objTCA.adr->SINGLE.INTCTRL = configINTCTRL.value;//Overflow-Interrupt wird wieder aktiviert 
}

void waitUs(uint32_t us){
	//uint8_t result = NO_ERROR;
	int8_t i = 0;
	i = searchFreeGenerator();//suchen die freie Stopuhr
	//Falls gefunden, macht weiter sonst gibt Fehler zurueck
	if (i!=-1){
		objTCA.adr->SINGLE.INTCTRL &= ~(1<<0);//Vorlaeufig deaktiviert wird Overflow-Interrupt
		counter[i].lock = 1;
		counter[i].value = us/objTCA.resolutionUs + (us%objTCA.resolutionUs)?1:0;
		objTCA.adr->SINGLE.INTCTRL |= (1<<0);//Overflow-Interrupt wird wieder aktiviert 
		while (counter[i].value);
	} else{
		//result = FIFO_FULL;
	}
	//return result;
}

void waitCycle(uint32_t cycle){
	for (uint32_t i = 0; i<cycle*5;i++);
}

ISR(TCA0_OVF_vect){
	uint8_t loopMax = NO_OF_SUBTIMER;
	for (int i = 0; i<loopMax;i++){
		if (counter[i].lock){
				counter[i].value--;
		}
		if (!counter[i].value){
			unlockGenerator(i);
		}
	}
	objTCA.adr->SINGLE.INTFLAGS |=  TCA_SINGLE_OVF_bm;//Loeschen von Interrupt-Flag
}


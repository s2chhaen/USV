/*
 * timerUnit.c
 *
 * Created: 6/29/2023 11:57:34 PM
 *  Author: Thach
 */ 

#include "timerUnit.h"

volatile timer_t objTCA ={.adr=&(TCA0)};
volatile tickGenerator counter[NO_OF_STOPTIMER];

static void resetAllGenerator(){
	for (uint8_t i = 0; i<NO_OF_STOPTIMER;i++)
	{
		counter[i].value = 0;
		counter[i].lock = 0;
	}
	//TODO reset Interrupt Flag, set value in counter register equal 0
}

void timerInit(uint8_t minValue){
	resetAllGenerator();
}

void wait(uint8_t us){
	uint8_t i = 0;
	//suchen die freie Stelle
	for (uint8_t i = 0;i<NO_OF_STOPTIMER+1;i++){
		if (i<=NO_OF_STOPTIMER)
		{
			if(counter[i].lock){
				break;
			}
		}
		
	}
	if (i<=NO_OF_STOPTIMER){
		//TODO Load code in generator
	} else{
		//TODO Return Error
	}
}
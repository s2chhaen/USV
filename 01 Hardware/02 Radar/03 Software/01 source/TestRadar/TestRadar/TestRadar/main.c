/*
 * TestRadar.c
 *
 * Created: 12/4/2023 2:51:26 PM
 * Author : Thach
 */ 

#include "main.h"

#define D1_INIT PORTD.DIR |= (1<<0)
#define D1_ON PORTD.OUT |= (1<<0)
#define D1_OFF PORTD.OUT &= ~(1<<0)
#define D1_TOOGLE PORTD.OUT ^= (1<<0)

int main(void){
	uint8_t temp = 0;
	__asm__("nop");
	/* Deklaration der Variablen */
	volatile reg8Model_t mainStream = {0};
	/* Radar-Handler Variablen*/
	volatile int8_t radarVel = 0;
	volatile uint16_t radarDis = 0;
	uint8_t radarFsmState = RADAR_MAIN_FSM_SYNC_STATE;
	/*Temp-Var*/
	usartConfig_t config = {
		.usartNo = iUSART0,
		.baudrate = 19200,
		.usartChSize = USART_CHSIZE_8BIT_gc,
		.parity = USART_PMODE_DISABLED_gc,
		.stopbit = USART_SBMODE_1BIT_gc,
		.sync = false,
		.mpcm = false,
		.address = 0,
		.portMux = PORTMUX_USARTx_DEFAULT_gc,
	};
	/*Initialisierung der Module*/
	//CPU-CLK
	init_Core_CLK(INTERN_CLK,1);
	//IO-Init
	D1_INIT;
	//Radar-Handler
	radar_initDev((const usartConfig_t*)&config,(int8_t*)&radarVel,(uint16_t*)&radarDis,(reg8Model_t*)&mainStream);
	//Timer
	timerInit(REZ_MS,10);
	sei();
	
	radarFsmState = RADAR_MAIN_FSM_DATA_REQ_STATE;
	radarFsmState = RADAR_MAIN_FSM_RESET_STATE;
    /* Replace with your application code */
    while (1){
		radarFsmState = radar_mainFsmLookupTable[radarFsmState]();
		temp++;
    }
}


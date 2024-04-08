////////////////////////////////////////////////////////////////////////////////////////////////////
// ATMegaXX09Clock.c
//
// Created: 15.11.2019 15:28:34
//  Author: franke
// Update: 27.04.2022 13:27:55
//  Create Timeout for the external 32kHz Crystal
//////////////////////////////////////////////////////////////////////////////////////////////////// 

#include <avr/io.h>
#include ".\ATMegaXX09Clock.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// globale Variable für die CPU Taktrate
////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t CLK_CPU = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////
// lokale Definitionen
////////////////////////////////////////////////////////////////////////////////////////////////////
#define CLK_Prescaler_Table {2,4,8,16,32,64,0,0,6,10,12,24,48}
#define Main_CLK_Switch_Table {20000000UL,32000UL,32768UL,EXTCLK}
	
////////////////////////////////////////////////////////////////////////////////////////////////////
// Deklaration für lokale Funktionen
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// get Core Clock and write in the "CLK_CPU" variable
////////////////////////////////////////////////////////////////////////////////////////////////////
void get_CPU_CLK(void)
{
	uint8_t Prescaler_tab[] = CLK_Prescaler_Table;
	uint32_t Main_CLK_Switch_tab[] = Main_CLK_Switch_Table;
	uint8_t Prescaler = 0;
	
	uint32_t Main_CLK_Switch = Main_CLK_Switch_tab[(CLKCTRL.MCLKCTRLA & CLKCTRL_CLKSEL_gm)];
	
	
	if((CLKCTRL.MCLKCTRLB & CLKCTRL_PEN_bm) == CLKCTRL_PEN_bm)
	{
		Prescaler = Prescaler_tab[((CLKCTRL.MCLKCTRLB & CLKCTRL_PDIV_gm)>>1)];
		CLK_CPU = Main_CLK_Switch/Prescaler;
	}
	else
	{
		CLK_CPU = Main_CLK_Switch;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// init Core Clock with 20 MHz and start the external 32,768 Hz Crystal Oscillator
// return value: 0 - external Crystal Oscillator initialisation valid
//				 1 - external Crystal Oscillator initialisation fail
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma GCC push_options
#pragma GCC optimize("O3")
uint8_t init_Core_CLK(void)
{
	register uint8_t reg_val = 0;
	register uint8_t timeout = 255;
		
	// internen 20MHz Oszillator auswählen
	if((CLKCTRL.MCLKSTATUS & CLKCTRL_OSC20MS_bm) == CLKCTRL_OSC20MS_bm)
	{
		CLKCTRL.MCLKCTRLA = CLKCTRL_CLKSEL_OSC20M_gc;
		CLKCTRL.MCLKCTRLA &= ~CLKCTRL_CLKOUT_bm;
	}
	
	// Kein Prescaler von CLK
	reg_val = CLKCTRL.MCLKCTRLB;
	reg_val &= ~CLKCTRL_PEN_bm;
	CPU_CCP = CCP_IOREG_gc;
	CLKCTRL.MCLKCTRLB = reg_val;
	
	get_CPU_CLK();
	
	
	// externen 32.768 kHz Crystal Oscillator auswählen für RTC
	reg_val = CLKCTRL.XOSC32KCTRLA;
	reg_val = (reg_val & ~CLKCTRL_CSUT_gm) | CLKCTRL_CSUT_16K_gc;
	reg_val |= CLKCTRL_RUNSTDBY_bm;
	reg_val |= CLKCTRL_ENABLE_bm;
	CPU_CCP = CCP_IOREG_gc;
	CLKCTRL.XOSC32KCTRLA = reg_val;
	
	for(timeout = 255;
	((CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm) != CLKCTRL_XOSC32KS_bm)&&(timeout != 0);
	timeout--);
	if(timeout > 0)
		return 0;
	else
		return 1;
	//while((CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm) != CLKCTRL_XOSC32KS_bm)
}
#pragma GCC pop_options
//EOF//

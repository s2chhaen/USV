////////////////////////////////////////////////////////////////////////////////////////////////////
// ATMegaXX09Clock.c
//
// Created: 15.11.2019 15:28:34
//  Author: franke
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
// init Core Clock with 20Mhz and start the external 32,768 Crystal Oscillator
////////////////////////////////////////////////////////////////////////////////////////////////////
void init_Core_CLK(void)
{
	register uint8_t reg_val = 0;
	
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
	
	while((CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm) != CLKCTRL_XOSC32KS_bm);
}

//EOF//

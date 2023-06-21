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
void init_Core_CLK(uint8_t type, uint8_t prescaler)
{
	register uint8_t reg_val = 0;
	//Nur eine Art von Taktgeber darf gleichzeitig ausgewaehaelt werden
	switch (prescaler){
	case 1:
		prescaler = ~CLKCTRL_PEN_bm;
		break;
	case 2:
		prescaler = (CLKCTRL_PEN_bm)|(CLKCTRL_PDIV_2X_gc);
		break;
	case 4:
		prescaler = (CLKCTRL_PEN_bm)|(CLKCTRL_PDIV_4X_gc);
		break;
	case 6:
		prescaler = (CLKCTRL_PEN_bm)|(CLKCTRL_PDIV_6X_gc);
		break;
	case 8:
		prescaler = (CLKCTRL_PEN_bm)|(CLKCTRL_PDIV_8X_gc);
		break;
	case 10:
		prescaler = (CLKCTRL_PEN_bm)|(CLKCTRL_PDIV_10X_gc);
		break;
	case 12:
		prescaler = (CLKCTRL_PEN_bm)|(CLKCTRL_PDIV_12X_gc);
		break;
	case 16:
		prescaler = (CLKCTRL_PEN_bm)|(CLKCTRL_PDIV_16X_gc);
		break;
	case 24:
		prescaler = (CLKCTRL_PEN_bm)|(CLKCTRL_PDIV_24X_gc);
		break;
	case 32:
		prescaler = (CLKCTRL_PEN_bm)|(CLKCTRL_PDIV_32X_gc);
		break;
	case 48:
		prescaler = (CLKCTRL_PEN_bm)|(CLKCTRL_PDIV_48X_gc);
		break;
	case 64:
		prescaler = (CLKCTRL_PEN_bm)|(CLKCTRL_PDIV_64X_gc);
		break;
	default:
		prescaler = ~CLKCTRL_PEN_bm;
		break;
	}
	switch(type){
		case EXTCLK:
			// externen 32.768 kHz Crystal Oscillator auswählen für RTC
			reg_val = CLKCTRL.XOSC32KCTRLA;
			reg_val = (reg_val & ~CLKCTRL_CSUT_gm) | CLKCTRL_CSUT_16K_gc;
			reg_val |= CLKCTRL_RUNSTDBY_bm;
			reg_val |= CLKCTRL_ENABLE_bm;
			CPU_CCP = CCP_IOREG_gc;
			CLKCTRL.XOSC32KCTRLA = reg_val;
			get_CPU_CLK();
			while((CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm) != CLKCTRL_XOSC32KS_bm);
			break;
		case INTERN_CLK:
			//16/20MHz Internal oscillator
			reg_val = CLKCTRL.MCLKCTRLA;
			reg_val |= CLKCTRL_CLKSEL_OSC20M_gc;
			reg_val &= ~CLKCTRL_CLKOUT_bm;
			CCP=CCP_IOREG_gc;
			CLKCTRL.MCLKCTRLA = reg_val;
			
			//Prescaler disable
			reg_val = prescaler;
			CCP=CCP_IOREG_gc;
			CLKCTRL.MCLKCTRLB = reg_val;
			//unlock
			CCP=CCP_IOREG_gc;
			CLKCTRL.MCLKLOCK &= ~CLKCTRL_LOCKEN_bm;
			get_CPU_CLK();
			while(CLKCTRL.MCLKSTATUS !=(CLKCTRL_OSC20MS_bm &(~CLKCTRL_EXTS_bm)&(~CLKCTRL_XOSC32KS_bm)&(~CLKCTRL_SOSC_bm)));
			break;
		default:
			break;
	}	
}

//EOF//

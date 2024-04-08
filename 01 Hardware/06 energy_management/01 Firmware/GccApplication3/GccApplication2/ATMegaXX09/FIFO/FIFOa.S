/////////////////////////////////////////////////////////////////////////////////////////
//
//
// FIFOa.S
//
// Created: 20.07.2023 20:58:33
//  Author: Franke
//
//
// rev 1.1 runtime optimisation; 2023.07.20
//
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef FIFOA_S_
#define FIFOA_S_

#include "FIFO.h"

/////////////////////////////////////////////////////////////////////////////////////////
// assembler defines
/////////////////////////////////////////////////////////////////////////////////////////
#define I1L r24
#define I1H r25
#define I2L r22
#define I2H r23
#define I3L r20
#define I3H r21
#define O1L r24
#define O1H r25

#define _ZR r1
#define  XL r26
#define  XH r27
#define  YL r28
#define  YH r29
#define  ZL r30
#define  ZH r31

 .section .text

 .global FIFO_writebyte
 /////////////////////////////////////////////////////////////////////////////////////////
 // bool FIFO_writebyte(sFIFO_t* FIFO, uint8_t byte) __attribute__((used))
 //	bool (R24) FIFO_writebyte(sFIFO_t* (R25,R24), uint8_t (R22))
 // {
 /////////////////////////////////////////////////////////////////////////////////////////
 FIFO_writebyte:
 MOVW  ZL, I1L
 LDD  r24, z+_p_read
 LDD  r25, z+_p_write
 INC  r25
 ANDI r25, _FIFO_bits
 CP   r24, r25
 LDI  O1L, 0
BREQ FFWB_end
 LDD  r24, Z+_p_write
 MOVW  XL,  ZL
 ADD   XL, r24
 ADC   XH, _ZR
#if _p_data!=0
 ADIW  XL, _p_data
#endif
 ST   X,   r22
 STD  Z+_p_write, r25
 LDI  O1L, 1
FFWB_end:
 ret

 /////////////////////////////////////////////////////////////////////////////////////////
 // }
 /////////////////////////////////////////////////////////////////////////////////////////

  .global FIFO_readbyte
 /////////////////////////////////////////////////////////////////////////////////////////
 // bool FIFO_readbyte(sFIFO_t* FIFO, uint8_t* byte) __attribute__((used))
 //	bool (R24) FIFO_readbyte(sFIFO_t* (R25,R24), uint8_t* (R23,R22))
 // {
 /////////////////////////////////////////////////////////////////////////////////////////
 FIFO_readbyte:
 MOVW  ZL, I1L
 LDD  r25, z+_p_read
 LDD  r24, z+_p_write
 CP   r24, r25
 LDI  O1L, 0
BREQ FFRB_end
 MOVW  XL, ZL
 ADD   XL, r25
 ADC   XH, _ZR
#if _p_data!=0
 ADIW  XL, _p_data
#endif
 LD   r24, x
 MOVW  XL, r22
 ST     x, r24
 INC  r25
 ANDI r25, _FIFO_bits
 STD  z+_p_read, r25
 LDI  O1L, 1
FFRB_end:
 ret
 /////////////////////////////////////////////////////////////////////////////////////////
 // }
 /////////////////////////////////////////////////////////////////////////////////////////

 .global FIFO_copy_to_lin
 /////////////////////////////////////////////////////////////////////////////////////////
 // uint8_t FIFO_copy_to_linA(uint8_t* lin_space, sFIFO_t* FIFO, uint8_t lenght) __attribute__((used))
 //	uint8_t (R24)					FIFO_copy_to_linA(uint8_t*,sFIFO_t*,       uint8_t)
 //	R01 == 0				  R31,R30 R29,R28 R27,R26 R25,R24 R23,R22          R21,R20
 /////////////////////////////////////////////////////////////////////////////////////////
 FIFO_copy_to_lin:			//		   |   |		   |   |   |   |				|
 PUSH  R28					//		   |   |>ST		   |   |   |   |				|
 PUSH  R29					//		   |>ST			   |   |   |   |				|
 MOVW  ZL, I2L				// |<Z-|<----------|---|---|---|---|___|				|
 MOVW  XL, I1L				// |   |		   |<X-|<--|___|	     				|
 // diff read Data			// |   |		   |   |		       _				|
 LDD  R22, Z+_p_write		// |   |		   |   |		     Z>|				|
 SUBI R22, (~(_FIFO_bits))	// |   |		   |   |		   _   |-K				|
 LDD  R23, Z+_p_read		// |   |		   |   |		 Z>|   |				|
 SUB  R22, R23				// |   |		   |   |		   |->-|				|
 ANDI R22, _FIFO_bits		// |   |		   |   |		   |   |^K				|
 // Berechne die min.		// |   |		   |   |		   |   |				|
 // Kopierlänge				// |   |		   |   |		   |   |				|
 CP   R22, R20				// |   |		   |   |		   |   |				|
BRGE .+2					// |   |		   |   |		   |   |				|
 MOV  R20, R22				// |   |		   |   |		   |   |--------------?>|
 //   R20 copy_length		// |   |		   |   |		   |					|
 // Berechne die Länge 		// |   |		   |   |		   |     				|
 // für die erste Kopier-	// |   |		   |   |		   |     				|
 // anweisungen				// |   |		   |   |		   |   _ 				|
 LDD  R22, Z+_p_FIFO_max	// |   |		   |   |		   | Z>| 				|
 SUB  R22, R23				// |   |		   |   |		   |->-| 				|
 CP	  R22, R20				// |   |		   |   |		   |   |				|
BRLT  .+2					// |   |		   |   |		   |   |				|
 MOV  R22, R20				// |   |		   |   |		   |   |<?--------------|
 // R22 read_length_block	// |   |		   |   |	   _   |   |                |
 LDI  O1L, 0				// |   |		   |   |	   |=0 |   |                |
 TST  R22					// |   |		   |   |	   |   |   |                |
BREQ  FFCTL_end				// |   |   _   _   |   |	   |># |   |				|
 MOVW  YL,  ZL				// |---|-->|-Y>|   |   |		   |   |				|
#if _p_data!=0				// |   |   |   |   |   |		   |   |				|
 ADIW  YL, _p_data			// |   |   |<--|+K |   |		   |   |				|
#endif						// |   |   |   |   |   |		   |   |				|
 ADD   YL, R23				// |   |   |  C|+<-|---|-----------|   |				|
 ADC   YH, R01				// |   |   |+C |+0 |   |   _	   |   |				|
 MOV  R25, R22				// |   |   |   |   |   |   |<------|---|				|
 // R25 Schleifenzähler		// |   |   |   |   |   |   |	   |   |				|
 // Kopierschleife 1		// |   |   |   |   |   |   |	   |   |				|
FFCTL_c1:					// |   |   |   |   |   |   |<$ _   |   |				|
 LD   R24, Y+				// |   |   |<--|+1 |   |   | Y>|   |   |				|
 ST   X+,  R24				// |   |   |   |   |<--|+1 |   |>X |   |				|
 DEC  R25					// |   |   |   |   |   |   |-1	   |   |				|
BRNE  FFCTL_c1				// |   |   |   |   |   |   |>$	   |   |				|
 // Berechne die Länge für	// |   |		   |   |		   |   |				|
 // die zweite Kopier-		// |   |		   |   |		   |   |				|
 // anweisungen				// |   |		   |   |   _	   |   |				|
 MOV  R25, R20				// |   |		   |   |   |<------|---|----------------|
 SUB  R25, R22				// |   |		   |   |   |-<-----|---|				|
 // R25 Schleifenzähler		// |   |		   |   |   |	   |					|
BREQ FFCTL_cend				// |   |		   |   |   |	   |					|
 MOVW  YL,  ZL				// |---|-->|-Y>|   |   |   |	   |					|
#if _p_data!=0				// |   |   |   |   |   |		   |					|
 ADIW  YL, _p_data			// |   |   |<--|+K |   |   |	   |					|
#endif						// |   |   |   |   |   |		   |					|
 // Kopierschleife 2		// |   |   |   |   |   |   |	   |					|
FFCTL_c2:					// |   |   |   |   |   |   |<§ _   |					|
 LD   R24, Y+				// |   |   |<--|+1 |   |   | Y>|   |					|
 ST   X+,  R24				// |   |   |   |   |<--|+1 |   |>X |					|
 DEC  R25					// |   |   |   |   |   |   |-1	   |					|
BRNE  FFCTL_c2				// |   |   |   |   |   |   |>§	   |					|
 // Berechne neuen Lese-	// |   |						   |					|
 // zeiger					// |   |						   |					|
FFCTL_cend:					// |   |						   |					|
 ADD  R23, R20				// |   |						   |+<------------------|
 ANDI R23, _FIFO_bits		// |   |						   |^K					|
 STD  Z+_p_read, R23 		// |   |					       |>Z					|
 MOV  O1L, R20				//							   |<-----------------------|
FFCTL_end:					//							   |<#
 POP  R29					//							   |
 POP  R28					//							   |
 ret						//							   |
 /////////////////////////////////////////////////////////////////////////////////////////
 // }
 /////////////////////////////////////////////////////////////////////////////////////////
#endif

// EOF //

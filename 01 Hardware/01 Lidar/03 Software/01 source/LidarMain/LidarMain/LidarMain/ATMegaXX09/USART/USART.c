////////////////////////////////////////////////////////////////////////////////////////////////////
//
// USART.c
//
// Created: 04.09.2020 16:33:50
//  Author: franke
//
//
//		-Implementierung als UART-Treiber
//
//		rev. 0.1
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "USART.h"
#include "USART_config.h"
#include "../../Math/MinMax.h"
#include "../ATMegaXX09Clock.h"
#include "../Port/Port.h"
#include "../EEPROM/EEPROM.h"
#include "../FIFO/FIFO.h"

#if USART_USE_EVENTSYSTEM==1
#include USART_EVENTSYSTEM_HEADER
#endif

#include <stddef.h>
#include <stdlib.h>
#include <avr/interrupt.h>


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// USART lokale Definitionen
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined USART3
	#define COUNT_USART 4
#elif defined USART2
	#define COUNT_USART 3
#elif defined USART1
	#define COUNT_USART 2
#elif defined USART0
	#define COUNT_USART 1
#else
	#define COUNT_USART 0
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// lokale Makros
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#define PASTE2( a, b) a##b
#define PASTE3( a, b ,c) a##b##c
#define ISR_vect( obj, num , typ) obj##num##_##typ##_vect
#define ISR_vect_num( obj, num , typ) obj##num##_##typ##_vect_num
#define MACRO_DDR( port) PASTE2( DDR, port)
#define MACRO_DDR_BIT( port, bit) PASTE3( DD, port ,bit)
#define SETBIT(port,bit) ((port)|=1L<<(bit))
#define CLEARBIT(port,bit) ((port)&=~(1L<<(bit)))

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// lokale Typendefinition
//
////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//
// USART Types
//
/////////////////////////////////////////////////////////////////////////////////////////
typedef union
{
	uint16_t Word;
	uint8_t ByteArr[2];
	struct
	{
		uint8_t Low;
		uint8_t High;
	}Byte;
}unuint16_t;

/////////////////////////////////////////////////////////////////////////////////////////
//
// USART Register Types
//
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// USART Receiver Data Register
/////////////////////////////////////////////////////////////////////////////////////////
typedef union
{
	uint16_t Word[1];
	uint8_t ByteArr[2];
	struct 
	{
		uint16_t DATA:9;
		uint8_t PERR:1;
		uint8_t FERR:1;
		uint8_t reserved:3;
		uint8_t BUFOVF:1;
		uint8_t RXCIF:1;
	}Register;
}RXDATA_t;

/////////////////////////////////////////////////////////////////////////////////////////
// USART Control and Status Register A
/////////////////////////////////////////////////////////////////////////////////////////
typedef union
{
	uint8_t this;
	uint8_t Byte;
	struct
	{	// Low Bit
		uint8_t RS485:2;
		uint8_t ABEIE:1;
		uint8_t LBME:1;
		uint8_t RXSIE:1;
		uint8_t DREIE:1;
		uint8_t TXCIE:1;
		uint8_t RXCIE:1;
	}Register;
}CTRLA_t;

/////////////////////////////////////////////////////////////////////////////////////////
//USART Control and Status Register B
/////////////////////////////////////////////////////////////////////////////////////////
typedef union
{
	uint8_t Byte;
	struct
	{	// Low Bit
		uint8_t MPCM:1;
		uint8_t RXMODE:2;
		uint8_t ODME:1;
		uint8_t SFDEN:1;
		uint8_t reserved:1;
		uint8_t TXEN:1;
		uint8_t RXEN:1;
	}Register;
}CTRLB_t;

/////////////////////////////////////////////////////////////////////////////////////////
// USART Control and Status Register C
/////////////////////////////////////////////////////////////////////////////////////////
typedef union
{
	uint8_t Byte;
	struct
	{	// Low Bit
		uint8_t CHSIZE:3;
		uint8_t SBMODE:1;
		uint8_t PMODE:2;
		uint8_t CMODE:2;
	}Reg_AsyncMode;
	struct
	{	// Low Bit
		uint8_t reserved0:1;
		uint8_t UCPHA:1;
		uint8_t UDORD:1;
		uint8_t reserved1:3;
		uint8_t CMODE:2;
	}Reg_MasterSPI;
}CTRLC_t;

/////////////////////////////////////////////////////////////////////////////////////////
// USART Control and Status Register D
/////////////////////////////////////////////////////////////////////////////////////////
typedef union
{
	uint8_t Byte;
	struct
	{	// Low Bit
		uint8_t reserve:6;
		uint8_t ABW:2;
	}Register;
}CTRLD_t;

/////////////////////////////////////////////////////////////////////////////////////////
// USART Status Register
/////////////////////////////////////////////////////////////////////////////////////////
typedef union
{
	register8_t Byte;
	struct
	{	// Low Bit
		uint8_t WFB:1;
		uint8_t BDF:1;
		uint8_t reserve:1;
		uint8_t ISFIF:1;
		uint8_t RXSIF:1;
		uint8_t DREIF:1;
		uint8_t TXCIF:1;
		uint8_t RXCIF:1;
	}Register;
}STATUS_t;
#define fastSTATUS(handle) ((STATUS_t)handle->STATUS)

/////////////////////////////////////////////////////////////////////////////////////////
// PORTMUX Control register for USART
/////////////////////////////////////////////////////////////////////////////////////////
typedef union
{
	uint8_t Byte;
	struct
	{
		PORTMUX_USARTx_t USART3_2:2;
		PORTMUX_USARTx_t USART2_2:2;
		PORTMUX_USARTx_t USART1_2:2;
		PORTMUX_USARTx_t USART0_2:2;
	}Register;
}USARTROUTEA_t;

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	USART Port Tables Defines
//
////////////////////////////////////////////////////////////////////////////////////////////////////
EEMEM const uint8_t USARTPort[4] = {0, 2, 5, 1};

EEMEM const uint8_t USARTPortTX_Set[4] = {0x11,0x11,0x11,0x11};
EEMEM const uint8_t USARTPortRX_Clear[4] = {0x22,0x22,0x22,0x22};
EEMEM const uint8_t USARTPortXCK_Set[4] = {0x44,0x44,0x44,0x04};
EEMEM const uint8_t USARTPortXDIR_Set[4] = {0x88,0x88,0x08,0x08};
EEMEM const uint8_t USARTPortDirMask[4] = {0x0F, 0xF0, 0x00, 0x00};


EEMEM const uint8_t USARTRXEventID[4] = {USART0_RXC_vect_num, USART1_RXC_vect_num, \
	USART2_RXC_vect_num, (int16_t)NULL};
EEMEM const uint8_t USARTTXEventID[4] = {USART0_TXC_vect_num, USART1_TXC_vect_num, \
	USART2_TXC_vect_num, (int16_t)NULL};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	USART lokale Variablen
//
////////////////////////////////////////////////////////////////////////////////////////////////////
USART_receive_Array_callback_fnc_t USART_receive_Array_callback[4] = {NULL};								// 8 Bytes
USART_send_Array_callback_fnc_t USART_send_Array_callback[4] = {NULL};										// 8 Bytes

USART_t* const USART_handle[4] =
{
	#ifdef USART0
	&USART0,
	#else
	NULL,
	#endif

	#ifdef USART1
	&USART1,
	#else
	NULL,
	#endif
	
	#ifdef USART2
	&USART2,
	#else
	NULL,
	#endif
	
	#ifdef USART3
	&USART3
	#else
	NULL
	#endif
};																											// 4 Bytes

uint8_t USART_adresse[4] = {0};																				// 4 Bytes
uint8_t USART_bytes_to_receive[4] = {0};																	// 4 Bytes
uint8_t USART_bytes_received[4] = {0};																		// 4 Bytes

////////////////////////////////////////////////////////////////////////////////////////////////////
// USART FIFOs
////////////////////////////////////////////////////////////////////////////////////////////////////
sFIFO_t USART_RX_FIFO[COUNT_USART] = preinit_FIFO_arr(COUNT_USART);											// max. 4x35 Bytes
sFIFO_t USART_TX_FIFO[COUNT_USART] = preinit_FIFO_arr(COUNT_USART);											// max. 4x35 Bytes

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Definition lokaler Funktionen
//
////////////////////////////////////////////////////////////////////////////////////////////////////
void USART_RXC_Byte(USART_t* const handle, uint8_t USARTnumber, uint8_t event_id);
void USART_TXC_Byte(USART_t* const handle, uint8_t USARTnumber, uint8_t event_id);

////////////////////////////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routinen defines
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// Makro für USART-ISR abhängig von der USART-Nummer und Typ
////////////////////////////////////////////////////////////////////////////////////////////////////
//*//
#define ISR_USARTx_INT(USARTnumber, typ) \
ISR(ISR_vect(USART,USARTnumber,typ))\
{\
		PASTE3(USART_,typ,_Byte)(USART_handle[USARTnumber],USARTnumber, ISR_vect_num(USART,USARTnumber,typ));\
		USART_handle[USARTnumber]->STATUS &=  PASTE3(USART_,typ,IF_bm);\
}\
//*/

/*//
ISR(USART0_RXC_vect)
{
	PASTE3(USART_,RXC,_Byte)(USART_handle[0],0,ISR_vect_num(USART,0,RXC));
	//USART_RXC_Byte(USART_handle[0],0,0);
	USART_handle[0]->STATUS &=  PASTE3(USART_,RXC,IF_bm);
}
//*/

////////////////////////////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routinen
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma GCC push_options
#pragma GCC optimize("O3")
#ifdef USART0
ISR_USARTx_INT(0,RXC)
ISR_USARTx_INT(0,TXC)
//ISR_USARTx_INT(0,DRE)
#endif
#ifdef USART1
ISR_USARTx_INT(1,RXC)
ISR_USARTx_INT(1,TXC)
//ISR_USARTx_INT(1,DRE)
#endif
#ifdef USART2
ISR_USARTx_INT(2,RXC)
ISR_USARTx_INT(2,TXC)
//ISR_USARTx_INT(2,DRE)
#endif
#ifdef USART3
ISR_USARTx_INT(3,RXC)
ISR_USARTx_INT(3,TXC)
//ISR_USARTx_INT(3,DRE)
#endif
#pragma GCC pop_options

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Funktionendeklaration
//
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// bool USART_init(uint8_t USARTnumber, uint32_t baudrate, USART_CHSIZE_t bits, USART_PMODE_t parity, 
//	USART_SBMODE_t stopbit, bool sync, bool addressmode, uint8_t address)
//
// Initialisiert die USART Schnittstelle
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma GCC push_options
#pragma GCC optimize("O0")
bool USART_init(uint8_t USARTnumber, uint32_t baudrate, USART_CHSIZE_t bits, USART_PMODE_t parity, \
	USART_SBMODE_t stopbit, bool sync, bool MPCM, uint8_t address, PORTMUX_USARTx_t PortMux)
{
	USART_t* const handle = USART_handle[USARTnumber];
	unuint16_t u16_baudteiler = {0};
	
	// Registerinhalte laden
	CTRLA_t UCSRA = {.Byte = 0x00};
	CTRLB_t UCSRB = {.Byte = 0x00};
	CTRLC_t UCSRC = {.Byte = USART_CHSIZE_8BIT_gc};
	CTRLD_t UCSRD = {.Byte = 0x00};
	
	// Lade das Port-Mux-Register von der USART-Schnittstellen
	USARTROUTEA_t USARTROUTEA = {.Byte = PORTMUX.USARTROUTEA};
	
	bool returnval = true;
	
	// Prüfe auf die Konsistenz der Eingangsvariablen
	if((bits > USART_CHSIZE_9BITH_gc)||((bits > USART_CHSIZE_8BIT_gc)&&(bits < USART_CHSIZE_9BITL_gc))\
		||(parity > USART_PMODE_ODD_gc)||\
		((stopbit < USART_SBMODE_1BIT_gc)&&(stopbit > USART_SBMODE_2BIT_gc)))
	{
		returnval = true;
	}
	else if(handle != NULL)
	{
		// Berechnen des Bautratenregister für synchrone oder
		if(sync)
		{
			u16_baudteiler.Word = (uint16_t)MIN((((CLK_CPU/baudrate/2))<<6), UINT16_MAX);
		}
		else // für asynchrone Übertragungen
		{
			u16_baudteiler.Word = (uint16_t)MIN(((CLK_CPU*8)/baudrate),UINT16_MAX);
			if(u16_baudteiler.Word < 64)
			{
				u16_baudteiler.Word = 64;
			}
		}
		
		// Anzahl der Bits einstellen
		UCSRC.Reg_AsyncMode.CHSIZE = bits;
		
		// Parität einstellen
		UCSRC.Reg_AsyncMode.PMODE = ((uint8_t)parity>>4);
		
		// synchrone Übertragung einstellen
		if(sync)
		{
			UCSRC.Reg_AsyncMode.CMODE = ((uint8_t)USART_CMODE_SYNCHRONOUS_gc>>6);
		}
		else
		{
			UCSRC.Reg_AsyncMode.CMODE = ((uint8_t)USART_CMODE_ASYNCHRONOUS_gc>>6);
		}
		
		// USART Multiprozessor Kommunikation einstellen für RS485
		if(MPCM)
		{
			UCSRA.Register.RS485 = USART_RS485_EXT_gc;
			UCSRB.Register.MPCM = true;
			USART_adresse[USARTnumber] = address;
			
			// Zwei Stop-Bits einstellen, zwingend notwendig für den MPCM
			UCSRC.Reg_AsyncMode.SBMODE  = (uint8_t)USART_SBMODE_2BIT_gc>>3;
		} 
		else
		{
			UCSRA.Register.RS485 = USART_RS485_OFF_gc;
			UCSRB.Register.MPCM = false;
			USART_adresse[USARTnumber] = 0x00;
			
			// Stop-Bit einstellen
			UCSRC.Reg_AsyncMode.SBMODE = ((uint8_t)stopbit>>3);
		}
		
		// schalte doppelte Geschwindigkeit an (nur asynchrone Übertragungen)
		if(!sync)
			UCSRB.Register.RXMODE = (uint8_t)USART_RXMODE_CLK2X_gc>>1;
		
		// Konfiguriere I/O-Pins und den Portmultiplexer
		USARTROUTEA.Byte = PORTMUX.USARTROUTEA;
		switch(USARTnumber)
		{
		case 0:
			USARTROUTEA.Register.USART0_2 = PortMux;
			break;
		case 1:
			USARTROUTEA.Register.USART1_2 = PortMux;
			break;
		case 2:
			USARTROUTEA.Register.USART2_2 = PortMux;
			break;
		case 3:
			USARTROUTEA.Register.USART3_2 = PortMux;
			break;
		}
		
		// Schalte RX und TX an
		UCSRB.Register.RXEN = true;
		UCSRB.Register.TXEN = true;
		
		// Schalte RX und TX Interrupte an
		UCSRA.Register.RXCIE = true;
		UCSRA.Register.TXCIE = true;
		
		// Schreibe das Port-Mux-Register für die UART-Schnittstelle
		PORTMUX.USARTROUTEA = USARTROUTEA.Byte;
		
		// Schreibe die UART-Register
		handle->CTRLA = UCSRA.Byte;
		handle->CTRLB = UCSRB.Byte;
		handle->CTRLC = UCSRC.Byte;
		handle->CTRLD = UCSRD.Byte;
		
		handle->BAUD = u16_baudteiler.Word;
		
		handle->DBGCTRL  = 0x00;
		handle->EVCTRL   = 0x00;
		handle->TXPLCTRL = 0x00;
		handle->RXPLCTRL = 0x00;
		
		// Konfiguriere die Portregister
		{
			
			uint8_t USARTPort_loc;
			uint8_t USARTPort_Mask_loc;
			uint8_t USARTPortTX_Set_loc;
			uint8_t USARTPortRX_Clear_loc;
			uint8_t USARTPortXCK_SET_loc = 0x00;
			uint8_t USARTPortXDIR_SET_loc = 0x00;
			
			while((NVMCTRL.STATUS&NVMCTRL_EEBUSY_bm) == NVMCTRL_EEBUSY_bm);
			USARTPort_loc = MAP_EEPROM(USARTPort[USARTnumber]);
			
			while((NVMCTRL.STATUS&NVMCTRL_EEBUSY_bm) == NVMCTRL_EEBUSY_bm);
			USARTPort_Mask_loc = MAP_EEPROM(USARTPortDirMask[PortMux]);
			
			while((NVMCTRL.STATUS&NVMCTRL_EEBUSY_bm) == NVMCTRL_EEBUSY_bm);
			USARTPortTX_Set_loc = MAP_EEPROM(USARTPortTX_Set[USARTnumber]);
			
			while((NVMCTRL.STATUS&NVMCTRL_EEBUSY_bm) == NVMCTRL_EEBUSY_bm);
			USARTPortRX_Clear_loc = MAP_EEPROM(USARTPortRX_Clear[USARTnumber]);
			
			
			if(sync)
			{
				while((NVMCTRL.STATUS&NVMCTRL_EEBUSY_bm) == NVMCTRL_EEBUSY_bm);
				USARTPortXCK_SET_loc = MAP_EEPROM(USARTPortXCK_Set[USARTnumber]);
			}
			if(MPCM)
			{
				while((NVMCTRL.STATUS&NVMCTRL_EEBUSY_bm) == NVMCTRL_EEBUSY_bm);
				USARTPortXDIR_SET_loc = MAP_EEPROM(USARTPortXDIR_Set[USARTnumber]);
			}
			
			if((USARTPort_Mask_loc != 0xFF)&&(USARTPort_loc != 0xFF))
			{
				PORTs.INDEX[USARTPort_loc].DIRSET = \
				(USARTPortTX_Set_loc|USARTPortXCK_SET_loc|USARTPortXDIR_SET_loc) & USARTPort_Mask_loc;
				PORTs.INDEX[USARTPort_loc].DIRCLR = USARTPortRX_Clear_loc & USARTPort_Mask_loc;
				returnval = false;
			}
			else
			{
				returnval = true;
			}
		}
	}
	
	return returnval;
}
#pragma GCC pop_options

////////////////////////////////////////////////////////////////////////////////////////////////////
// void USART_DeInit(uint8_t USARTnumber)
//
// Deinitialisiert die USART Schnittstelle
//  alle Register werden zurückgesetzt
////////////////////////////////////////////////////////////////////////////////////////////////////
void USART_DeInit(uint8_t USARTnumber)
{
	USART_t* handle = USART_handle[USARTnumber];
	
	// Registerinhalte laden
	CTRLA_t UCSRA = {.Byte = 0x00};
	CTRLB_t UCSRB = {.Byte = 0x00};
	CTRLC_t UCSRC = {.Byte = USART_CHSIZE_8BIT_gc};
	CTRLD_t UCSRD = {.Byte = 0x00};
		
	// Schreibe die UART-Register
	handle->CTRLA = UCSRA.Byte;
	handle->CTRLB = UCSRB.Byte;
	handle->CTRLC = UCSRC.Byte;
	handle->CTRLD = UCSRD.Byte;
		
	handle->BAUD = 0x0000;
		
	handle->DBGCTRL  = 0x00;
	handle->EVCTRL   = 0x00;
	handle->TXPLCTRL = 0x00;
	handle->RXPLCTRL = 0x00;
	
	USART_receive_Array_callback[USARTnumber] = NULL;
	USART_adresse[USARTnumber] = 0;
	USART_adresse[USARTnumber] = 0;
	USART_bytes_received[USARTnumber] = 0;
	
	FIFO_flush(&USART_RX_FIFO[USARTnumber]);
	FIFO_flush(&USART_TX_FIFO[USARTnumber]);
}

void USART_flushRXFIFO(uint8_t USARTnumber){
	USART_bytes_to_receive[USARTnumber] = 0;
	USART_bytes_received[USARTnumber] = 0;
	FIFO_flush(&USART_RX_FIFO[USARTnumber]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// void USART_get_RX_Byte(uint8_t event_id, uint8_t eventdata)
//	Interne Funktion, der Aufruf erfolgt nur über die ISR()
//		
//		const USART_t* handle -> Zeiger auf die USART-Register
//		uint8_t USARTnumber -> Nummer des USART-Ports.
//		uint8_t event_id -> Event-ID-Nummer
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma GCC push_options
#pragma GCC optimize("O3")
void USART_RXC_Byte(USART_t* const handle, uint8_t number, uint8_t event_id)
{
	uint8_t Buffer = 0;
	
	CTRLB_t UCSRB;
	
	UCSRB.Byte = handle->CTRLB;
	// USART Multiprozessor-Kommunikation
	if(UCSRB.Register.MPCM == true)
	{
		
	}
	else // normale UASRT Operation
	{
		// prüfe ob Daten vorhanden sind
		if( fastSTATUS(handle).Register.RXCIF == true )
		{
			// lese Byte aus dem Buffer
			/* Mod hier*/
			Buffer = handle->RXDATAL;
			uint8_t check = (USART_bytes_to_receive[number] != 0) &&\
							FIFO_chk_for_writedata(&USART_RX_FIFO[number]);// prüfe ob Daten in den FIFO geschrieben werden können
			/* Ende vom Mod*/
			if(check)
			{
				//Mod here: verschieben die Lesen immer nach oben
				// schreibe Byte in den FIFO
				FIFO_writebyte(&USART_RX_FIFO[number], Buffer);
				
				// Bytezähler vergrößern
				USART_bytes_received[number]++;
				
				// prüfe ob die vorgegebene Anzahl an Bytes gelesen worden sind
				if(USART_bytes_to_receive[number] == USART_bytes_received[number])
				{
					#if USART_USE_EVENTSYSTEM==1
					
					ISR_C_func(event_id);
					USART_bytes_received[number] = 0;
					
					#elif USART_USE_EVENTSYSTEM==0
					// Prüfen ob eine Callbackfunktion hinterlegt ist
					if(USART_receive_Array_callback[number] != NULL)
					{
						uint8_t Adress = 0;
						uint8_t* Data = NULL;
						uint8_t length = USART_bytes_received[number];
						
						// Länge des Arrays bestimmen (vollständige Datenanzahl im FIFO)
						length = FIFO_diff_readdata(&USART_RX_FIFO[number]);
						
						// Datenarray für die Funktionsübergabe anlegen
						Data = (uint8_t*)malloc(length*sizeof(uint8_t));
						
						// Kopiere Daten in den FIFO
						FIFO_copy_to_lin(Data, &USART_RX_FIFO[number], length);
						
						// Rufe Callbackfunktion auf
						USART_receive_Array_callback[number](Adress, Data, length);
						
						// lösche Datenarray
						free(Data);
						
						USART_bytes_received[number] = 0;
					}
					#endif //USART_USE_EVENTSYSTEM==0
				}
			}
		}
	}
}
#pragma GCC pop_options

////////////////////////////////////////////////////////////////////////////////////////////////////
// void USART_set_TX_Byte(uint8_t USARTnumber)
//	Interne Funktion, der Aufruf erfolgt nur über die ISR()
//
//		const USART_t* handle -> Zeiger auf die USART-Register
//		uint8_t USARTnumber -> Nummer des USART-Ports.
//		uint8_t event_id -> Event-ID-Nummer
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma GCC push_options
#pragma GCC optimize("O3")
void USART_TXC_Byte(USART_t* const handle, uint8_t USARTnumber, uint8_t event_id)
{
	//uint8_t Adress_Flag  = 0;
	uint8_t Buffer = 0;
	CTRLB_t UCSRB;
	
	UCSRB.Byte = handle->CTRLB;
	// USART Multiprozessor-Kommunikation
	if(UCSRB.Register.MPCM == true)
	{
		
	}
	else // normale USART Operation
	{
		if(FIFO_readbyte(&USART_TX_FIFO[USARTnumber], &Buffer))
		{
			// Abfragen ob USART frei ist
			if(fastSTATUS(handle).Register.DREIF == true)
				handle->TXDATAL = Buffer;
		}
		else // Wenn keine Daten im FIFO mehr vorhanden sind, Aufruf von der Callback-TX-Funktion
		{
			#if USART_USE_EVENTSYSTEM==1
			ISR_C_func(event_id);
			#elif USART_USE_EVENTSYSTEM==0
			uint8_t Adress = 0;
			uint8_t* Data = NULL;
			uint8_t length = 0;
			// Prüfen ob eine Callbackfunktion hinterlegt ist
			if(USART_send_Array_callback[USARTnumber] != NULL)
			{
				// Aufrufen der Callback-Funktion
				USART_send_Array_callback[USARTnumber](&Adress, &Data, &length, FIFO_diff_writedata(&USART_TX_FIFO[USARTnumber]));
				// Prüfen ob Daten kopiert worden sind
				if((Data != NULL) && (length != 0))
				{
					// Kopiere Daten in den FIFO
					FIFO_copy_from_lin(&USART_TX_FIFO[USARTnumber], Data, length);
					// Prüfe ob der übergebene Zeiger im Heap liegt
					if(((void*)__malloc_heap_start < (void*)Data) && ((void*)__malloc_heap_end >= (void*)Data))
					{
						//Wenn ja, dann löschen des Heapspeichers
						free(Data);
					}
					
					// Schreiben des ersten Bytes in das USART_UDR Register
					if(FIFO_readbyte(&USART_TX_FIFO[USARTnumber], &Buffer))
					{
						// Abwarten bis USART frei ist
						while ( !fastSTATUS(handle).Register.DREIF );
						// Kopiere Byte
						handle->TXDATAL = Buffer;
					}
				}
			}
			#endif //USART_USE_EVENTSYSTEM==0
		}
	}
}
#pragma GCC pop_options

////////////////////////////////////////////////////////////////////////////////////////////////////
// bool USART_send_Array(uint8_t USARTnumber, uint8_t adress, uint8_t data[], uint8_t length)
//
// Funktion zum Senden eines Frames über die USART Schnittstelle. Daten werden vorher im FIFO
// abgelegt und nacheinander gesendet.
//
//	Übernahmeparameter:
//		uint8_t USARTnumber -> Nummer des USART-Ports.
//		uint8_t adress		-> Adresse für den Empfänger für die Multiprozessor-Kommunikation.
//		uint8_t data[]		-> Daten-Array, welche übertragen weden sollen.
//		uint8_t length		-> Länge des Daten-Arrays.
//	
//	Übergabeparameter:
//		return bool			-> 	Daten wurden erfolgreich in den FIFO übertragen und die Übertragung
//								wurde gestartet.
////////////////////////////////////////////////////////////////////////////////////////////////////
bool USART_send_Array(uint8_t USARTnumber, uint8_t adress, uint8_t data[], uint8_t length)
{
	USART_t* handle = USART_handle[USARTnumber];
	bool returnval = false;
	// Prüfen ob Daten kopiert worden sind
	if((data != NULL) && (length != 0))
	{
		// Schreiben des ersten Bytes in das USART_UDR Register
		// Abfragen ob USART frei ist
		//if(fastSTATUS(handle).Register.DREIF) <- geht beim ATMEGA4809 durch den TX buffer nicht mehr
		// in dem Zusammenhang
		// Jetzt abfragen ob der FIFO leer ist
		if(!FIFO_chk_for_readdata(&USART_TX_FIFO[USARTnumber]))
		{
			// Kopiere erstes Byte
			handle->TXDATAL = data[0];
			
			// Kopiere die Restlichen Daten in den FIFO
			FIFO_copy_from_lin(&USART_TX_FIFO[USARTnumber], data+1, length-1);
			returnval = true;
		}
		else
		{
			// Kopiere die Restlichen Daten in den FIFO
			FIFO_copy_from_lin(&USART_TX_FIFO[USARTnumber], data, length);
			returnval = true;
		}
	}
	return returnval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// bool USART_receive_Array(uint8_t USARTnumber, uint8_t* adress, uint8_t* data[], 
// uint8_t max_length, uint8_t* length)
//
// Liest die zuvor emfpangenen Bytes aus dem FIFO aus und speichert diese in ein Array.
// Das Array muss vorher angelegt und als Zeiger übergeben werden.
// 
//	Übernahmeparameter:
//		uint8_t USARTnumber -> Nummer des USART-Ports
//		uint8_t* adress		-> Zeiger auf einen externen Speicherbereich für die empfangene Adresse.
//		uint8_t* data[]		-> Zeiger auf einen externes Datenarray.
//		uint8_t max_length	-> Maximale Länge des Datenarrays.
//		uint8_t* length		-> Zeiger auf einen externen Speicherbereich für die gespeicherte Länge.
//	Übergabeparameter:
//		return bool			-> true keine weiteren Daten im FIFO
//		adress				-> Empfänger-Adresse für die Multiprozessor-Kommunikation. 
//		data[]				-> Empfangsdaten der USART.
//		length				-> Länge der gespeicherten Daten.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma GCC push_options
#pragma GCC optimize("O3")
bool USART_receive_Array(uint8_t USARTnumber, uint8_t* adress, uint8_t* data[], uint8_t max_length,\
	uint8_t* length)
{
	*length = 0;
	if(adress!=NULL)
		*adress = 0;
	// Prüfen ob Daten kopiert werden können
	if(data != NULL)
	{
		// lese die maximale Anzahl der Bytes im FIFO
		*length = MIN(max_length, FIFO_diff_readdata(&USART_RX_FIFO[USARTnumber]));
		if(*length != 0)
		{
			// Kopiere Daten vom FIFO in das Array
			FIFO_copy_to_lin(*data, &USART_RX_FIFO[USARTnumber], *length);
		}
	}
	
	return (FIFO_diff_readdata(&USART_RX_FIFO[USARTnumber]) == 0);
}
#pragma GCC pop_options

////////////////////////////////////////////////////////////////////////////////////////////////////
// void USART_set_receive_Array_callback_fnc(uint8_t USARTnumber, 
//	USART_receive_Array_callback_fnc_t USART_receive_Array_callback_in)
//
// Speichert die Empfangs-Callback-Funktion.
// Diese wird nach einem vollständigen empfangenen Frame aufgerufen.
// Die Framlänge wird mit der Funktion "USART_set_Bytes_to_receive" eingestellt.
//
//	Übernahmeparameter:
//		uint8_t USARTnumber -> Nummer des USART-Ports
//		USART_receive_Array_callback_fnc_t USART_receive_Array_callback_in -> Funktionszeiger für 
//																			  die Callbackfunktion 
////////////////////////////////////////////////////////////////////////////////////////////////////
void USART_set_receive_Array_callback_fnc(uint8_t USARTnumber, \
	USART_receive_Array_callback_fnc_t USART_receive_Array_callback_in)
{
	if(USART_receive_Array_callback_in != NULL)
	{
		USART_receive_Array_callback[USARTnumber] = USART_receive_Array_callback_in;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// void USART_set_send_Array_callback_fnc(uint8_t USARTnumber, 
//	USART_send_Array_callback_fnc_t USART_send_Array_callback_in)
//
// Speichert den Zeiger der Sende-Callback-Funktion.
// Diese wird nach einem vollständigen gesendeten Frame aufgerufen.
// Die Framlänge wird mit der Funktion "USART_send_Array" übergeben.
//
//	Übernahmeparameter:
//		uint8_t USARTnumber -> Nummer des USART-Ports
//		USART_send_Array_callback_fnc_t USART_send_Array_callback_in -> Funktionszeiger für
//																		die Callbackfunktion 
//
////////////////////////////////////////////////////////////////////////////////////////////////////
void USART_set_send_Array_callback_fnc(uint8_t USARTnumber, \
	USART_send_Array_callback_fnc_t USART_send_Array_callback_in)
{
	if(USART_send_Array_callback_in != NULL)
	{
		USART_send_Array_callback[USARTnumber] = USART_send_Array_callback_in;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// void USART_set_Bytes_to_receive(uint8_t USARTnumber, uint8_t Bytes_to_receive)
//	
// Speichert die Framelänge die empfangen werden soll, eh ein Callback oder ein Event aufgerufen 
// wird.
//
//	Übernahmeparameter:
//		uint8_t USARTnumber -> Nummer des USART-Ports
//		uint8_t Bytes_to_receive -> Bytes die Empfangen werden sollen. 
//									Maximale Anzahl wird duch den FIFO bestimmt.
////////////////////////////////////////////////////////////////////////////////////////////////////
void USART_set_Bytes_to_receive(uint8_t USARTnumber, uint8_t Bytes_to_receive)
{
	USART_bytes_to_receive[USARTnumber] = Bytes_to_receive;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// uint8_t USART_get_RXeventID(uint8_t USARTnumber)
//
//	Gibt dynamisch die RX-Event-ID zur USART Nummer zurück 
//
//	Übernahmeparameter:
//		uint8_t USARTnumber -> Nummer des USART-Ports
//
////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t USART_get_RXeventID(uint8_t USARTnumber)
{
	while((NVMCTRL.STATUS&NVMCTRL_EEBUSY_bm) == NVMCTRL_EEBUSY_bm);
	return MAP_EEPROM(USARTRXEventID[USARTnumber]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// uint8_t USART_get_TXeventID(uint8_t USARTnumber)
//
//	Gibt dynamisch die RX-Event-ID zur USART Nummer zurück 
//
//	Übernahmeparameter:
//		uint8_t USARTnumber -> Nummer des USART-Ports
//
////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t USART_get_TXeventID(uint8_t USARTnumber)
{
	while((NVMCTRL.STATUS&NVMCTRL_EEBUSY_bm) == NVMCTRL_EEBUSY_bm);
	return MAP_EEPROM(USARTTXEventID[USARTnumber]);
}

// EOF //

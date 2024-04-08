/*
 * GccApplication2.c
 *
 * Created: 2023/11/9 11:50:27
 * Author : 芋泥啵啵奶茶
 */ 
//#include <xc.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include ".\ATMegaXX09\ATMegaXX09Clock.h"
#include ".\ATMegaXX09\USART\USART.h"

#include "conversation.h"
#include "frame_CRC.h"
extern void generate_frame(uint8_t adress, uint8_t *data, uint8_t len, uint8_t *frame);
bool generate_frame_and_send(uint8_t *auswahl, uint8_t data_lenght);

#define adress_daten 0x12	//Speicheradresse fuer daten
#define DIV256

typedef union energy_template
{
	struct{
		uint8_t lsb[4];
		uint8_t t1:1;
		uint16_t energy_1kw;
		uint8_t t2:7;
		uint8_t t3;
	}struc;
	uint64_t value;
}energy_t;


//#define BLINK_DELAY_MS 500
#define DIV256
uint16_t voltage, current;
uint32_t power, power_old;
energy_t energy;
uint16_t time = 5;
char flag;

typedef union sendData_tt
{
	uint8_t daten[6];
	struct 
	{
		uint16_t Spannung;
		uint16_t Strom;
		uint16_t Energie;
	}werte;
}sendData_t;

	// set baud for uasrt0
	// set config. registers for uasrt0
	
	uint8_t daten[6]={0,0,0,0,0,0}; // wegen Datenuebertragung ein Array
	sendData_t daten2 = {0};
		
	//uint8_t *TX_Frame=NULL;	//Array fuer Senden an Slave-Modul; oeffentlich, wegen Abrage, ob es vom Slave-Modul erfolgreich empfangen wurde. Kann gel鰏cht werden, wenn 躡ertragung funktioniert
	uint8_t TX_Frame[9];		//Array fuer Senden an Slave-Modul; oeffentlich, wegen Abrage, ob es vom Slave-Modul erfolgreich empfangen wurde
	uint8_t send_state=0;		//Sendestatus; Wenn UART-Uebertragung stattfindet, soll keine erneute gleichzeitige Uebertragung stattfinden, bevor das Slave-Modul antwortet
	//0: Keine aktuelle Uebertragung; Neue Uebertragung moeglich
	//1: (wird nur gesetzt, wenn daten vorhanden) Erste Uebertragung fuer daten gestartet. Neue Uebertragung nicht moeglich bis ACK von Slave-Modul den Status auf 2 setzt
	//2: Letzte Uebertragung gestartet; Neue Uebertragung nicht moeglich bis ACK von Slave-Modul den Status wieder auf 0 setzt

	// write byte to uart tx data register
    /* Replace with your application code */
	
//Funktionendeklaration
	/*
	readData_callback
	Wandelt nach Leseinterrupt den String in das geforderte Zahlenformat und speichert diese in globale Variablen.
	*/

	bool readData_callback(uint8_t adress, uint8_t data[], uint8_t length)
	{
		char *string_daten=NULL;
		
		
		if (USART_receive_Array(iUSART1, &adress, &data, 255, &length))			//Wenn Auslesen des gesamten FIFO nach dem Callback erfolgreich
		{
		
			//Anlegen der Zwischenspeichers
			string_daten = (char*)malloc(length*sizeof(char));
			
			
			string_from_Frame(data, length, string_daten);		//Strings aus Frame ermitteln
			daten_conversion(string_daten,daten);								//String in Array wandeln
			free(string_daten);													//Freigabe Speicher
			
		}
		return true;
	}
	
	bool generate_frame_and_send(uint8_t *auswahl, uint8_t data_lenght)
	{
		uint8_t frame_len = 7+ data_lenght;				//Framelaenge: 7+ Datenbytes
		uint8_t adress=0;								//Speicheradresse bei USV-Slave
		bool returnval=false;							//Return-Wert

		if (auswahl==daten)						//richtige Auswahl getroffen
		{
			adress=adress_daten;
			

			//TX_Frame=(uint8_t*)malloc(frame_len*sizeof(uint8_t));		//Speicher f黵 Groe遝 des Frames anlegen. Hatte nicht funktioniert fuer Speed, da wahrscheinlich urspr黱glich zu klein. Kann gel鰏cht werden.
			generate_frame(adress,auswahl,data_lenght,TX_Frame);		//Frame generieren: 0x12: Speicheradresse;
			USART_send_Array(iUSART1 , 0, TX_Frame, frame_len);			//Sende ueber UART
			returnval=true;
		}
		
		return returnval;
	}
	
bool Slave_Response(uint8_t adress, uint8_t data[], uint8_t length)
{
	bool returnval=false;
	uint8_t num_of_send=0;			//Anzahl der Sendeversuche
	
	if (USART_receive_Array(iUSART1, &adress, &data, 255, &length))	//Wenn Auslesen des gesamten FIFO nach dem Callback erfolgreich
	{
		if  ((data[0]==0xA1) || (data[0]==0xA2))		//Pruefe, ob Antwort vom Slave-Modul: 0xA1 = ACK; 0xA2 = NACK;
		{
			// Wenn NACK, pruefe ob bereits neue Werte vom Sensor vorhanden sind oder noch nicht alle Sendeversuche gestartet wurden
			if ((data[0]==0xA2) && (daten[0]=0) && (num_of_send<3))
			{
				USART_send_Array(iUSART1 , 0, TX_Frame, length);		//Wiederhole das Senden an Slave-Modul; maximal, aber bis zu 3 Versuche
				num_of_send++;											//Sendeversuchszaehler hochzaehlen
			}
			//Ansonsten, wenn Sendestatus erfolgreich, oder neue Werte vorhanden sind bzw. Anzahl Versuche ueberschritten sind, gebe Sendespeicher wieder frei
			else
			{
				
				send_state = 0;		
				

				//free (TX_Frame);		//Freigabe des Sendespeichers; Hatte nicht funktioniert. Kann gel鰏cht werden, wenn 躡ertragung funktioniert
				num_of_send=0;			//Setze Sendezaehler zurueck
			}
			
			returnval=true;
		}
	}

	return returnval;
}

void ADC_Init()
{
	/*
	Control A
	Run in Standby (Bit7): Off --> 0
	Resolution Selection (Bit2): 10 bit--> 1
	Free-Running (Bit1): Off --> 0
	ADC Enable (Bit0): On --> 1
	*/
	ADC0.CTRLA |= ADC_ENABLE_bm;
	
	/*
	Control B
	No accumulation
	*/
	ADC0.CTRLB = 0x0;
	
	/*
	Control C
	Sample Capacitance Selection (Bit6): Vref>1V --> 1
	Reference Selection (Bit5:4): external reference --> 10
	Prescaler (Bit2:0): DIV256 --> 0x07
	*/
	ADC0.CTRLC=ADC0.CTRLC & 0x00;
	ADC0.CTRLC|=(ADC_SAMPCAP_bm|ADC_PRESC_DIV256_gc);
	
	/*
	Control D
	Initialization Delay (Bit7:5): Delay 256 CLK_ADC cycles
	Automatic Sampling Delay Variation (Bit4): off
	*/
	ADC0.CTRLD = ADC0.CTRLD & ~ADC_PRESC_gm & ~ADC_REFSEL_gm & ~ADC_REFSEL0_bm;
	ADC0.CTRLD |= (ADC_INITDLY2_bm|ADC_SAMPDLY0_bm)&~ADC_ASDV_bm;
	
	/*
	Control E
	Window Comparator Mode (Bit2:0): No Window Comparison
	*/
	ADC0.CTRLE = ADC0.CTRLE & ~ADC_WINCM_gm;
	
}

void TCA0_init(){
	//1. Write a TOP value to the Period (TCAn.PER) register
	//TCA0_SINGLE_PER |= 0xc350; //50000 (5 ms
	//TCA0_SINGLE_PERBUF = 50000;
	TCA0_SINGLE_PER = 63951;
	//2. Enable the peripheral by writing a ‘1’ to the ENABLE bit in the Control A (TCAn.CTRLA) register.
	TCA0_SINGLE_CTRLA = (TCA0_SINGLE_CTRLA & ~0x0f); //| 0x01;
	TCA0_SINGLE_CTRLA = 1<<1;
	TCA0_SINGLE_CTRLA |= 0b00000001;
	
	// Aktiviere den Interrupt für den Vergleichsmatch
	//TCA0_SINGLE_INTCTRL = TCA_SINGLE_OVF_bm;
	TCA0_SINGLE_INTCTRL = TCA_SINGLE_OVF_bm;

	// Setze den Timer in den normalen Modus
	//TCA0_SINGLE_CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;
	TCA0_SINGLE_CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;
}

uint8_t mux_tab[] = {0,1};
uint8_t indexTab = 0;
uint16_t ADC_res[] = {0,0};

void VREF_init()
{
	VREF.CTRLA = (VREF.CTRLA & ~VREF_AC0REFSEL_gm) | VREF_AC0REFSEL1_bm | VREF_ADC0REFSEL1_bm;
	VREF.CTRLB = VREF.CTRLB | VREF_AC0REFEN_bm | VREF_ADC0REFEN_bm;
}

void LED_init()
{
	PORTA.DIR |= (PIN4_bm|PIN5_bm|PIN6_bm|PIN7_bm);
	//PORTA.OUT |= ((1 << 4)|(1 << 5)|(1 << 6)|(1 << 7));
	PORTC.DIR |= (PIN2_bm|PIN3_bm);
}

void LED_on(int pos)
{
	PORTA.OUT &= ~(1 << pos);
}

void LED_off(int pos)
{
	PORTA.OUT |= (1 << pos);
}

void LED_toggle(int pos)
{
	PORTA.OUT ^= (1 << pos);
}

void error_off(int pos)
{
	pos += 1;
	PORTC.OUT &= ~(1 << pos);
}

void error_on(int pos)
{
	pos += 1;
	PORTC.OUT |= (1 << pos);
}

void error_toggle(int pos)
{
	pos += 1;
	PORTC.OUT ^= (1 << pos);
}

uint16_t ADC_Read(){
	uint16_t tmp;
	// Warten Sie, bis die Konversion abgeschlossen ist
	while (ADC0.COMMAND & (ADC0.COMMAND | ADC_STCONV_bm));
	
	// Lesen und zurückgeben des ADC-Wertes
	tmp = ADC0_RES;
	return tmp;
}

void ADC_on(uint8_t channel){
	// Wählen Sie den AD-Kanal, von dem gelesen werden soll
	//Muxpos
	//Muxpos (Bit4:0): AIN0
	ADC0.MUXPOS = (ADC0.MUXPOS & ~ADC_MUXPOS_gm) | mux_tab[channel];
	indexTab = channel;
	//Starten Sie die Konversion
	ADC0.COMMAND = ADC0.COMMAND | ADC_STCONV_bm;
}

// Interrupt Service Routine für den Timer A Overflow
ISR(TCA0_OVF_vect) {
	//Interrupt Control vom ADC anschalten	ADC0.INTCTRL = ADC0.INTCTRL & ~(ADC_RESRDY_bm|ADC_WCMP_bm); //alles löschen	ADC0.INTCTRL |= ADC_RESRDY_bm; //ResultReady-Interrupt setzen
	
	// Toggle den Zustand der LED
	//LED_toggle(4);
	
	// Lösche das Overflow Flag, indem das entsprechende Bit geschrieben wird
	TCA0_SINGLE_INTFLAGS = TCA_SINGLE_OVF_bm;
	
	error_on(2);
	//auf Kanal 0
	ADC_on(0);
}

ISR(ADC0_RESRDY_vect){
	ADC_res[indexTab] = ADC_Read();
	error_toggle(2);
	if ( indexTab==0)
	{
		//auf Kanal 1
		ADC_on(1);
		error_toggle(2);
	}
	else
	{
		//Interrupt Control vom ADC ausschalten		ADC0.INTCTRL = ADC0.INTCTRL & ~(ADC_RESRDY_bm|ADC_WCMP_bm); //alles löschen
		error_off(2);
		//Messflag setzen
		flag = 1;
	}
}




bool send_data_callback(uint8_t* adress, uint8_t* data[], uint8_t* length, \
uint8_t max_length)
{
	/*
	send_data_callback
	wird nach Sende-Interupt aufgerufen. Ist hier nur definiert, um zu pruefen, ob Daten vollst鋘dig gesendet wurden.
	Die Pr黤ung erfolgt in der internen Funktion "USART_TXC_Byte"
	*/
	return true;
}

int main(void)
{	
	// 1. Systemstart
	
	init_Core_CLK();		//Takt einstellen
	ADC_Init();
	VREF_init();
	LED_init();
	PORTA.OUT  |= (1 << 4)|(1 << 5)|(1 << 6)|(1 << 7); //erstmal alles ausschalten
	PORTC.OUT  |= ((1 << 2)|(1 << 3));
	TCA0_init();
	
	sei();					//Interrupts an

	//USART_init(iUSART0,19200, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, USART_SBMODE_1BIT_gc,false,0,0,PORTMUX_USARTx_DEFAULT_gc);		//UART1 fuer Sensor-Empfang konfigurieren, TODO: Baudraute testen
	//Default laut Datenblatt: 8-bits, no parity, 19,200 baud rate, and 1 stop bit
	//USART_set_receive_Array_callback_fnc(iUSART0,&readData_callback);	//Setzen des Funktionszeigers, wenn Interrupt durch empfangenen Frame vom Sensor ausgeloest wird
	//USART_set_send_Array_callback_fnc(iUSART0, &send_data_callback);					    //Setzen des Zeichenkettenendes bis zu dem der String vor Callback zwischengespeichert wird. Annahme, dass Sensor bis zum Linefeed (0x0A) sendet.
	
	//TODO: an Realitaet anpassen. Ggf. Set_Bytes_to_receive verwenden
	
	USART_init(iUSART1,250000, USART_CHSIZE_8BIT_gc, USART_PMODE_ODD_gc, USART_SBMODE_1BIT_gc,false,0,0,PORTMUX_USARTx_DEFAULT_gc);			//UART2 fuer Bus-Senden konfigurieren
	//USV-Protokoll: 8-bits, odd(ungerade) parity, 250,000 baud rate, and 1 stop bit
	USART_set_Bytes_to_receive(iUSART1,1);								//Setzen der Groe遝 der Daten, die vor Callback zwischengespeichert werden sollen.
	USART_set_receive_Array_callback_fnc(iUSART1,&Slave_Response);		//Setzen des Funktionszeigers, wenn Interrupt durch empfangenen Frame vom Slave ausgeloest wird
	USART_set_send_Array_callback_fnc(iUSART1, &send_data_callback);	//Setzen des Funktionszeigers, wenn Interrupt nach gesendeten Frame zu Slave ausgeloest wird

   //2. Variablendefinition
   size_t length=0;				//Laenge der zu uebertragenden Nutzdaten
    while (1) 
    {
		if(flag == 1)
		{
			flag = 0;
			//Berechnen Sie die Spannung (angenommen, die Referenzspannung ist 2.5V)
			voltage = (uint16_t)(((uint32_t)ADC_res[0] * 15000) >> 10); //Millivolt
			//Berechnen Sie den Strom (angenommen, die Referenzspannung ist 2.5V)
			current = (uint16_t)(((uint32_t)ADC_res[1] * 2025938UL + 374) >> 14); //MilliAmpere
			power_old = power;
			power = (uint32_t) voltage * (uint32_t) current; //millimilliwatt
			energy.value = energy.value + (uint64_t)((power + power_old) >> 1);
			daten2.werte.Spannung = voltage;
			daten2.werte.Strom = current;
			daten2.werte.Energie = energy.struc.energy_1kw;
			send_state=0;
			
			
		}
		
		if (send_state==0)				//Pruefe, ob eine UART-躡ertragung bereits stattfindet. 0: keine 躡etragung
		{
			if (daten2.daten[0]!=0) 			//Pruefe, ob Range-Wert vorhanden ist
			{
				length=sizeof(daten2);
				generate_frame_and_send(daten2.daten, length);					//Frame von Range generieren
				send_state=1;
									//Wenn daten vorhanden sind, starte sp鋞er neue Uebertragung f黵 speed
			}										//Sende-Status 1: Wenn von Slave ACK kommt, wird als n鋍hstes speed 黚etragen
			else
				send_state=2;											//Sende-Status 2: Wenn von Slave ACK kommt, wird 躡etragung beendet
				
				daten[0]=0;												//Range-Wert zuruekstetzen, um neue Uebertragung abfragen zu koennen

			}
			
		
		
    }
}
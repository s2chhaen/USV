/*
 * main.c
 *
 * Created: 3/9/2023 12:17:29 PM
 *  Author: Admin
 */ 

#include <xc.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include ".\ATMegaXX09\ATMegaXX09Clock.h"
#include ".\ATMegaXX09\USART\USART.h"

#include "conversion.h"
#include "Frame_CRC.h"
#include "LED.h"

//Konstanten
#define adress_range 0x12	//Speicheradresse fuer Abstand
#define adress_speed 0x13	//Speicheradresse fuer Geschwindigkeit

//Globale Variablen
uint8_t speed[2]={0, 0};	//Geschwindigkeit
uint8_t range[1]={0};		//Entfernung; wegen Datenuebertragung ein Array
//uint8_t *TX_Frame=NULL;	//Array fuer Senden an Slave-Modul; oeffentlich, wegen Abrage, ob es vom Slave-Modul erfolgreich empfangen wurde. Kann gelöscht werden, wenn Übertragung funktioniert
uint8_t TX_Frame[9];		//Array fuer Senden an Slave-Modul; oeffentlich, wegen Abrage, ob es vom Slave-Modul erfolgreich empfangen wurde
uint8_t send_state=0;		//Sendestatus; Wenn UART-Uebertragung stattfindet, soll keine erneute gleichzeitige Uebertragung stattfinden, bevor das Slave-Modul antwortet
							//0: Keine aktuelle Uebertragung; Neue Uebertragung moeglich
							//1: (wird nur gesetzt, wenn Range und Speed vorhanden) Erste Uebertragung fuer Range gestartet. Neue Uebertragung nicht moeglich bis ACK von Slave-Modul den Status auf 2 setzt
							//2: (wird nur gesetzt, wenn Range und Speed vorhanden) Zweite Uebertragung fuer Geschwindigkeit kann gestartet werden
							//3: Letzte Uebertragung gestartet; Neue Uebertragung nicht moeglich bis ACK von Slave-Modul den Status wieder auf 0 setzt

//Funktionendeklaration
/*
readData_callback
Wandelt nach Leseinterrupt den String in das geforderte Zahlenformat und speichert diese in globale Variablen.
*/
bool readData_callback(uint8_t adress, uint8_t data[], uint8_t length)
{
	char *string_speed=NULL;
	char *string_range=NULL;
	
	if (USART_receive_Array(iUSART1, &adress, &data, 255, &length))			//Wenn Auslesen des gesamten FIFO nach dem Callback erfolgreich
	{
		led_tgl();															//LED blinkt bei Datenuebertragung
		
		//Anlegen der Zwischenspeichers
		string_speed = (char*)malloc(length*sizeof(char));
		string_range = (char*)malloc(length*sizeof(char));
		
		string_from_Frame(data, length, string_speed, string_range);		//Strings aus Frame ermitteln
		speed_conversion(string_speed,speed);								//Speed-String in Array wandeln
		free(string_speed);													//Freigabe Speicher
		range[0]=range_conversion(string_range);							//Range-String in int wandeln		
		free(string_range);													//Freigabe Speicher
	}
	return true;
}

/*
generate_frame_and_send()
Generiert einen neuen Frame, wenn neue Werte vorhanden sind und sendet diesen an Slave-Modul

Übernahmeparameter:
	uint8_t *auswahl	-> Zeiger auf einen externen Speicherbereich für die empfangene Adresse.
	uint8_t data_lenght	-> Laenge der Nutzdaten
Übergabeparameter:
	return bool			-> true erfolgreich
*/
bool generate_frame_and_send(uint8_t *auswahl, uint8_t data_lenght)
{
	uint8_t frame_len = 7+ data_lenght;				//Framelaenge: 7+ Datenbytes
	uint8_t adress=0;								//Speicheradresse bei USV-Slave
	bool returnval=false;							//Return-Wert

	if ((auswahl==range) || (auswahl==speed))						//richtige Auswahl getroffen
	{
		if (auswahl==range)											//Pruefe Auswahl und setze Adresse entsprechend
		{
			adress=adress_range;
		}
		else if (auswahl==speed)
		{
			adress=adress_speed;
		}

		//TX_Frame=(uint8_t*)malloc(frame_len*sizeof(uint8_t));		//Speicher für Groeße des Frames anlegen. Hatte nicht funktioniert fuer Speed, da wahrscheinlich ursprünglich zu klein. Kann gelöscht werden.
		generate_frame(adress,auswahl,data_lenght,TX_Frame);		//Frame generieren: 0x12: Speicheradresse;
		USART_send_Array(iUSART1 , 0, TX_Frame, frame_len);			//Sende ueber UART
		returnval=true;
	}	
							
	return returnval;
}

/*
Slave_Response
Überprüft Antwort vom Slave-Modul über UART1, nachdem ein Frame gesendet wurde und setzt den Sende-Status entsprechend.
*/
bool Slave_Response(uint8_t adress, uint8_t data[], uint8_t length)
{	
	bool returnval=false;
	uint8_t num_of_send=0;			//Anzahl der Sendeversuche
		
	if (USART_receive_Array(iUSART1, &adress, &data, 255, &length))	//Wenn Auslesen des gesamten FIFO nach dem Callback erfolgreich
	{
		if  ((data[0]==0xA1) || (data[0]==0xA2))		//Pruefe, ob Antwort vom Slave-Modul: 0xA1 = ACK; 0xA2 = NACK;
		{
			// Wenn NACK, pruefe ob bereits neue Werte vom Sensor vorhanden sind oder noch nicht alle Sendeversuche gestartet wurden
			if ((data[0]==0xA2) && ((range[0]=0) || (speed[0]=0)) && (num_of_send<3))
			{
				USART_send_Array(iUSART1 , 0, TX_Frame, length);		//Wiederhole das Senden an Slave-Modul; maximal, aber bis zu 3 Versuche
				num_of_send++;											//Sendeversuchszaehler hochzaehlen
			}
			//Ansonsten, wenn Sendestatus erfolgreich, oder neue Werte vorhanden sind bzw. Anzahl Versuche ueberschritten sind, gebe Sendespeicher wieder frei
			else
			{
				//Pruefe Antwort vom Slave-Modul: 0xA1 und Sendestatus=1: Gibt es beide Werte fuer Range und Speed und fand vorher Range-Uebertragung statt.
				if ((send_state==1) && (data[0]==0xA1))
				{
					send_state = 2;		//Setze Sendestatus 2: Uebertragung der Geschwindigkeit als zweite Uebertragung ermoeglichen
				}
				else
				{
					send_state = 0;		//Setze Sendestatus zurueck, damit neue Uebertragung gestartet werden kann
				}

				//free (TX_Frame);		//Freigabe des Sendespeichers; Hatte nicht funktioniert. Kann gelöscht werden, wenn Übertragung funktioniert
				num_of_send=0;			//Setze Sendezaehler zurueck
			}
			
			returnval=true;
		}
	}

	return returnval;
}

/*
send_data_callback
wird nach Sende-Interupt aufgerufen. Ist hier nur definiert, um zu pruefen, ob Daten vollständig gesendet wurden.
Die Prüfung erfolgt in der internen Funktion "USART_TXC_Byte"
*/
bool send_data_callback(uint8_t* adress, uint8_t* data[], uint8_t* length, \
uint8_t max_length)
{
	return true;
}


int main(void)
{
	// 1. Systemstart	
	
 	init_Core_CLK();		//Takt einstellen
	
	led_config();			//LED konfigurieren
	led_ein();				//LED einschalten
	
	sei();					//Interrupts an

	USART_init(iUSART0,19200, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, USART_SBMODE_1BIT_gc,false,0,0,PORTMUX_USARTx_DEFAULT_gc);		//UART1 fuer Sensor-Empfang konfigurieren, TODO: Baudraute testen
																																			//Default laut Datenblatt: 8-bits, no parity, 19,200 baud rate, and 1 stop bit
	USART_set_receive_Array_callback_fnc(iUSART0,&readData_callback);	//Setzen des Funktionszeigers, wenn Interrupt durch empfangenen Frame vom Sensor ausgeloest wird
	USART_set_end_of_string(iUSART0, 0x0A);								//Setzen des Zeichenkettenendes bis zu dem der String vor Callback zwischengespeichert wird. Annahme, dass Sensor bis zum Linefeed (0x0A) sendet.
																		//Weitere Annahme: Geschwindigkeit und Entfernung werden in einer Zeichenkettenfolge gesendet, die in Geschwindigkeit und Entfernung aufgeteilt werden muss.
																		//TODO: an Realität anpassen. Ggf. Set_Bytes_to_receive verwenden
																																	
	USART_init(iUSART1,250000, USART_CHSIZE_8BIT_gc, USART_PMODE_ODD_gc, USART_SBMODE_1BIT_gc,false,0,0,PORTMUX_USARTx_DEFAULT_gc);			//UART2 fuer Bus-Senden konfigurieren
																																			//USV-Protokoll: 8-bits, odd(ungerade) parity, 250,000 baud rate, and 1 stop bit
	USART_set_Bytes_to_receive(iUSART1,1);								//Setzen der Groeße der Daten, die vor Callback zwischengespeichert werden sollen.
 	USART_set_receive_Array_callback_fnc(iUSART1,&Slave_Response);		//Setzen des Funktionszeigers, wenn Interrupt durch empfangenen Frame vom Slave ausgeloest wird
 	USART_set_send_Array_callback_fnc(iUSART1, &send_data_callback);	//Setzen des Funktionszeigers, wenn Interrupt nach gesendeten Frame zu Slave ausgeloest wird


	//2. Variablendefinition
	size_t length=0;				//Laenge der zu uebertragenden Nutzdaten
	
    while(1)
    {
		if (send_state==0)				//Pruefe, ob eine UART-Übertragung bereits stattfindet. 0: keine Übetragung
		{
			if (range[0]!=0) 			//Pruefe, ob Range-Wert vorhanden ist
			{
				length=sizeof(range);
				generate_frame_and_send(range, length);					//Frame von Range generieren
				
				if ((range[0]!=0)  && (speed[0]!=0))					//Wenn beide Werte vorhanden sind, starte später neue Uebertragung für speed
				send_state=1;											//Sende-Status 1: Wenn von Slave ACK kommt, wird als nächstes speed übetragen
				else
				send_state=3;											//Sende-Status 3: Wenn von Slave ACK kommt, wird Übetragung beendet
				
				range[0]=0;												//Range-Wert zuruekstetzen, um neue Uebertragung abfragen zu koennen

			}
			else if (speed[0]!=0)		//Pruefe, ob Speed-Wert vorhanden ist
			{
				length=sizeof(speed);
				generate_frame_and_send(speed, length);					//Frame von Geschwindigkeit generieren
				
				send_state=3;											//Sende-Status 3: Wenn von Slave ACK kommt, wird Übetragung beendet
				
				speed[0]=0;												//Speed-Wert zuruecksetzen, um neue Uebertragung abfragen zu koennen
				speed[1]=0;
			}
		}
		else if (send_state==2)			//Pruefe, ob eine UART-Übertragung bereits stattfindet. 2: keine Übetragung nach Range-Senden
		{
			length=sizeof(speed);										
			generate_frame_and_send(speed, length);						//Frame von Geschwindigkeit generieren
			
			send_state=3;												//Sende-Status 3: Wenn von Slave ACK kommt, wird Übetragung beendet
			
			speed[0]=0;													//Speed-Wert zuruecksetzen, um neue Uebertragung abfragen zu koennen
			speed[1]=0;
		}
	}
}
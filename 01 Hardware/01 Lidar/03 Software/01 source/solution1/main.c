/*
 * solution1.c
 *
 * Created: 11.03.2023 16:31:29
 * Author : Riccardo
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include ".\ATMegaXX09\ATMegaXX09Clock.h"
#include ".\ATMegaXX09\USART\USART.h"
#include "dataprep.h"


/* Header vor dem Senden der eigentlichen Daten: 06 02 80 D6 02 B0 69 01 
anschließend folgen 722 Datenbyte --> 361 Datenpunkte, jeweils LSB / MSB

UART RS232 - ATMega: iUSART0
UART Slave - ATMega: iUSART1
UART RS422 - ATMega: iUSART2

*/

/*Globale Variablen**********************************************************
*uint8_t ack : wird mit Acknowledge Antwort vom Slave überschrieben
*uint8_t *datastream : enthält die vom USART empfangenen Sensordaten
*uint8_t *filtered_data : enthält die gefilterten Sensordaten (nach Finden des Headers)
*uint8_t *distances : enthält die berechneten Distanzen in cm
*uint8_t *rounded_distances : enthält die auf 0,5m gerundeten Distanzen
*uint8_t *converted_distances : enthält die von cm in m konvertierten Distanzen gemäß Forderung
*bool state = false : Abarbeitungsstatus der empfangenen Daten; ist true, wenn Daten gesendet werden können
*************************************************************/
uint8_t ack = 0x0;
uint8_t *datastream;
uint8_t *filtered_data;
uint8_t *distances;
uint8_t *rounded_distances;
uint8_t *converted_distances;
bool state = false;


/*daten_empfangen**********************************************************
*Input:
*uint8_t adress
*uint8_t data[]
*uint8_t length
*
*Output:
*bool true : fehlerfreies Abarbeiten der Funktion
* Callback Funktion, wird nach Empfangs- Interrupt aufgerufen
* verarbeitet empfangene Daten, sucht nach Startheader und speichert folgende 722 Byte in neues Array
* weitere Funktionen zum Berechnen der Abstandswerte, zum Runden der Werte und zum Konvertieren der Werte in Meter werden ausgeführt
*************************************************************/

bool daten_empfangen(uint8_t adress, uint8_t data[], uint8_t length)
{
	int i = 0;
	int j = 0;
	int n;
	unsigned char header[] = {0x06, 0x02, 0x80, 0xD6, 0x02, 0xB0, 0x69, 0x01};
	bool header_found = false;
	
	USART_set_Bytes_to_receive(iUSART0,0);	//Daten empfangen deaktivieren, bis Daten verarbeitet sind
	for (i=0; i<length; i++) {
		datastream[i] = data[i];
	}
	
	// Header: 0x06, 0x02, 0x80, 0xD6, 0x02, 0xB0, 0x69, 0x01
	while (!header_found && i<length) {
			if (datastream[i] == header[j]) {
				j = j+1;
			}
			else {
				j = 0;
			}
			if (j==8) {
				header_found = true;
			}
	}
	for (n=i; n<i+722; n++) {
		filtered_data[n] = datastream[n];
	}
	distances = bytes_to_values(filtered_data);
	rounded_distances = round_values(distances);
	converted_distances = cm_to_m(rounded_distances);
	state = true;
	
	return true;
}


/*daten_senden**********************************************************
*Input:
*uint8_t adress
*uint8_t data[]
*uint8_t length
*
*Output:
*bool true : fehlerfreies Abarbeiten der Funktion
* Callback Funktion, wird nach Sende- Interrupt aufgerufen und prüft, ob Daten erfolgreich an Slave gesendet wurden
* max. Sendeversuche: 3, anschließend werden neue Daten vom Sensor verarbeitet
*************************************************************/
bool daten_senden(uint8_t adress, uint8_t data[], uint8_t length)
{
	uint8_t num_of_send=0;											//Anzahl der Sendeversuche
	
	if (USART_receive_Array(iUSART1, &adress, &data, 255, &length))	{//Wenn Auslesen des gesamten FIFO nach dem Callback erfolgreich
		//Pruefe Antwort vom Slave-Modul: 0xA1 = ACK; 0xA2 = NACK; und Anzahl der max. Sendeversuche
		if ((data[0]==0xA2) && (num_of_send<3)) {
			length = sizeof(converted_distances);
			USART_send_Array(iUSART1 , 0, converted_distances, length);		//Wiederhole das Senden an Slave-Modul; maximal, aber bis zu 3 Versuche
			num_of_send++;											//Sendeversuchszaehler hochzaehlen
		}
		else {
			num_of_send=0;
			ack = 0x0;
		}
	}
	return true;
}


/*setup**********************************************************
*Funktion initialisiert internen Taktgeber und USART- Schnittstellen
*************************************************************/

void setup() {
	uint8_t start_command = {0x02, 0x00, 0x02, 0x00, 0x20, 0x24, 0x34, 0x08};
	init_Core_CLK();
	USART_init(iUSART0,9600, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, USART_SBMODE_1BIT_gc,false,0,0,PORTMUX_USARTx_DEFAULT_gc); //UART für Sensor
	//USART_receive_Array_callback_fnc_t USART_receive_Array_callback_in = &daten_empfangen; //Initialisierung von einem Zeiger auf die Funktion zum Daten empfangen
	USART_set_receive_Array_callback_fnc(iUSART0,&daten_empfangen); //Übergabe der Callback- Funktion bei Interrupt an USART0
	USART_init(iUSART1,250000, USART_CHSIZE_8BIT_gc, USART_PMODE_ODD_gc, USART_SBMODE_1BIT_gc,false,0,0,PORTMUX_USARTx_DEFAULT_gc);	//UART für Slave Modul
	USART_set_Bytes_to_receive(iUSART1,1);
	USART_set_receive_Array_callback_fnc(iUSART1,&daten_senden);
	USART_send_Array(iUSART0, 0x0, start_command, sizeof(start_command));	//Startkommando senden, um Messung zu starten
	//TODO: Konfiguration des LIDARS über Kommandos, falls Einstellungen nicht mehr wirksam sind
}

/*CRC8**********************************************************
*Funktion bildet eine Checksumme
*************************************************************/
uint8_t CRC8( uint8_t *addr, uint8_t len){
	uint8_t crc = 0;
	while (len--){
		uint8_t inbyte = *addr++;
		uint8_t i;
		for (i = 8; i; i--){
			uint8_t mix = ( crc ^ inbyte ) & 0x80;
			crc <<= 1;
			if (mix)
			{
				crc ^= 0xD5;	// Generatorpolynom 0xD5
			}
			inbyte <<= 1;
		}
	}
	return crc;
}


int main(void) {
	uint8_t adress = 0x203;	//Stand: Januar 2023; muss ggf. nochmal geändert werden, falls sich die Zuordnung ändert
	uint8_t crc8;
	size_t length = 0;
	int i = 0;
	setup();
	
    while (1) {
		USART_set_Bytes_to_receive(iUSART0,1460); //Setzen der Buffergröße, bevor Callback ausgelöst wird: 1460 = 2x Gesamtgröße Datenstream (8 Byte Header + 722 Byte Daten)
		//TODO: Startup Message herausfinden, scheint mit Datenblatt nicht übereinzustimmen
		//wenn Sendestatus = true, dann bilde Checksumme und sende Daten an Slave
		if (state) {
			length = sizeof(converted_distances);
			crc8 = CRC8(converted_distances, length);
			converted_distances[length+1] = crc8;
			USART_send_Array(iUSART1 , adress, converted_distances, length);
			if (ack) {
				//Sendestatus zurücksetzen
				state = false;
				//alle empfangenen Daten werden mit 0 überschrieben, um neue Daten zu empfangen
				for (i=0; i<sizeof(datastream); i++) {
					datastream[i] = 0;
				}
			}
		}	
    }
	return 0;
}


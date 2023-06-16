/*
 * solution1.c
 *
 * Created: 11.03.2023 16:31:29
 * Author : Riccardo
 * Bearbeiter: Thach
 */ 


#include "main.h"


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
volatile uint8_t ack = 0x0;
volatile uint8_t *datastream;
volatile uint8_t *filtered_data;
volatile uint8_t *distances;
volatile uint8_t *rounded_distances;
volatile uint8_t *converted_distances;
volatile bool state = false;



//Implementation von CRC16-Checksum durch Anpassung Muster-Program in Buch "Telegramme zur Konfiguration und//Bedienung der Lasermesssysteme LMS2xx" von Hersteller
uint16_t checksumCrc16(uint8_t input[], uint32_t length){
	const uint16_t genPolynom = 0x8005;
	uint16_t uCrc16=0;
	uint8_t temp[]={0,0};
	for (uint32_t i=0;i<length;i++){
		temp[1]=temp[0];
		temp[0]=input[i];
		if (uCrc16&0x8000){
			uCrc16 = (uCrc16&0x7fff)<<1;
			uCrc16 ^= genPolynom;
			} else{
			uCrc16<<=1;
		}
		uCrc16 ^= (input[0]|(input[1]<<8));
	}
	return uCrc16;
}

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
	int j = 0;
	int i = 0;
	const uint8_t header[] = {0x06, 0x02, 0x80, 0xD6, 0x02, 0xB0, 0x69, 0x01};
	bool header_found = false;
	
	USART_set_Bytes_to_receive(iUSART0,0);	//Daten empfangen deaktivieren, bis Daten verarbeitet sind
	for (i=0; i<length; i++) {
		datastream[i] = data[i];
	}
	
	// Header-check
	// Header: 0x06, 0x02, 0x80, 0xD6, 0x02, 0xB0, 0x69, 0x01
	while (!header_found && i<length) {
			if (datastream[i] == header[j]) {
				j++;
			}
			else {
				j = 0;
			}
			if (j==8) {
				header_found = true;
			}
	}
	for (int n=i; n<i+722; n++) {
		filtered_data[n] = datastream[n];
	}
	distances = bytes_to_values((uint8_t *)filtered_data);
	rounded_distances = round_values((uint8_t *)distances);
	converted_distances = cm_to_m((uint8_t *)rounded_distances);
	state = true;
	
	return true;
}

//refactoring of function daten_empfangen
bool dataReceive(uint8_t adress, uint8_t data[], uint8_t length){
	bool result = true;
	uint16_t config = NO_OF_VALUE|(PARTIAL_SCAN_00<<11)|(PARTIAL_SCAN<<13)|(CM_U<<14);
	uint8_t configL = (uint8_t)(config&0x00FF);
	uint8_t configH = (uint8_t)((config&0xFF00)>>8);
	const headerInst_t inst1 = {{ACK_SYMBOL, STX_SYMBOL, ADR_SLV, LENGTH_LOW , LENGTH_HIGH, DATA_REQ_RESP,configL,configH}};
	USART_set_Bytes_to_receive(iUSART0,0);
	for (int i = 0;i<length;i++){
		datastream[i] = data[i];
	}

	for (int i = 0;i<HEADER_LENGTH;i++){
		if (datastream[i]!=inst1.data[i]){
			result = false;
			break;
		}
	}
	if(result){
		for (int i = HEADER_LENGTH;i<HEADER_LENGTH+722;i++){
			filtered_data[i] = datastream[i];
		}
		distances = bytes_to_values((uint8_t *)filtered_data);
		rounded_distances = round_values((uint8_t *)distances);
		converted_distances = cm_to_m((uint8_t *)rounded_distances);
		state = true;
	}
	USART_set_Bytes_to_receive(iUSART1,1);
	return result;
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
	
	//if (USART_receive_Array(iUSART1, &adress, &data, 255, &length))	{//Wenn Auslesen des gesamten FIFO nach dem Callback erfolgreich
		//Pruefe Antwort vom Slave-Modul: 0xA1 = ACK; 0xA2 = NACK; und Anzahl der max. Sendeversuche
		if ((data[0]==0xA2) && (num_of_send<3)) {
			length = sizeof(converted_distances);
			USART_send_Array(iUSART1 , 0, (uint8_t *)converted_distances, length);		//Wiederhole das Senden an Slave-Modul; maximal, aber bis zu 3 Versuche
			num_of_send++;											//Sendeversuchszaehler hochzaehlen
		}
		else {
			num_of_send=0;
			ack = 0x0;
		}
	//}
	return true;
}

bool dataSend(uint8_t adress, uint8_t data[], uint8_t length){
	bool result = true;
	return result;
}

/*setup**********************************************************
*Funktion initialisiert internen Taktgeber und USART- Schnittstellen
*************************************************************/

void setup() {
	uint8_t start_command[] = {0x02, 0x00, 0x02, 0x00, 0x20, 0x24, 0x34, 0x08};
	init_Core_CLK();
	USART_init(iUSART0,BAUDRATE_SENSOR, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, USART_SBMODE_1BIT_gc,false,0,0,PORTMUX_USARTx_DEFAULT_gc); //UART für Sensor
	//USART_receive_Array_callback_fnc_t USART_receive_Array_callback_in = &daten_empfangen; //Initialisierung von einem Zeiger auf die Funktion zum Daten empfangen
	USART_set_receive_Array_callback_fnc(iUSART0,&daten_empfangen); //Übergabe der Callback- Funktion bei Interrupt an USART0
	USART_init(iUSART1,BAUDRATE_SLAVE, USART_CHSIZE_8BIT_gc, USART_PMODE_ODD_gc, USART_SBMODE_1BIT_gc,false,0,0,PORTMUX_USARTx_DEFAULT_gc);	//UART für Slave Modul
	USART_set_Bytes_to_receive(iUSART1,1);
	USART_set_receive_Array_callback_fnc(iUSART1,&daten_senden);
	USART_send_Array(iUSART0, 0x0, start_command, sizeof(start_command));	//Startkommando senden, um Messung zu starten
	//TODO: Konfiguration des LIDARS über Kommandos, falls Einstellungen nicht mehr wirksam sind
}

//refactoring of function setup()
uint8_t init(){
	uint8_t start_command_wth_checksum[]={STX_SYMBOL, SENSOR_ADR, 0x02, 0x00, SET_OP_MODE, ALL_VALUE_COUNTINUE};
	uint16_t checksumValue = checksumCrc16(start_command_wth_checksum, sizeof(start_command_wth_checksum)/sizeof(uint8_t));
	uint8_t checksumValueL = (uint8_t)(checksumValue & 0x00FF);
	uint8_t checksumValueH = (uint8_t)(checksumValue & 0xFF00);
	uint8_t start_command[] = {STX_SYMBOL, SENSOR_ADR, 0x02, 0x00, SET_OP_MODE, ALL_VALUE_COUNTINUE, checksumValueL, checksumValueH};
	init_Core_CLK();
	bool sensorUartInit = USART_init(iUSART0,BAUDRATE_SENSOR, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, USART_SBMODE_1BIT_gc, SYNC_TX,MPC_MODE, 0, PORTMUX_USARTx_DEFAULT_gc);
	USART_set_receive_Array_callback_fnc(iUSART0,&dataReceive);
	bool slaveUartInit = USART_init(iUSART1,BAUDRATE_SLAVE, USART_CHSIZE_8BIT_gc, USART_PMODE_ODD_gc, USART_SBMODE_1BIT_gc, SYNC_TX, MPC_MODE, 0, PORTMUX_USARTx_DEFAULT_gc);
	USART_set_receive_Array_callback_fnc(iUSART1,&dataSend);
	USART_set_Bytes_to_receive(iUSART1,1);
	USART_send_Array(iUSART0, 0, start_command, sizeof(start_command));
	return sensorUartInit&&slaveUartInit;
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
	uint16_t adress = 0x203;	//Stand: Januar 2023; muss ggf. nochmal geändert werden, falls sich die Zuordnung ändert
	uint8_t crc8;
	size_t length = 0;
	int i = 0;
	setup();
	int temp1=1460/32+1;//jedes FIFO ist 32 Byte groß
	
    while (1) {
		for(int i=0;i<temp1;i++){
			USART_set_Bytes_to_receive(iUSART0,32);
		}
		//USART_set_Bytes_to_receive(iUSART0,1460); //Setzen der Buffergröße, bevor Callback ausgelöst wird: 1460 = 2x Gesamtgröße Datenstream (8 Byte Header + 722 Byte Daten)
		//TODO: Startup Message herausfinden, scheint mit Datenblatt nicht übereinzustimmen
		//wenn Sendestatus = true, dann bilde Checksumme und sende Daten an Slave
		if (state) {
			length = sizeof(converted_distances);
			crc8 = CRC8((uint8_t*)converted_distances, length);
			converted_distances[length+1] = crc8;
			USART_send_Array(iUSART1 , adress, (uint8_t*)converted_distances, length);
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


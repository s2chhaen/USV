/*
 * Frame_CRC.c
 *
 * Created: 12.03.2023 19:56:50
 *  Author: Admin
 */ 

#include <avr/io.h>
#include <stdio.h>

/* crc8_Berechnung
Berechnet aus den Nutzdaten von beliebiger Laenge den CRC-8-Wert

Parameter:
data... Zeiger auf Nutzdatenarray
len ... Laenge des Nutzdatenarrays
Return: CRC-8 der Nutzdaten
*/

uint8_t crc8_Berechnung(uint8_t *data, uint8_t len)
{
	uint8_t crc=0;  					// CRC-Wert Initial-Value 0
	uint8_t i,j;        				// Schleifenzaehler
	uint8_t polynom = 0xD5;  			// Generatorpolynom

	for(i=0; i<len; i++) {				// Schleife für alle Datenbytes
		crc ^= (data[i]);				// XOR mit Nutzdaten
		
		// 8-Bit Schleife für alle Stellen des Generatorpolynoms
		for (j=0; j<8; j++)
		{
			if((crc & (1<<7))!=0)		// Pruefe MSB=1
			{					
				crc=crc ^ polynom;		// XOR mit Polynom
			}
			crc=(crc<<1);				// shift links
		}
	}

	return crc;
}

/* generate_frame
Generiert den USV-Frame, der vom Anwender- zum Slave-Modul auf den Bus gesendet werden soll

Parameter:
adress..Register-Adresse des Ziel-Slave-Moduls; 0x12 fuer Abstand; 0x13 fuer Geschwindigkeit
data... Zeiger auf Quelle der Nutzdaten
len ... Laenge des Nutzdatenarrays
frame...Zeiger auf Ziel-Array fuer Frame
*/
void generate_frame(uint8_t adress, uint8_t *data, uint8_t len, uint8_t *frame)
{
	uint8_t i;
	
	frame[0]=0xA5;		//Startbyte
	frame[1]=0x02;		//Slavespezifische ID fuer RADAR: 0x02
	frame[2]=0x80;		//0x8: 4 Bit fuer Schreiben + 0x0: Erste 4 Bits fuer Register-Adresse des Ziel-Slave-Moduls
	frame[3]=adress;	//Register-Adresse des Ziel-Slave-Moduls
	frame[4]=len+7;		//Framelaenge
	
	for (i=0; i<len; i++)	//Datenbytes fuellen.
	{
		frame[5+i]=data[i];
	}
	
	frame[5+i]=crc8_Berechnung(data, len);	//CRC8 von den Datenbytes, Polynom 0xD5
	frame[6+i]=0xA6;	//Endbyte
}
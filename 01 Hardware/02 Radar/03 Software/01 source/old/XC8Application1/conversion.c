/*
 * conversion.c
 *
 * Created: 11.03.2023 23:56:01
 *  Author: Admin
 */ 

#include <xc.h>
#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*	speed_conversion
Wandelt String für die Geschwindigkeit in eine Zahl

Parameter:
data: Zeiger auf Quell-Array
arr: Zeiger auf Ziel-Array
 */ 
void speed_conversion (char *data, uint8_t *arr)
{
	float _speed;
	signed char a;
	uint8_t b;
	
	sscanf(data,"%f",&_speed);		//in Fließkommazahl wandeln
	a=trunc(_speed);					//Vorkommazahl ermitteln
	if (a<0)
	{
		b=lround((-_speed+a)*256);	//Bits für Nachkommazahl ermitteln
	}
	else
	{
		b=lround((_speed-a)*256);	//Bits für Nachkommazahl ermitteln
	}
	
	arr[0]=a;
	arr[1]=b;
} 


/* range_conversion
Wandelt String für die Entfernung in eine Zahl

Parameter:
data: Zeiger auf Quell-Array
Rückgabewert: _range als angerundete Dezimalzahl
 */
uint8_t range_conversion (char *data)
{
	uint16_t _range;
	sscanf(data,"%u",&_range);			//integer-Wandlung
	return (uint8_t)_range;
}


/* string_from_Frame
Ermittelt aus dem Frame die Strings für die Geschwindigkeit und Entfernung. Annahme, dass String so aussieht: "-22.7 m/s 33.9 m". Ggf. an Realität anpassen

Parameter:
data: Zeiger auf Quell-Array
length: Länge des Quell-Arrays
str_speed: Zeiger auf Ziel-Array für den String der Geschwindigkeit
str_range: Zeiger auf Ziel-Array für den String der Entfernung
 */
void string_from_Frame(uint8_t *data, uint8_t length, char *str_speed, char *str_range)
{
	char *temp;							//Zwischenspeicher fuer Geschwindigkeit oder Entfernung
	uint8_t a=0;							//Zaehler fuer uebergeordnete Schleife
	uint8_t i=0;							//Zaehler fuer Schleife 1: erste Zahl
	uint8_t j=0;							//Zaehler für String_Index
	uint8_t k=0;							//Zaehler fuer Schleife 2: Nachfolgestellen
	bool num=false;
	bool point=false;					//Zahl oder Punkt in String enthalten

	//Uebergeordnete Schleife: 0: Speed; 1: Range
	for (a=0;a<=1;a++)
	{
		j=0;
	
		if (a==0)				
		{
			temp=str_speed;
		}
		else
		{
			temp=str_range;
		}
	
		//1.Schleife, um erste Zahl zu finden, aber nur maximal bis letzen Eintrag
		do				
		{
			if ((data[i]>= 0x30) && (data[i] <= 0x39))		//Suche Zahl 0..9
			{
				if ((i>0) && (data[i-1] == 0x2D))	//Suche, ob Minus [-]=0x2D vorhanden; Bsp.: -100.1
				{
					temp[j]=data[i-1];
					temp[j+1]=data[i];	
					j++;
				}
				else
				{
					temp[j]=data[i];
				}
				j++;
				k=i+1;
			
				//2.Schleife
				while (~(point || (k==length)))						//Suche Punkt [.], aber maximal bis letzen Eintrag
				{
					if ((data[k]>= 0x30) && (data[k] <= 0x39))		//Suche Zahl 0..9 als Nachfolgestellen
					{
						temp[j]=data[k];
					}
					else if ((data[k] == 0x2E) && (data[k+1]>= 0x30) && (data[k+1] <= 0x39))		//Suche nach Punkt [.]=0x2E und pruefe, ob Nachkommazahl vorhanden Bsp.: -100.1
					{
						temp[j]=data[k];
						temp[j+1]=data[k+1];
						k++;
						point=true;									//Punkt erkannt
						break;
					}
					else
					{
						point=true;									//keine weitere Zahl oder Punkt vorhanden
						break;
					}
					j++;
					k++;
					
				}
				num=true;									//Wenn Zahl gefunden. Schleifenende der Schleife 1.
				i=k+1;
				break;
			}
			else											//Wenn Zahl nicht gefunden, erhoehe Zaehler, um weiter zu suchen
			{
				i++;
			}
		} while (~(num || (i==length)));		
	}
}


/*
 * CFile1.c
 *
 * Created: 2023/12/12 15:40:37
 *  Author: 芋泥啵啵奶茶
 */ 
//#include <xc.h>
#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*	daten_conversion
Wandelt String in eine Zahl

Parameter:
data: Zeiger auf Quell-Array
arr: Zeiger auf Ziel-Array
 */ 
void daten_conversion (char *data, uint8_t *arr)
{
	float _daten;
	signed char a;
	uint8_t b;
	
	sscanf(data,"%f",&_daten);		//in Fliesskommazahl wandeln
	a=trunc(_daten);					//Vorkommazahl ermitteln
	if (a<0)
	{
		b=lround((-_daten+a)*256);	//Bits fuer Nachkommazahl ermitteln
	}
	else
	{
		b=lround((_daten-a)*256);	//Bits fuer Nachkommazahl ermitteln
	}
	
	arr[0]=a;
	arr[1]=b;
}



/*
   Extrahiert Strings für Daten aus dem Datenrahmen

   Parameter:
   data: Zeiger auf Quell-Array
   length: Länge des Quell-Arrays
   str_daten: Zeiger auf Ziel-Array für den daten-String

*/
void string_from_Frame(uint8_t *data, uint8_t length, char *str_daten)
{
    char *temp;  // Dient zum temporären Speichern des aktuellen Strings
    uint8_t i = 0;  // Index des Datenarrays
    uint8_t j = 0;  // Index des Stringarrays
    bool num = false;  // Gibt an, ob eine Zahl gefunden wurde
    bool point = false;  // Gibt an, ob ein Dezimalpunkt gefunden wurde

    // Durchläuft eine Schleife zur Verarbeitung von daten                                                             for (uint8_t a = 0; a < 4; a++)
    temp = str_daten;
        

     // Erste Schleife, um die erste Zahl zu finden
       
     // Sucht nach Ziffern 0..9
     if ((data[i] >= 0x30) && (data[i] <= 0x39))
            {
                temp[j] = data[i];
                j++;
                uint8_t k = i + 1;

                // Zweite Schleife, um einen Dezimalpunkt zu finden
               while (~(point || (k == length)))
               {
	               // Sucht nach Ziffern 0..9 als Dezimalstellen
	               if ((data[k] >= 0x30) && (data[k] <= 0x39))
	               {
		               temp[j] = data[k];
	               }
	               // Findet einen Dezimalpunkt [.]=0x2E und überprüft, ob Dezimalstellen vorhanden sind
	               else if ((data[k] == 0x2E) && (data[k + 1] >= 0x30) && (data[k + 1] <= 0x39))
	               {
		               temp[j] = data[k];
		               temp[j + 1] = data[k + 1];
		               k++;
		               point = true;  // Dezimalpunkt erkannt
		               break;
	               }
	               else
	               {
		               point = true;  // Keine weiteren Ziffern oder Dezimalpunkt vorhanden
		               break;
	               }
	               j++;
	               k++;
               }
               num = true;  // Zahl gefunden, verlässt Schleife 1
               i = k + 1;
               //break;
               }
               else
               {
	               i++;
               }
             while (~(num || (i == length)));
        
}
					
                    
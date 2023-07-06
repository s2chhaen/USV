#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "dataprep.h"

/*bytes_to_values**********************************************************
*Input:
*uint8_t received[] : Übergabe des Arrays, welches die gefilterten Daten enthält
*
*Output:
*uint8_t* : gibt Zeiger auf Array zurück
*gefilterte Daten bestehen aus 722 Byte in der Form (LSB, MSB) pro Datenpunkt
*Werte werden zu Distanzen verrechnet, resultierendes Array enthält 361 Daten (0,5° je Messpunkt, 180° gesamt)
*************************************************************/
processResult_t bytes_to_values(uint8_t input[], uint16_t inLength, uint8_t output[], uint16_t outLength) {
	processResult_t returnValue = NO_ERROR;
	if ((input==NULL)||(output==NULL)){
		returnValue = NULL_POINTER;
	} else if(inLength>(outLength*2)){
		returnValue = IN_OUT_NOT_EQUAL;
	} else{
		for (int i=0; i<=inLength; i+=2) {
			output[i/2] = input[i] + 256*input[i+1];
		}
	}
	return returnValue;
}

/*round_values**********************************************************
*Input:
*uint8_t data[] : Übergabe des Arrays, welches die 361 Messpunkte enthält
*
*Output:
*uint8_t* : gibt Zeiger auf Array zurück
*gemäß Forderung werden die Messwerte (liegen in cm vor) auf 50cm genau gerundet
*aus Sicherheitsgründen: immer abrunden
*************************************************************/
processResult_t round_values(uint8_t data[],uint16_t length) {
	processResult_t returnValue = NO_ERROR;
	if (data!=NULL){
		for (int i=0; i<=length; i++) {
			/*abrunden wie folgt: 3412cm --> 3000cm, 2976cm --> 2500cm, 891cm --> 500cm*/
			data[i] = (data[i]/500)*500;
		}
	} else{
		returnValue = NULL_POINTER;
	}	
	return returnValue;
}

/*cm_to_m**********************************************************
*Input:
*uint8_t data[] : Übergabe des Arrays, welches die gerundeten Messwerte (in cm) enthält
*
*Output:
*uint8_t* : gibt Zeiger auf Array zurück
*Funktion konvertiert die Messwerte (in cm) zu m
*gemäß Forderung werden die Daten mit 2 multipliziert, um ganzzahlige Werte an Slave senden zu können
*************************************************************/
processResult_t cm_to_m (uint8_t data[],uint16_t length) {
	processResult_t returnValue = NO_ERROR;
	for (int i=0; i<=length; i++) {
		//Abrunden wie folgt: kleiner als 0,5 zu 0,5 und sonst zu 1
		data[i] = data[i]*2/100;
	}
	
	return returnValue;
}
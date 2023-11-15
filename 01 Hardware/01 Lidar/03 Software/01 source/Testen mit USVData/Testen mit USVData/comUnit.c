/*
 * comUnit.c
 *
 * Created: 7/10/2023 4:59:46 AM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.3
 */ 

#include "comUnit.h"

const uint16_t usartFIFOMaxLen = _FIFO_max_def - 1;
comUnitBuffer_t comUnit_rx = {0};
comUnitBuffer_t comUnit_tx = {0};
volatile comHandlerStatusNConfig_t comUnit_control = {0};
	
volatile uint16_t tempVar;

/**
 * \brief die Funktion, die bei ISR() von USART-Transmit zurückgeruft wird
 * \detailed diese Funktion ermöglicht die Sendung der mehr als 31-Byte-langen Zeichenfolge
 *
 * \param adress die Adresse von Empfanger
 * \param data Zeiger zur Adresse des nach Ende dieser Funktion weiter in USART-FIFO kopierten Datenblockes
 * \param length die Länge des obergenannten Datenblockes
 * \param max_length die die leere Stelle in USART-FIFO
 * 
 * \return bool immer true, da es bisher nicht weiter betrachtet wird
 */
static bool usartCallbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length, uint8_t max_length){
	uint8_t temp = comUnit_tx.toHandleBytes;
	if(temp){
		*data = (uint8_t*)(&(comUnit_tx.data[comUnit_tx.strPtr]));
		if (temp < max_length){
			*length = temp;
			comUnit_tx.toHandleBytes = 0;
			comUnit_tx.strPtr = 0;
		} else{
			*length = max_length;
			comUnit_tx.toHandleBytes -= max_length;
			comUnit_tx.strPtr += max_length;
		}
	}else{
		comUnit_tx.strPtr = 0;//Reset des Zeigers vom Buffer
	}
	return true;
}

/**
 * \brief die Funktion, die bei ISR() von USART-Receive zurückgeruft wird
 * 
 * \param adress die Adresse von Sender
 * \param data Zeiger zum Datenblock, in dem die Daten aus USART-FIFO gespeichert werden
 * \param length die Länge des obergenannten Datenblockes
 * 
 * \return bool immer true, da es bisher nicht weiter betrachtet wird
 */
#pragma GCC push_options
#pragma GCC optimize ("O3")
static bool usartCallbackRx(uint8_t adress, uint8_t data[], uint8_t length){
	//if (comUnit_rx.toHandleBytes){
		comUnit_rx.toHandleBytes -= length;
		tempVar = comUnit_rx.toHandleBytes;
		memcpy((uint8_t*)&(comUnit_rx.data[comUnit_rx.strPtr]),data,length);
		comUnit_rx.strPtr += length;
		
		if (tempVar > usartFIFOMaxLen){
			USART_set_Bytes_to_receive(comUnit_control.usart4USVData,usartFIFOMaxLen);
		} else if(tempVar){
			USART_set_Bytes_to_receive(comUnit_control.usart4USVData,tempVar);
		}
	//}
	return true;
}
#pragma GCC pop_options

/**
 * \brief Funktion zum Überwachen einer Variable, ob es erwünschten Wert in bestimmter Zeit erreicht
 * \detailed diese Funktion überwächt die Variable obj in bestimmter Zeitraum (in Schleifen), wenn sie
 * den Wert desiredValue erreicht dann gibt die Funktion keinen Fehler zurück. Sonst gibt sie 
 * timeout-Fehler zurück
 *
 * \param cycles die max. Timeout-Zeit (in Zyklen)
 * \param obj die zu überwachende Variable
 * \param desiredValue der erwünschte Wert
 * 
 * \return uint8_t 0: kein Fehler, die Variable den Wert rechtzeitig erreicht, sonst: nicht
 */
static uint8_t waitWithBreak(uint64_t cycles, uint8_t* obj, uint8_t desiredValue){
	uint8_t result = NO_ERROR;
	//Betrachtet den Wert von obj in einer bestimmten Zeit (in Anzahl der Schleife)
	for (uint64_t i = 0; i<cycles;i++){
		if ((*obj)==desiredValue){
			break;
		}
	}
	//Wird es nach der Zeit den Wert nicht erreicht, dann Fehler zurückgegeben
	if((*obj)!=desiredValue){
		result = TIME_OUT;
	}
	return result;
}


/**
 * \brief Daten über USART senden
 * 
 * \param data die Zeiger zum Datenblock
 * \param length die Länge des Datenblocks
 * 
 * \return uint8_t 0: keinen Fehler, sonst: Fehler
 */
uint8_t usartDataTx(uint8_t* data, uint16_t length){
	uint8_t result = NO_ERROR;
	if(length > BUFFER_LEN){
		result = DATA_INVALID;
	} else if (data==NULL){
		result = NULL_POINTER;
	} else if (!comUnit_control.init){
		result = NO_INIT;
	} else{
		const uint16_t* temp = timer_getCounter();
		timer_setCounter((4*length*3)/2);//magic number: 4 = Zeitsdauer/Zeichen (us), 3/2 = Sicherheit
		timer_setState(1);
		while(comUnit_tx.toHandleBytes){
			if (!(*temp)){
				result = TIME_OUT;
				break;
			}
		};
		timer_setState(0);
		if (result==NO_ERROR){
			memcpy((uint8_t*)comUnit_tx.data, data, length);
			if (length <= usartFIFOMaxLen){
				comUnit_tx.toHandleBytes = 0;
				USART_send_Array(comUnit_control.usart4USVData, 0, (uint8_t*)(comUnit_tx.data), length);
			} else{
				comUnit_tx.toHandleBytes = length - usartFIFOMaxLen;
				comUnit_tx.strPtr += usartFIFOMaxLen;
				USART_send_Array(comUnit_control.usart4USVData, 0, (uint8_t*)(comUnit_tx.data), usartFIFOMaxLen);
			}
			timer_setState(1);
			while(comUnit_tx.toHandleBytes){
				if (!(*temp)){
					result = TIME_OUT;
					break;
				}
			};
			timer_setState(0);
		}
	}
	return result;
}

/**
 * \brief Funktion zur Empfangen der Daten über USART
 * 
 * \param data der Zeiger zum Speicherbuffer
 * \param length die Länge der Speicherbuffer
 * 
 * \return uint8_t 0: kein Fehler, sonst Fehler
 */
uint8_t usartDataRx(uint8_t* data, uint16_t length){
	uint8_t result = NO_ERROR;
	if (data==NULL){
		result = NULL_POINTER;
	} else if ((length>BUFFER_LEN) || (length==0)){
		result = DATA_INVALID;
	} else{
		const uint16_t* temp = timer_getCounter();
		timer_setCounter((4*length*3)/2);//magic number: 4 = Zeitsdauer/Zeichen (us), 3/2 = Sicherheit
		timer_setState(1);
		while(comUnit_rx.toHandleBytes){
			if (!(*temp)){
				result = TIME_OUT;
				break;
			}	
		};
		if (result==NO_ERROR){
			comUnit_rx.toHandleBytes = length;
			if (length < usartFIFOMaxLen){
				USART_set_Bytes_to_receive(comUnit_control.usart4USVData,length);
			} else{
				USART_set_Bytes_to_receive(comUnit_control.usart4USVData,usartFIFOMaxLen);
			}
			timer_setState(1);
			while(comUnit_rx.toHandleBytes){
				if (!(*temp)){
					result = TIME_OUT;
					break;
				}
			};
			if (result==NO_ERROR){
				memcpy((uint8_t*)data,(uint8_t*)comUnit_rx.data,length);
			} else{
				memcpy((uint8_t*)data,(uint8_t*)comUnit_rx.data,comUnit_rx.strPtr);
			}
			comUnit_rx.strPtr = 0;
		}
	}
	return result;
}

/**
 * \brief Funktion zur Initialisation des User-Unit-Objektes
 * 
 * \param config die Struktur mit der Konfigurationsinformation (für USART)
 * 
 * \return uint8_t 0: kein Fehler, sonst: Fehler
 */
uint8_t initUserUnit(usartConfig_t config){
	uint8_t result = NO_ERROR;
	volatile uint8_t usartInit = USART_init(config.usartNo,config.baudrate, config.usartChSize, config.parity, \
									config.stopbit, config.sync, config.mpcm,config.address, config.portMux);
	comUnit_control.usart4USVData = config.usartNo;
	comUnit_control.init = (!usartInit)?1:0;
	USART_set_send_Array_callback_fnc(config.usartNo,&usartCallbackTx);
	USART_set_receive_Array_callback_fnc(config.usartNo,&usartCallbackRx);
	result = usartInit?NO_ERROR:PROCESS_FAIL;
	return result;
}

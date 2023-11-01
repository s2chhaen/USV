/*
 * comUnit.c
 *
 * Created: 7/10/2023 4:59:46 AM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.1
 */ 

#include "comUnit.h"

const uint16_t usartFIFOMaxLen = _FIFO_max_def - 1;
volatile comUnitBuffer_t comUnit_rx = {0};
volatile comUnitBuffer_t comUnit_tx = {0};
volatile comHandlerStatusNConfig_t comUnit_control = {0};

/**
 * \brief die Funktion, die bei ISR() von USART-Transmit zurückgeruft wird
 * \detailed diese Funktion ermöglicht die Sendung der mehr als 31-Byte-langen Zeichenfolge
 *
 * \param adress die Adresse von Empfanger
 * \param data Zeiger zur Adresse des nach Ende dieser Funktion weiter in USART-FIFO kopierten Datenblockes
 * \param length die Länge des obergenannten Datenblockes
 * \param max_length die max. frei Plätze in USART-FIFO
 * 
 * \return bool immer true, da es bisher nicht weiter betrachtet wird
 */
static bool usartCallbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length, uint8_t max_length){
	if (comUnit_tx.toHandleBytes == 0){//Wenn keine Daten mehr zu senden
		comUnit_tx.strPtr = 0;//Reset des Zeigers vom Buffer
	} else{ //sonst weiter ausfüllen die Daten in USART-FIFO
		*data = (uint8_t*)(&(comUnit_tx.data[comUnit_tx.strPtr]));
		//max. Bytes gleich die leere Stelle in FIFO
		if (comUnit_tx.toHandleBytes < max_length){
			*length = (comUnit_tx.toHandleBytes);
			comUnit_tx.toHandleBytes = 0;
			comUnit_tx.strPtr = 0;
		} else{
			*length = max_length;
			comUnit_tx.toHandleBytes -= max_length;
			comUnit_tx.strPtr += max_length;
		}
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
static bool usartCallbackRx(uint8_t adress, uint8_t data[], uint8_t length){
	//Empfangen der Daten in USART-FIFO und Kopieren in das Zwischenbuffer
	comUnit_rx.toHandleBytes -= length;
	memcpy((uint8_t*)&(comUnit_rx.data[comUnit_rx.strPtr]),data,length);
	comUnit_rx.strPtr += length;
	return true;
}

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
	if(length>uu.txObj.txLenMax){
		result = DATA_INVALID;
	} else if (data==NULL){
		result = NULL_POINTER;
	} else if (!uu.statusObj.initState){
		result = NO_INIT;
	} else{
		while(uu.txObj.toTxByte!=0);
		uu.statusObj.send = 1;
		uu.txObj.toTxByte = length;
		memcpy((uint8_t*)uu.txObj.txBuffer,data,length);
		if (length<=uu.txObj.usartFIFOMax){
			uu.txObj.toTxByte = 0;
			uu.txObj.strPtr += length;
			USART_send_Array(uu.statusObj.usart, 0, (uint8_t*)(uu.txObj.txBuffer), length);
		} else{
			uu.txObj.toTxByte -= uu.txObj.usartFIFOMax;
			uu.txObj.strPtr += uu.txObj.usartFIFOMax;
			USART_send_Array(uu.statusObj.usart, 0, (uint8_t*)(uu.txObj.txBuffer), uu.txObj.usartFIFOMax);
		}
		result = waitWithBreak(BYTE_RECEIVE_TIME_US*length*5000,(uint8_t*)&(uu.txObj.toTxByte),0);
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
	} else if ((length>(uu.rxObj.rxLenMax))||(length==0)){
		result = DATA_INVALID;
	} else{
		uint8_t usartNo = uu.statusObj.usart;
		uu.rxObj.toRxByte = length;
		USART_set_Bytes_to_receive(usartNo,length);
		uint8_t checkTimeout = waitWithBreak(BYTE_RECEIVE_TIME_US*length*5000,(uint8_t*)&(uu.rxObj.toRxByte),0);//magic number:Anzahl der Zyklen pro sekunden
		if (!checkTimeout){
			memcpy((uint8_t*)data,(uint8_t*)uu.rxObj.rxBuffer,length);
		} else{
			result = checkTimeout;
		}
		uu.rxObj.strPtr = 0;
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
	uu.statusObj.usart = config.usartNo;
	uu.statusObj.initState = (!usartInit)?1:0;
	USART_set_send_Array_callback_fnc(config.usartNo,&usartCallbackTx);
	USART_set_receive_Array_callback_fnc(config.usartNo,&usartCallbackRx);
	result = usartInit?NO_ERROR:PROCESS_FAIL;
	return result;
}

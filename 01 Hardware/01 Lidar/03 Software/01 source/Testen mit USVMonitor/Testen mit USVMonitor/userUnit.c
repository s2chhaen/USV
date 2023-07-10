/*
 * userUnit.c
 *
 * Created: 7/10/2023 4:59:46 AM
 * Author: Thach
 */ 

#include "userUnit.h"

userUnit_t uu={
	//rxObj
	.rxObj.toRxByte = 0,
	.rxObj.strPtr = 0,
	.rxObj.rxLenMax = RX_BUFFER_LEN,
	.rxObj.usartFIFOMax = _FIFO_max_def - 1,
	//txObj
	.txObj.toTxByte = 0,
	.txObj.strPtr = 0,
	.txObj.txLenMax = TX_BUFFER_LEN,
	.txObj.usartFIFOMax = _FIFO_max_def - 1,
	//statusObj
	.statusObj.usart = 0,
	.statusObj.initState = 0,
};

static bool usartCallbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length, uint8_t max_length){
	static uint8_t firstCall = 1;
	if (firstCall){
		if (uu.txObj.toTxByte<=uu.rxObj.usartFIFOMax){
			uu.txObj.toTxByte = 0;
		} else{
			uu.txObj.toTxByte -= uu.rxObj.usartFIFOMax;
			uu.txObj.strPtr += uu.rxObj.usartFIFOMax;
		}
		firstCall--;
	}
	
	if (uu.txObj.toTxByte){
		uint8_t *temp1 = (uint8_t*)&(uu.txObj.txBuffer[uu.txObj.strPtr]);
		data = &temp1;
		if (uu.txObj.toTxByte<max_length){
			//Dieser Fall ist nur dafür, wenn man High and Low-Byte von TXDATA von USART verwendet, sonst uu.txObj.toTxByte>=max_length immer
			*length = (uint8_t)uu.txObj.toTxByte;
			uu.txObj.toTxByte = 0;
		} else{
			*length = (uint8_t)max_length;
			uu.txObj.toTxByte-=max_length;
			uu.txObj.strPtr +=max_length;
		}
	} else{ //wenn kein Byte mehr in txBuffer zur Füllung in USART-FIFO
		uu.txObj.strPtr = 0;//Reset des Zeigers vom Buffer
	}
	
	//wenn usartFIFO total leer ist
	if (max_length==uu.rxObj.usartFIFOMax){
		firstCall = 1;
	}
	return true;
}

static bool usartCallbackRx(uint8_t adress, uint8_t data[], uint8_t length){
	return true;
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
		memcpy((uint8_t*)uu.txObj.txBuffer,data,length);
		uu.txObj.toTxByte = length;
		if (length<=uu.rxObj.usartFIFOMax){
			USART_send_Array(uu.statusObj.usart, 0, (uint8_t*)(&(uu.txObj.txBuffer)), length);
		} else{
			USART_send_Array(uu.statusObj.usart, 0, (uint8_t*)(&(uu.txObj.txBuffer)), uu.rxObj.usartFIFOMax);
		}
	}
	return result;
}

uint8_t usartDataRx(uint8_t* data, uint16_t* length){
	uint8_t result = NO_ERROR;
	return result;
}



uint8_t initUserUnit(usartConfig_t config){
	uint8_t result = NO_ERROR;
	uint8_t usartInit = USART_init(config.usartNo,config.baudrate, config.usartChSize, config.parity, \
									config.stopbit, config.sync, config.mpcm,config.address, config.portMux);
	uu.statusObj.usart = config.usartNo;
	uu.statusObj.initState = usartInit?NO_ERROR:1;
	USART_set_send_Array_callback_fnc(config.usartNo,&usartCallbackTx);
	USART_set_receive_Array_callback_fnc(config.usartNo,&usartCallbackRx);
	USART_set_Bytes_to_receive(config.usartNo, config.rxByte);
	result = usartInit?NO_ERROR:PROCESS_FAIL;
	return result;
}

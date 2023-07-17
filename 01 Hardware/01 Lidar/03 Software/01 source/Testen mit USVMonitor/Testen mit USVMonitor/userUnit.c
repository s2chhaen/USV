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
	.statusObj.send = 0,
	.statusObj.receive = 0
};

static bool usartCallbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length, uint8_t max_length){
	if (uu.txObj.toTxByte == 0){
		uu.statusObj.send = 0;
		uu.txObj.strPtr = 0;//Reset des Zeigers vom Buffer
	} else{
		*data = (uint8_t*)(&(uu.txObj.txBuffer[uu.txObj.strPtr]));
		if (uu.txObj.toTxByte<max_length){
			*length = (uu.txObj.toTxByte);
			uu.txObj.toTxByte = 0;
			uu.txObj.strPtr = 0;
		} else{
			*length = max_length;
			uu.txObj.toTxByte -= max_length;
			uu.txObj.strPtr += max_length;
		}
	}
	return true;
}

static bool usartCallbackRx(uint8_t adress, uint8_t data[], uint8_t length){
	uu.rxObj.toRxByte -= length;
	memcpy((uint8_t*)&(uu.rxObj.rxBuffer[uu.rxObj.strPtr]),data,length);
	uu.rxObj.strPtr += length;
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
		//while(uu.txObj.toTxByte);
	}
	return result;
}

uint8_t usartDataRx(uint8_t* data, uint8_t length){
	uint8_t result = NO_ERROR;
	if (data==NULL){
		result = NULL_POINTER;
	} else if ((length>(uu.rxObj.rxLenMax))||(length==0)){
		result = DATA_INVALID;
	} else{
		uint8_t usartNo = uu.statusObj.usart;
		uu.rxObj.toRxByte = length;
		USART_set_Bytes_to_receive(usartNo,length);
		//waitCycle(length*BYTE_RECEIVE_TIME_US);
		while (uu.rxObj.toRxByte);
		memcpy((uint8_t*)data,(uint8_t*)uu.rxObj.rxBuffer,length);
		uu.rxObj.strPtr = 0;
	}
	return result;
}

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

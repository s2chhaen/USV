/*
 * slave_device.c
 *
 * Created: 21.06.2023 13:01:53
 * Author: hathach
 */ 

#include "slave_device.h"

slaveDevice_t obj ={.initState=OFF,.crcActive=OFF,.rxTime=0,.txTime=0};

processResult_t initDev(uint16_t rxLength, uint16_t txLength,uint8_t USARTnumber, uint32_t baudrate,\
USART_CHSIZE_t bits, USART_PMODE_t parity,USART_SBMODE_t stopbit, \
bool sync, bool MPCM, uint8_t address, PORTMUX_USARTx_t PortMux){
	uint8_t result;
	if((rxLength!=0)&&(txLength!=0)){
		if(obj.initState==ON){
			free((void*)(obj.rxBuffer));
			free((void*)(obj.txBuffer));
		} else{
			obj.initState=ON;
		}
		obj.uart = USARTnumber;
		obj.rxLenMax = rxLength;
		obj.txLenMax = txLength;
		obj.rxBuffer = (uint8_t *)malloc(rxLength*sizeof(uint8_t));
		obj.txBuffer = (uint8_t *)malloc(txLength*sizeof(uint8_t));
		bool slaveUartInit = USART_init(USARTnumber,baudrate, bits, parity, \
										 stopbit, sync, MPCM,address, PortMux);
		USART_set_send_Array_callback_fnc(USARTnumber,&callbackTx);
		USART_set_receive_Array_callback_fnc(USARTnumber,&callbackRx);
		USART_set_Bytes_to_receive(iUSART1,32);
		result = (!slaveUartInit)?NO_ERROR:PROCESS_FAIL;
	}else{
		result = LENGTH_INVALID;
	}
	return result;
}

uint8_t DataTx(uint8_t length, uint8_t* data){
	uint8_t result = NO_ERROR;
	if(length>obj.txLenMax){
		result = LENGTH_EXCESS;
	} else if(data==NULL){
		result = NULL_POINTER;
	} else{
		obj.txTime = (length/obj.txLenMax) + ((length%obj.txLenMax)?1:0);
		uint16_t toTxByte = length;
		uint16_t position = 0; 
		while(obj.txTime){
			if(toTxByte<32){
				USART_send_Array(iUSART1, 0, &(obj.txBuffer[position]), toTxByte);
				position = 0;
				//TODO noch zu schreiben
			} else{
				USART_send_Array(iUSART1, 0, &(obj.txBuffer[position]), 32);
				position+=32;
				toTxByte-=32;
			}
		}
	}
	return result;
}

processResult_t resetDev(){
	processResult_t result= NO_ERROR;
	obj.rxLenMax=0;
	obj.txLenMax=0;
	free((void*)(obj.rxBuffer));
	free((void*)(obj.txBuffer));
	return result;
}

bool callbackRx(uint8_t adress, uint8_t data[], uint8_t length){
	return true;
}

bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length){
	obj.txTime--;
	//obj.toTxByte-=32;
	//obj.toTxByte = (obj.toTxByte<=0)?0:obj.toTxByte;
	return true;
}
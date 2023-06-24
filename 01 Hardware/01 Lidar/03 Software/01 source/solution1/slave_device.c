/*
 * slave_device.c
 *
 * Created: 21.06.2023 13:01:53
 * Author: hathach
 * Version: 0.1
 */ 

#include "slave_device.h"

slaveDevice_t obj ={
	.initState=OFF,
	.crcActive=OFF,
	.rxTime=0,
	.txTime=0,
	.txPtrPosition=0
};

#ifdef VERSION_1
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length) __attribute__((deprecated("funtcion is in debug time")));
#endif

processResult_t dataTx(uint8_t* data,uint16_t length){
	uint8_t result = NO_ERROR;
	if(length>obj.txLenMax){
		result = LENGTH_EXCESS;
		} else if (data==NULL){
		result = NULL_POINTER;
		} else{
		for (int i = 0;i<length;i++){
			obj.txBuffer[i] = data[i];
		}
		obj.toTxByte = length;
		obj.txTime = (length/MAX_BYTE_SEND) + ((length%MAX_BYTE_SEND)?1:0);
		obj.txTime--;
		if(obj.toTxByte<MAX_BYTE_SEND){
			obj.toTxByte = 0;
			USART_send_Array(obj.uart, 0, (uint8_t*)(&(obj.txBuffer[0])), length);
			} else{
			obj.toTxByte-=MAX_BYTE_SEND;
			obj.txPtrPosition+=MAX_BYTE_SEND;
			USART_send_Array(obj.uart, 0, (uint8_t*)(&(obj.txBuffer[0])), MAX_BYTE_SEND);
			
		}
	}
	return result;
}

#ifdef VERSION_1
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length){
	if (obj.txTime!=0)
	{
		uint16_t temp1 = 0;
		uint16_t temp2 = 0;
		if (obj.toTxByte<MAX_BYTE_SEND)
		{
			temp1 = obj.toTxByte;
			temp2 = obj.txPtrPosition;
			obj.toTxByte = 0;
			obj.txPtrPosition = 0;
			obj.txTime=0;
			USART_send_Array(obj.uart, 0, (uint8_t*)(&(obj.txBuffer[temp2])), temp1);//Nach dieser Funktion, keine Code mehr schreiben, sonst Stack-overflow-Fehler
		} else
		{
			temp2 = obj.txPtrPosition;
			obj.toTxByte-=MAX_BYTE_SEND;
			obj.txPtrPosition+=MAX_BYTE_SEND;
			obj.txTime--;
			USART_send_Array(obj.uart, 0, (uint8_t*)(&(obj.txBuffer[temp2])), MAX_BYTE_SEND);
		}
	}
	
	return true;
}
#endif

#ifndef VERSION_1
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length){
	uint8_t lengthForSend = (max_length>MAX_BYTE_SEND)?MAX_BYTE_SEND:max_length;
	uint8_t sizeOfArray = 1;
	uint8_t** temp = (uint8_t**)malloc(sizeOfArray*sizeof(uint8_t*));
	temp[1] = (uint8_t*)&(obj.txBuffer[obj.txPtrPosition]);
	data = temp;
	if (obj.toTxByte<lengthForSend){
		*length = (uint8_t)(obj.toTxByte);
		obj.toTxByte = 0;
		obj.txPtrPosition = 0;
		obj.txTime=0;
	} else{
		*length = (uint8_t)lengthForSend;
		obj.toTxByte-=lengthForSend;
		obj.txPtrPosition+=MAX_BYTE_SEND;
		obj.txTime--;
	}
	return true;
}
#endif

static bool callbackRx(uint8_t adress, uint8_t data[], uint8_t length){
	obj.rxTime--;
	return false;
}

processResult_t initDev(uint16_t rxLength, uint16_t txLength,uint8_t USARTnumber, uint32_t baudrate,USART_CHSIZE_t bits, USART_PMODE_t parity,USART_SBMODE_t stopbit,bool sync, bool MPCM, uint8_t address, PORTMUX_USARTx_t PortMux){
	uint8_t result;
	if((rxLength!=0)&&(txLength!=0)){
		if(obj.initState==ON){
			#ifdef SLAVE_DYNAMIC
			free((void*)(obj.rxBuffer));
			free((void*)(obj.txBuffer));
			#endif
			} else{
			obj.initState=ON;
		}
		//set maximal length of rx- and tx-Buffer
		obj.uart = USARTnumber;
		#ifdef SLAVE_DYNAMIC
		obj.rxLenMax = rxLength;
		obj.txLenMax = txLength;
		obj.rxBuffer = (uint8_t *)malloc(rxLength*sizeof(uint8_t));
		obj.txBuffer = (uint8_t *)malloc(txLength*sizeof(uint8_t));
		#else
		obj.rxLenMax = 900;
		obj.txLenMax = 900;
		#endif
		bool slaveUartInit = USART_init(USARTnumber,baudrate, bits, parity, \
		stopbit, sync, MPCM,address, PortMux);
		USART_set_send_Array_callback_fnc(USARTnumber,&callbackTx);
		//USART_set_receive_Array_callback_fnc(USARTnumber,&callbackRx);
		USART_set_Bytes_to_receive(USARTnumber,32);
		result = (!slaveUartInit)?NO_ERROR:PROCESS_FAIL;
		}else{
		result = LENGTH_INVALID;
	}
	return result;
}



processResult_t dataRx(uint16_t length, uint8_t* data){
	processResult_t result = NO_ERROR;
	if(data == NULL){
		result = NULL_POINTER;
		} else if(length > obj.rxLenMax){
		result = LENGTH_EXCESS;
		} else{
		//obj.rxTime = (length/32) + ((length%32)?1:0);
		uint16_t position = 0;
		uint16_t rxLength = 0;
		uint8_t address = 0;
		uint8_t rxDatalength = 0;
		volatile uint8_t *temp = (uint8_t*)&(obj.rxBuffer[position]);
		volatile uint8_t *temp1[] = {temp};
		result=!USART_receive_Array(obj.uart,&address,(uint8_t**)temp1,obj.rxLenMax,&rxDatalength);
		while((result)&&(rxDatalength)){
			rxLength+=rxDatalength;
			rxDatalength = 0;
			position+=32;
			if((rxLength>obj.rxLenMax)||(rxLength>length)){
				result = LENGTH_EXCESS;
				break;
			}
			temp = (uint8_t*)&(obj.rxBuffer[position]);
			result=!USART_receive_Array(obj.uart,&address,(uint8_t**)temp1,obj.rxLenMax,&rxDatalength);
		}
		if(!result){
			for (int i = 0;i<length;i++){
				data[i] = obj.rxBuffer[i];
			}
		}
	}
	return result;
}

processResult_t resetDev(){
	processResult_t result= NO_ERROR;
	obj.rxLenMax=0;
	obj.txLenMax=0;
	#ifdef SLAVE_DYNAMIC
	free((void*)(obj.rxBuffer));
	free((void*)(obj.txBuffer));
	#endif
	return result;
}



	

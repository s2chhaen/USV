/*
 * slave_device.c
 *
 * Created: 21.06.2023 13:01:53
 * Author: hathach
 * Version: 0.1
 */ 

#include "slave_device.h"

volatile slaveDevice_t obj ={.initState=OFF,.crcActive=OFF,.rxTime=0,.txTime=0};
volatile uint8_t send = 1;

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
		//set maximal length of rx- and tx-Buffer
		obj.uart = USARTnumber;
		obj.rxLenMax = rxLength;
		obj.txLenMax = txLength;
		obj.rxBuffer = (uint8_t *)malloc(rxLength*sizeof(uint8_t));
		obj.txBuffer = (uint8_t *)malloc(txLength*sizeof(uint8_t));
		bool slaveUartInit = USART_init(USARTnumber,baudrate, bits, parity, \
										 stopbit, sync, MPCM,address, PortMux);
		USART_set_send_Array_callback_fnc(USARTnumber,&callbackTx);
		USART_set_receive_Array_callback_fnc(USARTnumber,&callbackRx);
		USART_set_Bytes_to_receive(USARTnumber,32);
		result = (!slaveUartInit)?NO_ERROR:PROCESS_FAIL;
	}else{
		result = LENGTH_INVALID;
	}
	return result;
}

processResult_t dataTx(uint16_t length, uint8_t* data){
	uint8_t result = NO_ERROR;
	if(length>obj.txLenMax){
		result = LENGTH_EXCESS;
	} else if(data==NULL){
		result = NULL_POINTER;
	} else{
		for (int i = 1;i<length;i++){
			obj.txBuffer[i] = data[i];
		}
		obj.txTime = (length/32) + ((length%32)?1:0);
		uint16_t toTxByte = length;
		uint16_t position = 0; 
		//while(obj.txTime){
			if(toTxByte<32){
				send=1;
				USART_send_Array(obj.uart, 0, (uint8_t*)(&(obj.txBuffer[position])), toTxByte);
				while(send!=0);
			} else{
				USART_send_Array(obj.uart, 0, (uint8_t*)(&(obj.txBuffer[position])), 32);
				position+=32;
				toTxByte-=32;
			}
		//}
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
	free((void*)(obj.rxBuffer));
	free((void*)(obj.txBuffer));
	return result;
}

bool callbackRx(uint8_t adress, uint8_t data[], uint8_t length){
	obj.rxTime--;
	return true;
}

bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length){
	obj.txTime--;
	send = 0;
	return true;
}
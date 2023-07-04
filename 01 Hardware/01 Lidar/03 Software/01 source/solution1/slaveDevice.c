/*
 * slave_device.c
 *
 * Created: 21.06.2023 13:01:53
 * Author: hathach
 * Version: 0.1
 */ 

#include "slaveDevice.h"

//Refaktorisierung in Bearbeitung

slaveDevice_t obj ={
	//preinit rxObj
	.rxObj.toRxByte = 0,
	.rxObj.strReadPtr = 0,
	.rxObj.rxLenMax=RX_BUFFER_LEN,
	.rxObj.fifoLenMax=NO_OF_RX_BUFFER,
	.rxObj.readFIFOPtr = 0,
	.rxObj.writeFIFOPtr = 0,
	//preinit txObj
	.txObj.toTxByte = 0,
	.txObj.strReadPtr = 0,
	.txObj.txLenMax = TX_BUFFER_LEN,
	//preinit status
	.statusObj.uart = 0,
	.statusObj.initState= 0,
	.statusObj.crcActive= 0,
	.statusObj.rxBufferState = EMPTY,
	.statusObj.msgAvaliable = 0
};
//make a lastRxTime here
static uint8_t temp3 = 0;

#define  VERSION_3 1

#ifdef VERSION_1
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length) /*__attribute__((deprecated("funtcion is in debug time")))*/;
#endif

#ifdef VERSION_2
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length) /*__attribute__((deprecated("funtcion is in debug time")))*/;
#endif

#ifdef VERSION_3
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length) /*__attribute__((deprecated("funtcion is in debug time")))*/;
#endif

processResult_t dataTx(uint8_t* data,uint16_t length){
	uint8_t result = NO_ERROR;
	if(length>obj.txObj.txLenMax){
		result = LENGTH_EXCESS;
	} else if (data==NULL){
		result = NULL_POINTER;
	} else{
		//TODO copy data into txBuffer
		obj.toTxByte = length;
#ifdef VERSION_1
		obj.txTime = (length/MAX_BYTE_SEND) + ((length%MAX_BYTE_SEND)?1:0);
		obj.txTime--;
#endif
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

processResult_t dataRx(uint8_t* data, uint16_t length){
	processResult_t result = NO_ERROR;
	if(data == NULL){
		result = NULL_POINTER;
	} else if(length > obj.rxLenMax){
		result = LENGTH_EXCESS;
	} else if (length<obj.rxBuffer){
		result = LENGTH_EXCESS;
	}else{
		uint8_t adr=0;
		uint8_t temp4 =0;
		USART_receive_Array(obj.uart,&adr,&obj.rxBuffer,900,(uint8_t*)&temp4);
		obj.rxPtrPosition += temp4;
	}
	return result;
}

#ifdef VERSION_1
//zu sendende Byte wird konstant in FIFO jedes Interrupt hinzugefügt 
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


#ifdef VERSION_2
//wie oben aber lässt es die Sendung für TX_Callback function in Interrupt
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length){
	uint8_t lengthForSend = (max_length>MAX_BYTE_SEND)?MAX_BYTE_SEND:max_length;
	uint8_t sizeOfArray = 1;
	uint8_t** temp = (uint8_t**)malloc(sizeOfArray*sizeof(uint8_t*));
	temp[0] = (uint8_t*)&(obj.txBuffer[obj.txPtrPosition]);
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

#ifdef VERSION_3
//zu sendende Byte wird angepasst mit den freien Stellen in FIFO. Die Sendung wird hier durchgefuehrt
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length){
	temp3 = max_length;
	uint8_t sendByte = (max_length>MAX_BYTE_SEND)?MAX_BYTE_SEND:max_length;
	if (obj.toTxByte!=0)
	{
		uint16_t temp1 = 0;
		uint16_t temp2 = 0;
		if (obj.toTxByte<sendByte)
		{
			temp1 = obj.toTxByte;
			temp2 = obj.txPtrPosition;
			obj.toTxByte = 0;
			obj.txPtrPosition = 0;
			USART_send_Array(obj.uart, 0, (uint8_t*)(&(obj.txBuffer[temp2])), temp1);
		} else
		{
			temp2 = obj.txPtrPosition;
			obj.toTxByte-=sendByte;
			obj.txPtrPosition+=sendByte;
			USART_send_Array(obj.uart, 0, (uint8_t*)(&(obj.txBuffer[temp2])), MAX_BYTE_SEND);
		}
	}
	return true;
}
#endif

static bool callbackRx(uint8_t adress, uint8_t data[], uint8_t length){
	for (uint8_t i = 0;i<length;i++){
		obj.rxBuffer[obj.rxPtrPosition] = data[i];
	}
	obj.rxPtrPosition+=length;//TODO to correct later
	//get last time in system
	//get time now in system too
	//time interval = now - last
	//if time interval > 10*(Byte-sending-time) then set newCommando
	//Copy to IncommingCommand-Array (will be made later) and reset obj.rxPtrPosition
	return false;
}

//Refaktorisierung fertig, dynamische Eigenschaft wird vorlaeufig deaktiviert
processResult_t initDev(uint16_t rxLength, uint16_t txLength,uint8_t USARTnumber, uint32_t baudrate,USART_CHSIZE_t bits, USART_PMODE_t parity,USART_SBMODE_t stopbit,bool sync, bool MPCM, uint8_t address, PORTMUX_USARTx_t PortMux){
	uint8_t result;
	//initialisieren der zwei rx- und tx-Buffer
	memset((int**)obj.rxObj.rxBuffer,0,sizeof(obj.rxObj.rxBuffer));
	memset((int*)obj.txObj.txBuffer,0,sizeof(obj.txObj.txBuffer));
	obj.statusObj.uart = USARTnumber;
	bool slaveUartInit = USART_init(USARTnumber,baudrate, bits, parity, stopbit, sync, MPCM,address, PortMux);
	USART_set_send_Array_callback_fnc(USARTnumber,&callbackTx);
	USART_set_Bytes_to_receive(USARTnumber,31);
	USART_set_receive_Array_callback_fnc(USARTnumber,&callbackRx);
	obj.statusObj.initState = (!slaveUartInit)?1:0;
	result = (!slaveUartInit)?NO_ERROR:PROCESS_FAIL;
	return result;
}

void deinitDev(){
	USART_set_send_Array_callback_fnc(obj.statusObj.uart,NULL);
	USART_set_Bytes_to_receive(obj.statusObj.uart,0);
	USART_set_receive_Array_callback_fnc(obj.statusObj.uart,NULL);
	obj.statusObj.uart = 0;
	obj.statusObj.initState = 0;
	
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



	

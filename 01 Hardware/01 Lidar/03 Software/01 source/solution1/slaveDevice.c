/*
 * slave_device.c
 *
 * Created: 21.06.2023 13:01:53
 * Author: hathach
 * Version: 0.1
 */ 

#include "slaveDevice.h"

//Refaktorisierung in Bearbeitung

//preinit von slaveDevice
slaveDevice_t obj ={
	// rxObj
	.rxObj.toRxByte = {0},
	.rxObj.rxBuffer = {{0}},
	.rxObj.strReadPtr = 0,
	.rxObj.rxLenMax=RX_BUFFER_LEN,
	.rxObj.fifoLenMax=NO_OF_RX_BUFFER,
	.rxObj.readFIFOPtr = 0,
	.rxObj.writeFIFOPtr = 0,
	//txObj
	.txObj.toTxByte = 0,
	.txObj.strReadPtr = 0,
	.txObj.txLenMax = TX_BUFFER_LEN,
	//statusObj
	.statusObj.uart = 0,
	.statusObj.initState= 0,
	.statusObj.crcActive= 0,
	.statusObj.rxBufferState = NO_FULL,
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
		while(obj.txObj.toTxByte);//warte bis zum Buffer leer ist
		for (uint16_t i = 0;i<length;i++)
		{
			obj.txObj.txBuffer[i]=data[i];
		}
		obj.txObj.toTxByte = length;
#ifdef VERSION_1
		obj.txTime = (length/MAX_BYTE_SEND) + ((length%MAX_BYTE_SEND)?1:0);
		obj.txTime--;
#endif
		if(obj.txObj.toTxByte<MAX_BYTE_SEND){
			obj.txObj.toTxByte = 0;
			USART_send_Array(obj.statusObj.uart, 0, (uint8_t*)(&(obj.txObj.txBuffer[0])), length);
		} else{
			obj.txObj.toTxByte-=MAX_BYTE_SEND;
			obj.txObj.strReadPtr+=MAX_BYTE_SEND;
			USART_send_Array(obj.statusObj.uart, 0, (uint8_t*)(&(obj.txObj.txBuffer[0])), MAX_BYTE_SEND);	
		}
	}
	return result;
}

//Daten wird automatisch empfangen, diese ist zum Lesen der Daten in Buffer
//die Laenge von Input-Daten muss genug fuer die Empfangen Daten sein, selbst auswaehlen
processResult_t dataRx(uint8_t* data, uint16_t* length){
	processResult_t result = NO_ERROR;
	if(data == NULL){//checken null-pointer
		result = NULL_POINTER;
	} else if((*length) > obj.rxObj.rxLenMax){//checken die Laenge
		result = LENGTH_EXCESS;
	} else if ((obj.rxObj.readFIFOPtr == obj.rxObj.writeFIFOPtr)&&(obj.statusObj.rxBufferState == NO_FULL)){//checken leer
		result = FIFO_EMPTY;
	}else{
		for (uint16_t i = 0;i<(*length);i++){
			data[i] = obj.rxObj.rxBuffer[obj.rxObj.readFIFOPtr][i];
		}
		*length = obj.rxObj.toRxByte[obj.rxObj.readFIFOPtr];
		if(obj.statusObj.rxBufferState == FULL){
			obj.statusObj.rxBufferState = NO_FULL;
		}
		obj.rxObj.readFIFOPtr = (obj.rxObj.readFIFOPtr+1)%NO_OF_RX_BUFFER;
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
	if (obj.txObj.toTxByte!=0)
	{
		uint16_t temp1 = 0;
		uint16_t temp2 = 0;
		if (obj.txObj.toTxByte<sendByte)
		{
			temp1 = obj.txObj.toTxByte;
			temp2 = obj.txObj.strReadPtr;
			obj.txObj.toTxByte = 0;
			obj.txObj.strReadPtr = 0;
			USART_send_Array(obj.statusObj.uart, 0, (uint8_t*)(&(obj.txObj.txBuffer[temp2])), temp1);
		} else
		{
			temp2 = obj.txObj.strReadPtr;
			obj.txObj.toTxByte-=sendByte;
			obj.txObj.strReadPtr+=sendByte;
			USART_send_Array(obj.statusObj.uart, 0, (uint8_t*)(&(obj.txObj.txBuffer[temp2])), sendByte);
		}
	}
	return true;
}
#endif

static bool callbackRx(uint8_t adress, uint8_t data[], uint8_t length){
#ifdef ACTIVE_USART_WATCHER
	uint32_t remainTime = getUsartWatcherTimeout(obj.statusObj.uart);
#else
	uint32_t remainTime = 1;
#endif
	//checken, ob timeOut erloest wird oder nicht
	if (remainTime)
	{
		/*wenn die zu empfangenden Daten laenge als die Laenge von rxBuffer,kopieren bis zum Buffer voll, 
		  die uebrigen werden in neues Buffer kopiert*/
		if ((obj.rxObj.strReadPtr+length)<RX_BUFFER_LEN)
		{
			for (uint8_t i = 0;i<length;i++){//TODO benutzt memset hier
				obj.rxObj.rxBuffer[obj.rxObj.writeFIFOPtr][i+obj.rxObj.strReadPtr] = data[i];
			}
			obj.rxObj.strReadPtr+=length;
			obj.rxObj.toRxByte[obj.rxObj.writeFIFOPtr]=obj.rxObj.strReadPtr;
		}else
		{
			length = RX_BUFFER_LEN - obj.rxObj.strReadPtr;
			for (uint8_t i = 0;i<length;i++){
				obj.rxObj.rxBuffer[obj.rxObj.writeFIFOPtr][i+obj.rxObj.strReadPtr] = data[i];
			}
			obj.rxObj.toRxByte[obj.rxObj.writeFIFOPtr] = RX_BUFFER_LEN;
			obj.rxObj.strReadPtr = 0;
			obj.rxObj.writeFIFOPtr=(obj.rxObj.writeFIFOPtr+1)%NO_OF_RX_BUFFER;
			if (obj.rxObj.writeFIFOPtr != obj.rxObj.readFIFOPtr){
				for (uint8_t i = 0;i<length;i++){
					obj.rxObj.rxBuffer[obj.rxObj.writeFIFOPtr][i+obj.rxObj.strReadPtr] = data[i];
				}
				obj.rxObj.strReadPtr+=length;
				obj.rxObj.toRxByte[obj.rxObj.writeFIFOPtr]=obj.rxObj.strReadPtr;
			} else{
				obj.statusObj.rxBufferState=FULL;
			}
		} 
	} else{
		obj.rxObj.strReadPtr = 0;
		obj.rxObj.writeFIFOPtr=(obj.rxObj.writeFIFOPtr+1)%NO_OF_RX_BUFFER;
		//wenn FIFO nicht voll, dann schreibt, sonst nicht
		if (obj.rxObj.writeFIFOPtr != obj.rxObj.readFIFOPtr){
			for (uint8_t i = 0; i<length; i++){
				obj.rxObj.rxBuffer[obj.rxObj.writeFIFOPtr][i+obj.rxObj.strReadPtr] = data[i];
			}
			obj.rxObj.strReadPtr = (obj.rxObj.strReadPtr+length);
			obj.rxObj.toRxByte[obj.rxObj.writeFIFOPtr]=obj.rxObj.strReadPtr;
		} else{
			obj.statusObj.rxBufferState=FULL;
		}
	}
#ifdef ACTIVE_USART_WATCHER
	setUsartWatcherTimeout(obj.statusObj.uart,(USART_TIME_PRO_BYTE_US*3));
#endif
	return false;
}

//Refaktorisierung fertig, dynamische Eigenschaft wird vorlaeufig deaktiviert
processResult_t initDev(uint16_t rxLength, uint16_t txLength,uint8_t USARTnumber, uint32_t baudrate,USART_CHSIZE_t bits, USART_PMODE_t parity,USART_SBMODE_t stopbit,bool sync, bool MPCM, uint8_t address, PORTMUX_USARTx_t PortMux){
	uint8_t result = NO_ERROR;
	//initialisieren der zwei rx- und tx-Buffer
	memset((int**)obj.rxObj.rxBuffer,0,sizeof(obj.rxObj.rxBuffer));
	memset((int*)obj.txObj.txBuffer,0,sizeof(obj.txObj.txBuffer));
	memset((int*)obj.rxObj.toRxByte,0,sizeof(obj.rxObj.toRxByte));
	//obj.dataTx_p = dataTx;
	//obj.dataRx_p = dataRx;
	obj.statusObj.uart = USARTnumber;
	bool slaveUartInit = USART_init(USARTnumber,baudrate, bits, parity, stopbit, sync, MPCM,address, PortMux);
	USART_set_send_Array_callback_fnc(USARTnumber,&callbackTx);
#ifdef ACTIVE_USART_WATCHER
	USART_set_Bytes_to_receive(USARTnumber,1);
#else
	USART_set_Bytes_to_receive(USARTnumber,MAX_BYTE_SEND); 
#endif
	
	USART_set_receive_Array_callback_fnc(USARTnumber,&callbackRx);
	obj.statusObj.initState = (!slaveUartInit)?1:0;
#ifdef ACTIVE_USART_WATCHER
	setUsartWatcherTimeout(obj.statusObj.uart,(USART_TIME_PRO_BYTE_US*3));
#endif
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

void echoTest(){
	volatile uint8_t data[50]={0};
	volatile uint16_t rxLength = sizeof(data)/sizeof(uint8_t);
	volatile bool checkReceive = dataRx((uint8_t*)data,(uint16_t*)&rxLength);
	volatile uint16_t temp1 = 48;
	volatile uint16_t temp2 = 48;
	if(checkReceive==NO_ERROR){
		dataTx((uint8_t*)data,(uint16_t)rxLength);
		waitUs(1000000);
		temp1+=rxLength/10;
		temp2+=rxLength%10;
		data[0]=temp1;
		data[1]=temp2;
		dataTx((uint8_t*)data,2);
	}
}



	

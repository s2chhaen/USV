/*
 * slave_device.c
 *
 * Created: 21.06.2023 13:01:53
 * Author: Thach
 * Version: 1.00
 */ 

#include "slaveDevice.h"

//preinit von slaveDevice
slaveDevice_t obj ={
	// rxObj
	.rxObj.rxByte = {0},
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
	.statusObj.rxFIFOState = EMPTY,
	.statusObj.nextPhase = 0
};

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
		memcpy((uint8_t*)obj.txObj.txBuffer,data,length);
		if(length<MAX_BYTE_SEND){
			obj.txObj.toTxByte = 0;
			USART_send_Array(obj.statusObj.uart, 0, (uint8_t*)(&(obj.txObj.txBuffer[0])), length);
		} else{
			obj.txObj.toTxByte = length;
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
	} else if (obj.statusObj.rxFIFOState == EMPTY){//checken leer
		result = FIFO_EMPTY;
	}else{
#ifdef ACTIVE_USART_WATCHER
		while (getUsartWatcherTimeout(obj.statusObj.uart)!=0);//Warte bis rxBuffer total geschrieben wird
#else
		while (obj.statusObj.rxFIFOState==EMPTY);
#endif
		*length = obj.rxObj.rxByte[obj.rxObj.readFIFOPtr];
		memcpy(data,(uint8_t*)&(obj.rxObj.rxBuffer[obj.rxObj.readFIFOPtr]),(*length));
		obj.rxObj.rxByte[obj.rxObj.readFIFOPtr]=0;//Nach dem Lesen wird diese Byte als gelesen markiert
		obj.rxObj.readFIFOPtr = (obj.rxObj.readFIFOPtr+1)%NO_OF_RX_BUFFER;
		obj.statusObj.rxFIFOState = (obj.rxObj.readFIFOPtr == obj.rxObj.writeFIFOPtr)?EMPTY:FILLED;
	}
	return result;
}

#ifdef VERSION_1
//zu sendende Byte wird konstant in FIFO jedes Interrupt hinzugefügt. Die weitere Sendung wird hier durchgefuehrt
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length){
	if (obj.txObj.toTxByte!=0){
		uint16_t temp1 = 0;
		uint16_t temp2 = 0;
		if (obj.txObj.toTxByte<MAX_BYTE_SEND){
			temp1 = obj.txObj.toTxByte;
			temp2 = obj.txObj.strReadPtr;
			obj.txObj.toTxByte = 0;
			obj.txObj.strReadPtr = 0;
			USART_send_Array(obj.statusObj.uart, 0, (uint8_t*)(&(obj.txObj.txBuffer[temp2])), temp1);//Nach dieser Funktion, keine Code mehr schreiben
		} else{
			temp2 = obj.txObj.strReadPtr;
			obj.txObj.toTxByte-=MAX_BYTE_SEND;
			obj.txObj.strReadPtr+=MAX_BYTE_SEND;
			USART_send_Array(obj.statusObj.uart, 0, (uint8_t*)(&(obj.txObj.txBuffer[temp2])), MAX_BYTE_SEND);
		}
	}
	return true;
}
#endif

#ifdef VERSION_2
//wie oben aber lässt es die Sendung für TX_Callback function in ISR von HAL-Bibliothek
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length){
	uint8_t lengthForSend = (max_length>MAX_BYTE_SEND)?MAX_BYTE_SEND:max_length;
	uint8_t sizeOfArray = 1;
	*data = (uint8_t*)&(obj.txObj.txBuffer[obj.txObj.strReadPtr]);
	if (obj.txObj.toTxByte<lengthForSend){
		*length = (uint8_t)(obj.txObj.toTxByte);
		obj.txObj.toTxByte = 0;
		obj.txObj.strReadPtr = 0;
	} else{
		*length = (uint8_t)lengthForSend;
		obj.txObj.toTxByte-=lengthForSend;
		obj.txObj.strReadPtr+=MAX_BYTE_SEND;
	}
	return true;
}
#endif



#ifdef VERSION_3
//zu sendende Byte wird angepasst mit den freien Stellen in FIFO. Die Sendung wird hier durchgefuehrt
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length){
	uint8_t temp3 = max_length;
	uint8_t sendByte = (max_length>MAX_BYTE_SEND)?MAX_BYTE_SEND:max_length;
	volatile uint16_t temp1=0;
	volatile uint16_t temp2=0;
	if (obj.txObj.toTxByte!=0)
	{
		temp1=obj.txObj.strReadPtr;
		temp2=obj.txObj.toTxByte;
		if (obj.txObj.toTxByte<sendByte)
		{
			obj.txObj.toTxByte = 0;
			obj.txObj.strReadPtr = 0;	
			USART_send_Array(obj.statusObj.uart, 0, (uint8_t*)(&(obj.txObj.txBuffer[temp1])), temp2);
		} else
		{
			obj.txObj.toTxByte-=sendByte;
			obj.txObj.strReadPtr+=sendByte;
			USART_send_Array(obj.statusObj.uart, 0, (uint8_t*)(&(obj.txObj.txBuffer[temp1])), sendByte);
		}
	}
	return true;
}
#endif

static bool callbackRx(uint8_t adress, uint8_t data[], uint8_t length){
#ifdef ACTIVE_USART_WATCHER
	uint32_t remainTime = getUsartWatcherTimeout();
	if (remainTime)
	{
		//Weiter bis zum Ende der Schreibphase
		/*wenn die zu empfangenden Daten laenge als die Laenge von rxBuffer,kopieren bis zum Buffer voll, 
		  die uebrigen werden weggeworfen*/
		if ((obj.rxObj.strReadPtr+length)<RX_BUFFER_LEN)
		{
			memcpy((uint8_t*)&(obj.rxObj.rxBuffer[obj.rxObj.writeFIFOPtr][obj.rxObj.strReadPtr]),data,length);//TODO check
			obj.rxObj.strReadPtr+=length;
			obj.rxObj.rxByte[obj.rxObj.writeFIFOPtr]=obj.rxObj.strReadPtr;
		} else{
			uint8_t temp = RX_BUFFER_LEN-obj.rxObj.strReadPtr;
			memcpy((uint8_t*)&(obj.rxObj.rxBuffer[obj.rxObj.writeFIFOPtr][obj.rxObj.strReadPtr]),data,temp);//TODO check
			obj.rxObj.strReadPtr=0;
			obj.rxObj.rxByte[obj.rxObj.writeFIFOPtr]=RX_BUFFER_LEN;
		}
		obj.statusObj.nextPhase = 1;
	} else{
		//Anfang der Schreibphase
		obj.rxObj.strReadPtr = 0;
		//wenn FIFO nicht voll, dann schreibt, sonst nicht
		if (obj.statusObj.rxFIFOState!=FULL){
			memcpy((uint8_t*)&(obj.rxObj.rxBuffer[obj.rxObj.writeFIFOPtr][obj.rxObj.strReadPtr]),data,length);
			obj.rxObj.strReadPtr = (obj.rxObj.strReadPtr+length);
			obj.rxObj.rxByte[obj.rxObj.writeFIFOPtr]=obj.rxObj.strReadPtr;
		}
	}
	setUsartWatcherTimeout(USART_TIME_PRO_BYTE_US*3);
#else
	//Wenn keine Stringerkennungsmechanismus aktiv ist, dann bekommt die Charakter bis zum END-Text-Symbol erkennt oder Buffer voll
	if (length+obj.rxObj.toRxByte[obj.rxObj.writeFIFOPtr]<RX_BUFFER_LEN+1){
		memcpy((uint8_t*)&(obj.rxObj.rxBuffer[obj.rxObj.writeFIFOPtr][obj.rxObj.strReadPtr]),data,length);
		obj.rxObj.toRxByte[obj.rxObj.writeFIFOPtr]+=length;
		obj.rxObj.strReadPtr+=length;
		if (data[length-1]==END_SYM){
			obj.statusObj.nextPhase = 1;
			obj.rxObj.writeFIFOPtr = (obj.rxObj.writeFIFOPtr+1)%NO_OF_RX_BUFFER;
			obj.rxObj.strReadPtr=0;
			if (obj.rxObj.writeFIFOPtr==obj.rxObj.readFIFOPtr){
				obj.statusObj.rxBufferState=FULL;
			} else{
				obj.statusObj.rxBufferState=FILLED;
			}
		}
	} else {
		uint8_t temp = RX_BUFFER_LEN-obj.rxObj.strReadPtr;
		memcpy((uint8_t*)&(obj.rxObj.rxBuffer[obj.rxObj.writeFIFOPtr][obj.rxObj.strReadPtr]),data,temp);
		obj.rxObj.strReadPtr = 0;
		obj.rxObj.toRxByte[obj.rxObj.writeFIFOPtr] = RX_BUFFER_LEN;
		obj.statusObj.nextPhase = 1;
		obj.rxObj.writeFIFOPtr = (obj.rxObj.writeFIFOPtr+1)%NO_OF_RX_BUFFER;
		obj.statusObj.rxBufferState = (obj.rxObj.writeFIFOPtr==obj.rxObj.readFIFOPtr)?FULL:FILLED;
	}
#endif
	return false;
}

//Refaktorisierung fertig, dynamische Eigenschaft wird vorlaeufig deaktiviert
processResult_t initDev(uint8_t USARTnumber, uint32_t baudrate,USART_CHSIZE_t bits, USART_PMODE_t parity,USART_SBMODE_t stopbit,bool sync, bool MPCM, uint8_t address, PORTMUX_USARTx_t PortMux){
	uint8_t result = NO_ERROR;
	//initialisieren der zwei rx- und tx-Buffer
	memset((int**)obj.rxObj.rxBuffer,0,sizeof(obj.rxObj.rxBuffer));
	memset((int*)obj.txObj.txBuffer,0,sizeof(obj.txObj.txBuffer));
	memset((int*)obj.rxObj.rxByte,0,sizeof(obj.rxObj.rxByte));
	//obj.dataTx_p = dataTx;
	//obj.dataRx_p = dataRx;
	obj.statusObj.uart = USARTnumber;
	bool slaveUartInit = USART_init(USARTnumber,baudrate, bits, parity, stopbit, sync, MPCM,address, PortMux);
	USART_set_send_Array_callback_fnc(USARTnumber,&callbackTx);
#ifdef ACTIVE_USART_WATCHER
	USART_set_Bytes_to_receive(USARTnumber,1);
#else
	USART_set_Bytes_to_receive(USARTnumber,1); 
#endif
	
	USART_set_receive_Array_callback_fnc(USARTnumber,&callbackRx);
	obj.statusObj.initState = (!slaveUartInit)?1:0;
#ifdef ACTIVE_USART_WATCHER
	setUsartWatcherTimeout(USART_TIME_PRO_BYTE_US*3);
	setWatchedObj(&obj);
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



	

/*
 * slaveDevice.c: Quellcode für das slaveDevice-Modul
 *
 * Created: 21.06.2023 13:01:53
 * Author: Thach
 * Version: 1.0
 * Revision: 1.0
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
	.statusObj.rxFIFOState = EMPTY
};

#define  VERSION_2 1

#ifdef VERSION_1
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length) /*__attribute__((deprecated("funtcion is in debug time")))*/;
#endif

#ifdef VERSION_2
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length) /*__attribute__((deprecated("funtcion is in debug time")))*/;
#endif

#ifdef VERSION_3
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length) /*__attribute__((deprecated("funtcion is in debug time")))*/;
#endif

/**
 * \brief Zum Senden der Daten über USART
 * \detailed diese kopiert die Daten in txBuffer von obj, die Sendung erfolgt automatisch im Hintergrund
 *
 * \param data der Zeiger zum Datenbuffer
 * \param length die Länge der Datenbuffer
 * 
 * \return processResult_t 0: kein Fehler, erfolgreich; sonst: Fehler
 */
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

/**
 * \brief Lesen der empfangenen Daten aus rxBuffer von obj (Objektierung von slaveDevice_t Struktur)
 * \detailed Daten wird automatisch im Hintergrund empfangen und in rxBuffer von obj gespeichert
 * \param data der Zeiger zum Ausgabebuffer
 * \param length die Länge der zu empfangenden Daten
 * 
 * \return processResult_t 0: kein Fehler; sonst: Fehler
 */
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
		while (getUsartWatcherTimeout()!=0);//Warte bis rxBuffer total geschrieben wird
#endif
		uint8_t readFIFOPtrTemp = obj.rxObj.readFIFOPtr;
		*length = obj.rxObj.rxByte[readFIFOPtrTemp];
		memcpy(data,(uint8_t*)&(obj.rxObj.rxBuffer[readFIFOPtrTemp]),(*length));
		obj.rxObj.rxByte[readFIFOPtrTemp]=0;//Nach dem Lesen wird diese Byte als gelesen markiert
		obj.rxObj.readFIFOPtr = (obj.rxObj.readFIFOPtr+1)%NO_OF_RX_BUFFER;
		obj.statusObj.rxFIFOState = (obj.rxObj.readFIFOPtr == obj.rxObj.writeFIFOPtr)?EMPTY:FILLED;
	}
	return result;
}

#ifdef VERSION_1
/**
 * \brief die Rückruf-Funktion, es wird aufgeruft in ISR von Daten-Sendung-Erledigung
 * \detailed Version 1: die zu sendenden Bytes wird konstant in FIFO nach jedem Interrupt-Auflösen  
 *  hinzugefügt. Die weitere Sendung wird hier durchgefuehrt statt der Verlässung für die TX_Callback
 *  Funktion in ISR() von HAL-Bibliothek
 *  
 * \param adress der Zeiger zum Gerätsadresse
 * \param data der Zeiger zum Datenbuffer, das man mit HAL-Bibliothek-Funktion weiter senden will
 * \param length die Länge des ober genannten Datenbuffers
 * \param max_length die verfügbare Plätze von USART-FIFO (HAL-Bibliothek)
 * 
 * \return bool immer true, weil es in HAL-Bibliothek-Funktion nicht betrachtet wird
 */
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length){
	uint16_t toTxByteTemp = obj.txObj.toTxByte;
	if (toTxByteTemp!=0){
		uint16_t strReadPtrTemp = obj.txObj.strReadPtr;
		uint8_t usartNrTemp = obj.statusObj.uart;
		if (toTxByteTemp < MAX_BYTE_SEND){
			obj.txObj.toTxByte = 0;
			obj.txObj.strReadPtr = 0;
			USART_send_Array(usartNrTemp, 0, (uint8_t*)(&(obj.txObj.txBuffer[strReadPtrTemp])), toTxByteTemp);//Nach dieser Funktion, keine Code mehr schreiben
		} else{
			obj.txObj.toTxByte-=MAX_BYTE_SEND;
			obj.txObj.strReadPtr+=MAX_BYTE_SEND;
			USART_send_Array(usartNrTemp, 0, (uint8_t*)(&(obj.txObj.txBuffer[strReadPtrTemp])), MAX_BYTE_SEND);
		}
	}
	return true;
}
#endif

#ifdef VERSION_2
/**
 * \brief die Rückruf-Funktion, es wird aufgeruft in ISR() von Daten-Sendung-Erledigung (durch TX_Callback() Funktion)
 * \detailed Version 2: die in USART-FIFO hinzugefügten Bytes werden angepasst mit freien Plätzen in FIFO. 
 *  Die weitere Sendung wird für TX_Callback Funktion in ISR von HAL-Bibliothek verlassen
 * \param adress der Zeiger zum Gerätsadresse
 * \param data der Zeiger zum Datenbuffer, das man mit HAL-Bibliothek-Funktion weiter senden will
 * \param length die Länge des ober genannten Datenbuffers
 * \param max_length die verfügbare Plätze von USART-FIFO (HAL-Bibliothek)
 * 
 * \return bool immer true, weil es in HAL-Bibliothek-Funktion nicht betrachtet wird
 */
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length){
	uint8_t lengthForSend = (max_length>MAX_BYTE_SEND)?MAX_BYTE_SEND:max_length;
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
/**
 * \brief die Rückruf-Funktion, es wird aufgeruft in ISR() von Daten-Sendung-Erledigung (durch TX_Callback() Funktion)
 * \detailed Version 3: die in USART-FIFO hinzugefügten Bytes werden angepasst mit freien Plätzen in FIFO. 
 *  Die weitere Sendung wird hier durchgeführt statt der Verlassung für TX_Callback Funktion in ISR() von HAL-Bibliothek
 * \param adress der Zeiger zum Gerätsadresse
 * \param data der Zeiger zum Datenbuffer, das man mit HAL-Bibliothek-Funktion weiter senden will
 * \param length die Länge des ober genannten Datenbuffers
 * \param max_length die verfügbare Plätze von USART-FIFO (HAL-Bibliothek)
 * 
 * \return bool immer true, weil es in HAL-Bibliothek-Funktion nicht betrachtet wird
 */
bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length){
	uint8_t sendByte = (max_length>MAX_BYTE_SEND)?MAX_BYTE_SEND:max_length;
	uint16_t toTxByteTemp=obj.txObj.toTxByte;
	if (toTxByteTemp!=0)
	{
		uint16_t strReadPtrTemp=obj.txObj.strReadPtr;
		uint8_t usartNrTemp = obj.statusObj.uart;
		if (toTxByteTemp<sendByte)
		{
			obj.txObj.toTxByte = 0;
			obj.txObj.strReadPtr = 0;	
			USART_send_Array(usartNrTemp, 0, (uint8_t*)(&(obj.txObj.txBuffer[strReadPtrTemp])), toTxByteTemp);
		} else
		{
			obj.txObj.toTxByte-=sendByte;
			obj.txObj.strReadPtr+=sendByte;
			USART_send_Array(usartNrTemp, 0, (uint8_t*)(&(obj.txObj.txBuffer[strReadPtrTemp])), sendByte);
		}
	}
	return true;
}
#endif

/**
 * \brief die Rückruf-Funktion, es wird aufgeruft in ISR() von Daten-Empfangen-Erledigung (durch RX_Callback() Funktion)
 * 
 * \param adress Gerätsadresse
 * \param data der Zeiger zum USART-FIFO
 * \param length die Länge der schon empfangenen Daten
 * 
 * \return bool immer true, weil es in HAL-Bibliothek-Funktion nicht betrachtet wird
 */
static bool callbackRx(uint8_t adress, uint8_t data[], uint8_t length){
	uint16_t writeFIFOPtrTemp = obj.rxObj.writeFIFOPtr;
	uint16_t strReadPtrTemp = obj.rxObj.strReadPtr;
#ifdef ACTIVE_USART_WATCHER
	uint32_t remainTime = getUsartWatcherTimeout();
	if (remainTime)
	{
		//Weiter bis zum Ende der Schreibphase
		/*wenn die zu empfangenden Daten laenge als die Laenge von rxBuffer,kopieren bis zum Buffer voll, 
		  die uebrigen werden weggeworfen*/
		if ((strReadPtrTemp+length)<RX_BUFFER_LEN)
		{
			memcpy((uint8_t*)&(obj.rxObj.rxBuffer[writeFIFOPtrTemp][strReadPtrTemp]),data,length);//TODO check
			obj.rxObj.strReadPtr+=length;
			strReadPtrTemp = obj.rxObj.strReadPtr;//Aktualisiert des Werts von obj.rxObj.strReadPtr zu Zwischenvariable
			obj.rxObj.rxByte[writeFIFOPtrTemp]=strReadPtrTemp;
		} else{
			uint8_t temp = RX_BUFFER_LEN-strReadPtrTemp;
			memcpy((uint8_t*)&(obj.rxObj.rxBuffer[writeFIFOPtrTemp][strReadPtrTemp]),data,temp);//TODO check
			obj.rxObj.strReadPtr=0;
			obj.rxObj.rxByte[writeFIFOPtrTemp]=RX_BUFFER_LEN;
		}
	} else{
		//Anfang der Schreibphase
		obj.rxObj.strReadPtr = 0;
		//wenn FIFO nicht voll, dann schreibt, sonst nicht
		if (obj.statusObj.rxFIFOState!=FULL){
			memcpy((uint8_t*)&(obj.rxObj.rxBuffer[writeFIFOPtrTemp][strReadPtrTemp]),data,length);
			obj.rxObj.strReadPtr += length;
			strReadPtrTemp = obj.rxObj.strReadPtr;//Aktualisiert des Werts von obj.rxObj.strReadPtr zu Zwischenvariable
			obj.rxObj.rxByte[obj.rxObj.writeFIFOPtr]=strReadPtrTemp;
		}
	}
	uint64_t timeout = (10000000UL * BIT_PER_SYM*3/BAUDRATE_BAUD*2);
	setUsartWatcherTimeout(timeout);
#else
	//Wenn keine Stringerkennungsmechanismus aktiv ist, dann bekommt die Charakter bis zum END-Text-Symbol erkennt oder Buffer voll
	/*wenn die zu empfangenden Daten laenge als die Laenge von rxBuffer,kopieren bis zum Buffer voll, 
		  die uebrigen werden weggeworfen*/
	if (length+obj.rxObj.rxByte[writeFIFOPtrTemp]<=RX_BUFFER_LEN){
		memcpy((uint8_t*)&(obj.rxObj.rxBuffer[writeFIFOPtrTemp][strReadPtrTemp]),data,length);
		obj.rxObj.strReadPtr+=length;//Aktualisiert des Werts von obj.rxObj.strReadPtr zu Zwischenvariable
		strReadPtrTemp=obj.rxObj.strReadPtr;
		obj.rxObj.rxByte[writeFIFOPtrTemp]=strReadPtrTemp;
		
		if (data[length-1]==END_SYM){
			obj.rxObj.writeFIFOPtr = (writeFIFOPtrTemp+1)%NO_OF_RX_BUFFER;
			obj.rxObj.strReadPtr=0;
			obj.statusObj.rxFIFOState=(obj.rxObj.writeFIFOPtr==obj.rxObj.readFIFOPtr)?FULL:FILLED;
		}
	} else {
		uint8_t temp = RX_BUFFER_LEN-strReadPtrTemp;
		memcpy((uint8_t*)&(obj.rxObj.rxBuffer[writeFIFOPtrTemp][strReadPtrTemp]),data,temp);
		obj.rxObj.strReadPtr = 0;
		obj.rxObj.rxByte[writeFIFOPtrTemp] = RX_BUFFER_LEN;
		obj.rxObj.writeFIFOPtr = (writeFIFOPtrTemp+1)%NO_OF_RX_BUFFER;
		obj.statusObj.rxFIFOState = (obj.rxObj.writeFIFOPtr==obj.rxObj.readFIFOPtr)?FULL:FILLED;
	}
#endif
	return false;
}

/**
 * \brief Initialization des Moduls
 * 
 * \param USARTnumber die Nummer von USART (in USART.h in HAL-Bibliothek definiert)
 * \param baudrate die Baudrate in Baud
 * \param bits die Anzahl der Datenbytes in einem Frame
 * \param parity die Art vom Parity (kein,gerade, ungerade)
 * \param stopbit die Anzahl der Stopbits
 * \param sync Synchronmodus aktiv oder nicht 
 * \param MPCM Multiprocess
 * \param address adresse des Patnergerätes
 * \param PortMux Art des PortMultiplexers
 * 
 * \return processResult_t 0: kein Fehler, sonst: Fehler
 */
processResult_t initDev(uint8_t USARTnumber, uint32_t baudrate,USART_CHSIZE_t bits, USART_PMODE_t parity,USART_SBMODE_t stopbit,bool sync, bool MPCM, uint8_t address, PORTMUX_USARTx_t PortMux){
	uint8_t result = NO_ERROR;
	obj.statusObj.uart = USARTnumber;
	bool slaveUartInit = USART_init(USARTnumber,baudrate, bits, parity, stopbit, sync, MPCM,address, PortMux);
	obj.statusObj.initState = (!slaveUartInit)?1:0;
	if(!slaveUartInit){
		USART_set_send_Array_callback_fnc(USARTnumber,&callbackTx);
		USART_set_Bytes_to_receive(USARTnumber,1);
	
		USART_set_receive_Array_callback_fnc(USARTnumber,&callbackRx);
	
#ifdef ACTIVE_USART_WATCHER
		uint64_t timeout = (10000000UL * BIT_PER_SYM*3/BAUDRATE_BAUD*2);
		setUsartWatcherTimeout(timeout);
		setWatchedObj(&obj);
#endif
	}
	result = (!slaveUartInit)?NO_ERROR:PROCESS_FAIL;
	return result;
}

/**
 * \brief Deinitialization des Moduls
 * 
 * 
 * \return void
 */
void deinitDev(){
	USART_set_send_Array_callback_fnc(obj.statusObj.uart,NULL);
	USART_set_Bytes_to_receive(obj.statusObj.uart,0);
	USART_set_receive_Array_callback_fnc(obj.statusObj.uart,NULL);
	obj.statusObj.uart = 0;
	obj.statusObj.initState = 0;
}



	

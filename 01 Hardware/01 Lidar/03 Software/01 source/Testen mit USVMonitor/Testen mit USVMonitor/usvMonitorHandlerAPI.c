/*
 * usvMonitorHandlerAPI.c
 *
 * Created: 7/7/2023 8:42:59 AM
 * Author: Thach
 */

#include "usvMonitorHandlerAPI.h"

//Fehlererkennt, DEBUGGING ERFORDERLICH
//statische Fehlerdefinition
typedef enum {
	NO_ERROR,
	NULL_POINTER,
	FIFO_EMPTY,
	FIFO_FULL,
	DATA_INVALID,
	PROCESS_FAIL,
	HANDLER_NOT_INIT
}processResult_t;

volatile bool checkByteReg = false;
volatile bool checkByteAddAndRw = false;
volatile uuaslReadProtocol_t protocol;
volatile uint8_t tempBuffer[MAX_SIZE_FRAME]={0};
volatile uint8_t buffer[MAX_SIZE_FRAME]={0};//Gesamtarray
volatile uint8_t bufferMulti[MAX_SIZE_FRAME]={0};
volatile uint8_t bufferMultiTx[MAX_SIZE_FRAME]={0};
volatile int8_t temp01 = 0;
volatile int8_t temp02 = 0;
volatile uint16_t positionPtrTX=0;

static const slaveReg_t regSet[]={
	//Sensorblock
	{SEN_GESB_ADD,1},
	{SEN_LONGNITUDE_ADD,4},
	{SEN_LATITUDE_ADD,4},
	{SEN_SATFIX_ADD,1},
	{SEN_GPS_VEL_ADD,2},
	{SEN_COURSE_ANGLE_ADD,2},
	{SEN_TIMESTAMP_ADD,3},
	//Führungsgrößen der Antriebsregelung
	{REF_DRV_CTRL_REF_A_ADD,8},
	{REF_DRV_CTRL_REF_B_ADD,8},
	{REF_DRV_CTRL_VEL_ADD,2},
	{REF_DRV_CTRL_EPS_ADD,2},
	//Stellgrößen der Antriebsregelung
	{CTRL_DRV_CTRL_THRUST_ADD,2},
	{CTRL_DRV_CTRL_RUDDER_ANGLE_ADD,2},
	//lokaler Error Block
	{ESB_GPS_ADD,1},
	{ESB_COMPASS_ADD,1},
	{ESB_CTRL_ADD,1}//,
	//Lidar
	//{LIDAR_SEN_ADD,361}
};

static uint8_t crc8Checksum(uint8_t *data, uint16_t len, uint8_t polynom){
	uint8_t crc = 0;
	uint8_t mix;
	uint8_t inbyte;
	while (len--){
		inbyte = *data++;
		for (uint8_t i = 8; i; i--){
			mix = ( crc ^ inbyte ) & 0x80;
			crc <<= 1;
			if (mix){
				crc ^= polynom;
			}
			inbyte <<= 1;
		}
	}
	return crc;
}

static inline int8_t searchReg(uint16_t reg){
	int8_t result = -1;
	for (uint8_t i = 0;i<sizeof(regSet)/sizeof(slaveReg_t);i++)
	{
		if (regSet[i].add == reg){
			result = i;
			break;
		}
	}
	return result;
}

static inline int8_t searchEnd(int8_t begin, uint16_t len){
	int8_t result = -1;
	uint8_t endOfList = sizeof(regSet)/sizeof(slaveReg_t);
	int32_t remainLen = (int32_t)len;
	if(begin!=-1){
		uint8_t i = 0;
		for (i = begin; i<endOfList;i++){
			remainLen -= regSet[i].len;
			if (remainLen<0){//Wenn die erforderte Länge zwischen 2 Registers liegt, dann nimmt das untere als Ergebnis
				i--;
				break;
			} else if (!remainLen){
				break;
			}
		}
		result = i;
	}
	return result;
}

static inline uint16_t getTotalLen(int8_t begin, int8_t end){
	uint16_t result = 0;
	if ((begin>-1)&&(end>begin)){
		for (int i = begin;i<end+1;i++){
			result+=regSet[i].len;
		}
	}
	return result;
}

/**
 * \brief zum Erzeugen des Lesenprotokolls
 * \warning keine Fehlermeldung ausgeben, man muss die Gültigkeit vom Index vor dem Aufruf checken
 * \param add die spezifische Id von Slave (slave Adresse)
 * \param index Index in der Liste vom zu lesenden Register
 * 
 * \return uuaslReadProtocol_t das vollstängie Lesenprotokoll
 */
static inline uuaslReadProtocol_t readProtocolPrint(uint16_t add,uint16_t index){
	uuaslReadProtocol_t result ={
		.header.start =	0xA5,
		.header.slaveAdd = add,
		.header.rwaBytes.value_bf.slaveRegAddL = GET_SLAVE_ADD_LOW_PART(regSet[index].add),
		.header.rwaBytes.value_bf.slaveRegAddH = GET_SLAVE_ADD_HIGH_PART(regSet[index].add),
		.header.rwaBytes.value_bf.rw = UUASL_R_REQ,
		.header.length = 8,
		.dataLen = regSet[index].len,
		.tail.end = 0xA6
	};
	result.tail.checksum = crc8Checksum(&(result.dataLen), sizeof(result.dataLen), CRC8_POLYNOM);
	return result;
}

/**
 * \brief zum Erzeugen des Protokoll-Header
 * \warning keine Fehler ausgeben, man muss die Gültigkeit vom Index vor dem Aufruf checken
 * \param add die spezifische Id von Slave (slave Adresse)
 * \param index Index in der Liste vom zu lesenden Register
 * \param rwReq 8 für Schreiben- und 4 für Lesenanforderungen
 * 
 * \return uuaslProtocolHeader_t das vollstängie Header des Protokolls
 */
static inline uuaslProtocolHeader_t protocolHeaderPrint(uint16_t add,uint16_t index,uint8_t rwReq){
	uuaslProtocolHeader_t result = {
		.start = 0xA5,
		.slaveAdd = add,
		.rwaBytes.value_bf.slaveRegAddL = GET_SLAVE_ADD_LOW_PART(regSet[index].add),
		.rwaBytes.value_bf.slaveRegAddH = GET_SLAVE_ADD_HIGH_PART(regSet[index].add),
		.rwaBytes.value_bf.rw = rwReq,
		.length = regSet[index].len
	};
	return result;
}

//Das Checksum-CRC8-Ergebnis muss davor berechnet werden
static inline uuaslProtocolTail_t writeProtocolTailPrint(uint8_t crc8){
	uuaslProtocolTail_t result = {
		.checksum = crc8,
		.end = 0xA6
	};	
	return result;
}

static inline bool checkRxData(uint8_t* data,uint16_t dataLen, uint8_t rxChecksumValue, uint8_t crc8Polynom){
	return crc8Checksum(data,dataLen,crc8Polynom)==rxChecksumValue;
}


/**
 * \brief Initalisierung des Handlers
 * 
 * \param dev_p Zeiger zum zu initalisierenden Handler 
 * \param inputRxFunc_p der Zeiger zur Datenempfangen Funktion
 * \param inputTxFunc_p der Zeiger zur Datensenden Funktion
 * \param inputWaitFunc_p der Zeiger zur Warte Funktion
 * \param inputCrc8 der Checksum-CRC8 Polynom
 * 
 * \return uint8_t 0: keinen Fehler, sonst Fehler
 */
uint8_t initDev(usvMonitorHandler_t* dev_p, dataRx_t inputRxFunc_p, dataTx_t inputTxFunc_p, wait_t inputWaitFunc_p, uint8_t inputCrc8){
	uint8_t result = NO_ERROR;
	if ((inputRxFunc_p!=NULL)&&(inputTxFunc_p!=NULL)&&(inputWaitFunc_p!=NULL)){
		dev_p->receiveFunc_p = inputRxFunc_p;
		dev_p->transmitFunc_p = inputTxFunc_p;
		dev_p->waitFunc_p = inputWaitFunc_p;
		dev_p->crc8Polynom = inputCrc8;
		dev_p->initState = 1;
		dev_p->dynamicWait = (inputWaitFunc_p==NULL)?1:0;
	} else{
		result = NULL_POINTER;
	}
	return result;
	
}

/**
 * \brief Zum Schreiben in einem Register im Slave-Gerät
 * 
 * \param add Adresse vom Slave
 * \param reg die slavespezifische ID (Adresse von Register vom Slave)
 * \param dev_p der Zeiger zum Handler
 * \param input_p der Zeiger zur Eingabedaten 
 * \param length die Länge der Eingabe
 * 
 * \return uint8_t 0: keinen Fehler, sonst: Fehler
 */
uint8_t setData(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* input_p, uint16_t length){
	uint8_t result = NO_ERROR;
	int8_t index = searchReg(reg);
	if ((dev_p==NULL)|(input_p==NULL)){
		result = NULL_POINTER;
	} else{
		if (index != -1){
			if (regSet[index].len != length){
				result = DATA_INVALID;
			} else{
				
				uint16_t positionPtr=0;
				//Header im Gesamtarray kopieren
				uuaslProtocolHeader_t head = protocolHeaderPrint(add,index,UUASL_W_REQ);
				head.length+=7;
				memcpy((uint8_t*)(&buffer[positionPtr]),(uint8_t*)&head,sizeof(head)/sizeof(uint8_t));
				positionPtr+= sizeof(head)/sizeof(uint8_t);
				//Inhalt im Gesamtarray kopieren
				memcpy((uint8_t*)(&buffer[positionPtr]),input_p,length);
				positionPtr+=length;
				//Tail im Gesamtarray kopieren
				uint8_t crc8 = crc8Checksum(input_p,length,dev_p->crc8Polynom);
				uuaslProtocolTail_t tail = writeProtocolTailPrint(crc8);
				memcpy((uint8_t*)(&buffer[positionPtr]),(uint8_t*)&tail,sizeof(tail)/sizeof(uint8_t));
				positionPtr += sizeof(tail)/sizeof(uint8_t);
				//Senden die Daten
				result = (*(dev_p->transmitFunc_p))((uint8_t*)buffer,positionPtr);
				positionPtr = 1;
				(*(dev_p->waitFunc_p))(700);//warten 0,5ms für Datensendung
				(*(dev_p->receiveFunc_p))((uint8_t*)buffer, positionPtr);
				if (buffer[0]!=0xA1){
					result = PROCESS_FAIL;
				}
			}
		} else{
			result = DATA_INVALID;
		}
	}
	return result;
}

/**
 * \brief Empfangen die Daten aus dem Slave-Gerät, inklusiv CRC-Byte
 * 
 * \param add die Adresse vom Slave
 * \param reg die slavespezifische ID (Adresse von Register vom Slave)
 * \param dev_p der Zeiger zum Handler
 * \param output der empfangene Datenteil im gelesenen Register zusammen mit der CRC8-Checksum-Datei
 * \param outputLen die Länge der empfangenen Daten inklusiv der Checksum
 * 
 * \return uint8_t 0: keinen Fehler, sonst: Fehler
 */
uint8_t getData(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* output_p, uint16_t outputLen){
	uint8_t result = NO_ERROR;
	if((dev_p==NULL)||(output_p==NULL)){
		result = NULL_POINTER;
	} else if (dev_p->initState==0)
	{
		result = HANDLER_NOT_INIT;
	} else{
		int8_t index = searchReg(reg);
		if (index!=-1){
			
			uint16_t rxLength=1;
			//Bildung von Datenrahmen
			/**
			 * Bei Byte 3,4 (Adresse-Bytes) wird ein 2-Byte-langes Register dafür festgelegt. 
			 * Aufgrund vom little-endian System werden die Daten im Protokoll (Egal Hin- oder 
			 * Rückprotokoll) vertauscht
			 */
			protocol = readProtocolPrint(add,index);
			memcpy((uint8_t*)tempBuffer,(uint8_t*)&protocol,sizeof(protocol)/sizeof(uint8_t));			
			(*(dev_p->transmitFunc_p))((uint8_t*)tempBuffer, sizeof(protocol)/sizeof(uint8_t));
			if (!dev_p->dynamicWait){
				(*(dev_p->waitFunc_p))(800);//Warte 0,8ms
			}
			memset((uint8_t*)tempBuffer,0,8);
//#ifndef DEBUG_2
			//Nach dem Request-Senden, empfangen erste Byte
			(*(dev_p->receiveFunc_p))((uint8_t*)tempBuffer, rxLength);
			//checken erste Byte
			if(tempBuffer[0]==0xA2){
				result = DATA_INVALID;
			} else if(tempBuffer[0] == 0xA5){ //Wenn erfolgreich, checken weitere 4 Bytes
				//Byte 4 beim Daten lesen: Bei Hinprotokoll 0x4X, bei Rückprotokoll 0x0X => 0x4X XOR 0x0X = 0x40
				rxLength=4;
				(*(dev_p->receiveFunc_p))((uint8_t*)tempBuffer, rxLength);
				checkByteReg = (tempBuffer[0]==protocol.header.slaveAdd);
				checkByteAddAndRw = ((tempBuffer[2]^protocol.header.rwaBytes.value[1])==0x40) && (tempBuffer[1]==protocol.header.rwaBytes.value[0]);
				bool checkAll = checkByteReg&&checkByteAddAndRw;
				if(!(checkAll)){
					result = PROCESS_FAIL;
				} else{
					//empfangen weitere n Datenbytes sowie 1 CRC8- und 1 Endbyte
					rxLength =tempBuffer[3]-5;//Die Länge vom Header = 5
					(*(dev_p->receiveFunc_p))((uint8_t*)tempBuffer, rxLength);
					bool checkDataBlock = checkRxData((uint8_t*)tempBuffer,rxLength-2,tempBuffer[rxLength-2],dev_p->crc8Polynom);
					bool checkEnd = tempBuffer[rxLength-1]==0xA6;
					if (checkEnd&&checkDataBlock){
						//kopieren aller Datenbytes in Ausgabe
						memcpy(output_p,(uint8_t*)tempBuffer,rxLength-2);
					} else {
						result = PROCESS_FAIL;
					}
				}
			} else{
				result = PROCESS_FAIL;
			}
//#endif
		} else{
			result = DATA_INVALID;
		}
	}
	return result;
}

//Noch in Bearbeitung
uint8_t getMultiregister(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* output_p, uint16_t outputLen){
	uint8_t result = NO_ERROR;
	if((dev_p==NULL)||(output_p==NULL)){
		result = NULL_POINTER;
	} else if (dev_p->initState==0)
	{
		result = HANDLER_NOT_INIT;
	} else{
		int8_t begin = searchReg(reg);
		int8_t end = searchEnd(begin, outputLen);
		temp01 = begin;
		temp02 = end;
		if ((begin!=-1)&&(end!=-1)){
			uint16_t bufferMultiPtr = 0;
			outputLen = getTotalLen(begin,end);
			uuaslProtocolHeader_t header = protocolHeaderPrint(add,begin,UUASL_R_REQ);
			header.length = 8;//magic number: Bytes vom gesamten Protokoll: 1 für Datenlängenanfrage, 7 für übrigen
			memcpy((uint8_t*)(&bufferMulti[bufferMultiPtr]),(uint8_t*)&header,sizeof(header)/sizeof(uint8_t));
			bufferMultiPtr += sizeof(header)/sizeof(uint8_t);
			uint8_t dataSegLen = 0;
			dataSegLen = outputLen;
			memcpy((uint8_t*)&(bufferMulti[bufferMultiPtr]),(uint8_t*)&dataSegLen,sizeof(dataSegLen)/sizeof(uint8_t));
			bufferMultiPtr += sizeof(dataSegLen)/sizeof(uint8_t);
			uint8_t checksumCode = crc8Checksum(&dataSegLen,1,dev_p->crc8Polynom);
			uint8_t endByte = 0xA6;
			bufferMulti[bufferMultiPtr++]=checksumCode;
			bufferMulti[bufferMultiPtr++]=endByte;
			(*(dev_p->transmitFunc_p))((uint8_t*)bufferMulti,bufferMultiPtr);
			if (!dev_p->dynamicWait){
				(*(dev_p->waitFunc_p))(bufferMultiPtr*100);//Warte 0,8ms
			}
			bufferMultiPtr=1;
			(*(dev_p->receiveFunc_p))((uint8_t*)bufferMulti, bufferMultiPtr);
			if (bufferMulti[bufferMultiPtr-1]==0xA2){
				result = DATA_INVALID;
			} else if (bufferMulti[bufferMultiPtr-1]==0xA5){
				bufferMultiPtr=4;
				(*(dev_p->receiveFunc_p))((uint8_t*)bufferMulti, bufferMultiPtr);
				//Checken Protokollrelevante Informationen
				bool checkRxDataInfo = (bufferMulti[0]==add) && \
								   (bufferMulti[1]==header.rwaBytes.value[0]) && \
								   ((bufferMulti[2]^header.rwaBytes.value[1])==0x40);
				if (checkRxDataInfo){
					bufferMultiPtr=bufferMulti[3]-7+2;
					(*(dev_p->receiveFunc_p))((uint8_t*)bufferMulti, bufferMultiPtr);
					bool checkDataBlock = checkRxData((uint8_t*)bufferMulti,bufferMultiPtr-2,bufferMulti[bufferMultiPtr-2],dev_p->crc8Polynom);;
					bool checkEndByte = bufferMulti[bufferMultiPtr-1]==0xA6;
					if (checkDataBlock&&checkEndByte){
						memcpy(output_p,(uint8_t*)bufferMulti,bufferMultiPtr-2);
					} else {
						result = PROCESS_FAIL;
					}
				} else{
					result = PROCESS_FAIL;
				}
			} else{
				result = PROCESS_FAIL;
			}
		} else{
			result = DATA_INVALID;
		}
	}
	temp01 = 0;
	temp02 = 0;
	return result;
}

//Noch in Bearbeitung
uint8_t setMultiregister(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* input_p, uint16_t inputLen){
	uint8_t result = NO_ERROR;
	if((dev_p==NULL)||(input_p==NULL)){
		result = NULL_POINTER;
	} else{
		int8_t begin = searchReg(reg);
		int8_t end = searchEnd(begin, inputLen);
		inputLen = getTotalLen(begin,end);
		if ((begin != -1)&&(end>=begin)){
			
			
			//Header im Gesamtarray kopieren
			uuaslProtocolHeader_t head = protocolHeaderPrint(add,begin,UUASL_W_REQ);
			head.length = inputLen+7;
			memcpy((uint8_t*)(&bufferMultiTx[positionPtrTX]),(uint8_t*)&head,sizeof(head)/sizeof(uint8_t));
			positionPtrTX+= sizeof(head)/sizeof(uint8_t);
			//Inhalt im Gesamtarray kopieren
			memcpy((uint8_t*)(&bufferMultiTx[positionPtrTX]),input_p,inputLen);
			positionPtrTX+=inputLen;
			//Tail im Gesamtarray kopieren
			uint8_t crc8 = crc8Checksum(input_p,inputLen,dev_p->crc8Polynom);
			volatile uuaslProtocolTail_t tail = writeProtocolTailPrint(crc8);
			memcpy((uint8_t*)(&bufferMultiTx[positionPtrTX]),(uint8_t*)&tail,sizeof(tail)/sizeof(uint8_t));
			positionPtrTX += sizeof(tail)/sizeof(uint8_t);
			//Senden die Daten
			result = (*(dev_p->transmitFunc_p))((uint8_t*)bufferMultiTx,positionPtrTX);
			positionPtrTX = 1;
			(*(dev_p->waitFunc_p))(700);//warten 0,5ms für Datensendung
			(*(dev_p->receiveFunc_p))((uint8_t*)bufferMultiTx, positionPtrTX);
			if (bufferMultiTx[0]!=0xA1){
				result = PROCESS_FAIL;
			}
		} else{
			result = DATA_INVALID;
		}
	}
	return result;
}



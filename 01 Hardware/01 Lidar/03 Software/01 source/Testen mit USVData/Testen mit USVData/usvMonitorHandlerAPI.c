/*
 * usvMonitorHandlerAPI.c
 *
 * Created: 7/7/2023 8:42:59 AM
 * Author: Thach
 * Version: 1.3
 * Revision: 1.1
 */

#include "usvMonitorHandlerAPI.h"

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

//Liste der Register und deren Länge
static const slaveReg_t regSet[]={
	//Sensorblock
	{SEN_GESB_ADD,1},
	{SEN_LONGNITUDE_ADD,4},
	{SEN_LATITUDE_ADD,4},
	{SEN_SATFIX_ADD,1},
	{SEN_GPS_VEL_ADD,2},
	{SEN_COURSE_ANGLE_ADD,2},
	{SEN_TIMESTAMP_ADD,3},
	//Radar
	{RADAR_1_ADD,1},
	{RADAR_1_ADD,2},
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
	{ESB_CTRL_ADD,1},
	//Lidar
	{LIDAR_VALUE_ADD,360},
};

/**
 * \brief Bildung der checksum-Code für Programm
 * 
 * \param data der Zeiger zum Datenblock
 * \param len die Länge des Datenblocks
 * \param polynom das binäre Polynom (in hex Form)
 * 
 * \return uint8_t das checksum-Code
 */
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

/**
 * \brief die Position des Registers in der Liste suchen
 * 
 * \param reg die Adresse des Registers
 * 
 * \return int8_t -1: kein gefunden, sonst: die Position in der Liste
 */
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

/**
 * \brief der Datenblock bilden aus einem Register und einer Länge
 * 
 * \param begin das Register, wobei man die Suche beginnt will
 * \param len die Länge des erwünschten Datenblock
 * 
 * \return int8_t die Position des Registers, das mit der Länge erreicht werden kann, in der Liste
 */
static inline int8_t searchEnd(int8_t begin, uint16_t len){
	//TODO noch zu verbessern, weil es viele reservierten Register zwischen zwei definierten Registern gibt
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
	if ((begin>-1)&&(end>=begin)){
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
static inline uuaslReadProtocol_t readProtocolPrint(uint16_t add,uint16_t index, uint8_t crc8){
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
	result.tail.checksum = crc8Checksum(&(result.dataLen), sizeof(result.dataLen), crc8);
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


/**
 * \brief zum Bilden der letzten zwei Bytes (als Tail bezeichnet) für das Schreibprotokoll
 * \warning Das Checksum-CRC8-Ergebnis muss davor berechnet werden
 * \param crc8 das checksum-Code
 * 
 * \return uuaslProtocolTail_t das Tail des Schreibprotokoll
 */
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
	//TODO zu optimieren
	uint8_t result = NO_ERROR;
	int8_t index = searchReg(reg);
	if ((dev_p==NULL)|(input_p==NULL)){
		result = NULL_POINTER;
	} else{
		if (index != -1){
			if (regSet[index].len != length){
				result = DATA_INVALID;
			} else{
				uint8_t buffer[MAX_SIZE_FRAME] = {0};//Zwischenspeicherbuffer
				uint16_t positionPtr = 0;//der Zeiger zur nächsten freien Position, die Länge der genutzten Bytes
				//Header im Gesamtarray kopieren
				uuaslProtocolHeader_t head = protocolHeaderPrint(add,index,UUASL_W_REQ);
				head.length+=7;
				memcpy((&buffer[positionPtr]),(uint8_t*)&head,sizeof(head)/sizeof(uint8_t));
				positionPtr+= sizeof(head)/sizeof(uint8_t);
				//Inhalt im Gesamtarray kopieren
				memcpy((&buffer[positionPtr]),input_p,length);
				positionPtr+=length;
				//Tail im Gesamtarray kopieren
				uint8_t crc8 = crc8Checksum(input_p,length,dev_p->crc8Polynom);
				uuaslProtocolTail_t tail = writeProtocolTailPrint(crc8);
				memcpy((&buffer[positionPtr]),(uint8_t*)&tail,sizeof(tail)/sizeof(uint8_t));
				positionPtr += sizeof(tail)/sizeof(uint8_t);
				//Senden die Daten
				result = (*(dev_p->transmitFunc_p))(buffer,positionPtr);
#if WAIT_FUNCTION_ACTIVE
				(*(dev_p->waitFunc_p))(FACTOR_TO_MICROSEC*CHARS_PER_FRAME*positionPtr*3/BAUDRATE_BAUD/2);//warten
#endif
				positionPtr = 1;
				(*(dev_p->receiveFunc_p))(buffer, positionPtr);
#if WAIT_FUNCTION_ACTIVE
				(*(dev_p->waitFunc_p))(FACTOR_TO_MICROSEC*CHARS_PER_FRAME*positionPtr*3/BAUDRATE_BAUD/2);//warten
#endif
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
	//TODO zu optimieren
	uint8_t result = NO_ERROR;
	if((dev_p==NULL)||(output_p==NULL)){
		result = NULL_POINTER;
	} else if (dev_p->initState==0)
	{
		result = HANDLER_NOT_INIT;
	} else{
		int8_t index = searchReg(reg);
		if (index!=-1){
			uint8_t buffer[MAX_SIZE_FRAME] = {0};//Zwischenspeicherbuffer
			uint16_t positionPtr = 0;//der Zeiger zur nächsten freien Position, die Länge der nutzbaren Bytes
			//Bildung von Datenrahmen
			/**
			 * Bei Byte 3,4 (Adresse-Bytes) wird ein 2-Byte-langes Register dafür festgelegt. 
			 * Aufgrund vom little-endian System werden die Daten im Protokoll (Egal Hin- oder 
			 * Rückprotokoll) vertauscht
			 */
			uuaslReadProtocol_t protocol;
			protocol = readProtocolPrint(add,index,dev_p->crc8Polynom);
			positionPtr = sizeof(protocol)/sizeof(uint8_t);
			memcpy(buffer,(uint8_t*)&protocol, positionPtr);	
			//Senden der Daten		
			(*(dev_p->transmitFunc_p))(buffer, positionPtr);
#if WAIT_FUNCTION_ACTIVE
			(*(dev_p->waitFunc_p))(FACTOR_TO_MICROSEC*CHARS_PER_FRAME*positionPtr*3/BAUDRATE_BAUD/2);//warten
#endif
			positionPtr = 1;
			//Nach dem Request-Senden, empfangen erste Byte
			(*(dev_p->receiveFunc_p))(buffer, positionPtr);
#if WAIT_FUNCTION_ACTIVE
			(*(dev_p->waitFunc_p))(FACTOR_TO_MICROSEC*CHARS_PER_FRAME*positionPtr*3/BAUDRATE_BAUD/2);//warten
#endif
			//checken erste Byte
			if(buffer[0]==0xA2){
				result = DATA_INVALID;
			} else if(buffer[0] == 0xA5){ //Wenn erfolgreich, checken weitere 4 Bytes
				//Byte 4 beim Daten lesen: Bei Hinprotokoll 0x4X, bei Rückprotokoll 0x0X => 0x4X XOR 0x0X = 0x40
				positionPtr=4;
				(*(dev_p->receiveFunc_p))(buffer, positionPtr);
#if WAIT_FUNCTION_ACTIVE
				(*(dev_p->waitFunc_p))(FACTOR_TO_MICROSEC*CHARS_PER_FRAME*positionPtr*3/BAUDRATE_BAUD/2);//warten
#endif
				bool checkByteReg = (buffer[0]==protocol.header.slaveAdd);
				bool checkByteAddAndRw = ((buffer[2]^protocol.header.rwaBytes.value[1])==0x40) && (buffer[1]==protocol.header.rwaBytes.value[0]);
				bool checkAll = checkByteReg&&checkByteAddAndRw;
				if(!(checkAll)){
					result = PROCESS_FAIL;
				} else{
					//empfangen weitere n Datenbytes sowie 1 CRC8- und 1 Endbyte
					positionPtr =buffer[3]-5;//Die Länge vom Header = 5
					(*(dev_p->receiveFunc_p))(buffer, positionPtr);
#if WAIT_FUNCTION_ACTIVE
					(*(dev_p->waitFunc_p))(FACTOR_TO_MICROSEC*CHARS_PER_FRAME*positionPtr*3/BAUDRATE_BAUD/2);//warten
#endif
					bool checkDataBlock = checkRxData(buffer,positionPtr-2,buffer[positionPtr-2],dev_p->crc8Polynom);
					bool checkEnd = buffer[positionPtr-1]==0xA6;
					if (checkEnd&&checkDataBlock){
						//kopieren aller Datenbytes in Ausgabe
						memcpy(output_p,buffer,positionPtr-2);
					} else {
						result = PROCESS_FAIL;
					}
				}
			} else{
				result = PROCESS_FAIL;
			}
		} else{
			result = DATA_INVALID;
		}
	}
	return result;
}

/**
 * \brief zum Lesen in vielen Registern nur mit einem Protokoll (bis zum 255 Bytes unterstützt)
 * 
 * \param add die Adresse vom abgefragten Gerät
 * \param reg die Adresse des ersten Registers im gelesenen Datenblock
 * \param dev_p der Zeiger zum Handler
 * \param output_p das Buffer zur Ausgabe
 * \param outputLen die Länge der Ausgabebuffer
 * 
 * \return uint8_t 0: kein Fehler, sonst: Fehler
 */
uint8_t getMultiregister(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* output_p, uint16_t outputLen){
	//TODO zu testen
	uint8_t result = NO_ERROR;
	if((dev_p==NULL)||(output_p==NULL)){
		result = NULL_POINTER;
	} else if (dev_p->initState==0)
	{
		result = HANDLER_NOT_INIT;
	} else{
		int8_t begin = searchReg(reg);
		int8_t end = searchEnd(begin, outputLen);
		if ((begin!=-1)&&(end!=-1)){
			uint8_t buffer[MAX_SIZE_FRAME] = {0};//Zwischenspeicherbuffer
			uint16_t positionPtr = 0;//der Zeiger zur nächsten freien Position, die Länge der nutzbaren Bytes
			outputLen = getTotalLen(begin,end);
			uint8_t processTime = outputLen/PAYLOAD_PER_FRAME + ((outputLen%PAYLOAD_PER_FRAME)?1:0);
			for (uint8_t i = 0; i<processTime; i++){
				positionPtr = 0;
				begin = begin + PAYLOAD_PER_FRAME*i;
				uuaslProtocolHeader_t header = protocolHeaderPrint(add,begin,UUASL_R_REQ);
				header.length = 8;//magic number: Bytes vom gesamten Protokoll: 1 für Datenlängenanfrage, 7 für übrigen
				positionPtr += sizeof(header)/sizeof(uint8_t);
				memcpy((&buffer[0]),(uint8_t*)&header,positionPtr);
				uint8_t dataSegLen = (outputLen<PAYLOAD_PER_FRAME)?outputLen:PAYLOAD_PER_FRAME;
				memcpy(&(buffer[positionPtr]),&dataSegLen,1);//magic number: Die Länge von den zu empfangenen Nutzdaten in Byte
				positionPtr += sizeof(dataSegLen)/sizeof(uint8_t);
				buffer[positionPtr++] = crc8Checksum(&dataSegLen,1,dev_p->crc8Polynom);
				buffer[positionPtr++] = 0xA6;
				//Datensenden
				(*(dev_p->transmitFunc_p))(buffer,positionPtr);
#if WAIT_FUNCTION_ACTIVE
				(*(dev_p->waitFunc_p))(FACTOR_TO_MICROSEC*CHARS_PER_FRAME*positionPtr*3/BAUDRATE_BAUD/2);//warten
#endif
				positionPtr=1;//hier dient als die zu empfangenen Daten
				(*(dev_p->receiveFunc_p))(buffer, positionPtr);
#if WAIT_FUNCTION_ACTIVE
				(*(dev_p->waitFunc_p))(FACTOR_TO_MICROSEC*CHARS_PER_FRAME*positionPtr*3/BAUDRATE_BAUD/2);//warten
#endif
				if (buffer[0]==0xA2){
					result = DATA_INVALID;
				} else if (buffer[0]==0xA5){
					positionPtr=4;//hier dient als die zu empfangenen Daten
					(*(dev_p->receiveFunc_p))(buffer, positionPtr);
#if WAIT_FUNCTION_ACTIVE
					(*(dev_p->waitFunc_p))(FACTOR_TO_MICROSEC*CHARS_PER_FRAME*positionPtr*3/BAUDRATE_BAUD/2);//warten
#endif
					//Checken Protokollrelevante Informationen
					bool checkRxDataInfo = (buffer[0]==add) && \
											(buffer[1]==header.rwaBytes.value[0]) && \
											((buffer[2]^header.rwaBytes.value[1])==0x40);
					if (checkRxDataInfo){
						positionPtr=buffer[3]-5;//Die Länge vom Header = 5
						(*(dev_p->receiveFunc_p))(buffer, positionPtr);
#if WAIT_FUNCTION_ACTIVE
						(*(dev_p->waitFunc_p))(FACTOR_TO_MICROSEC*CHARS_PER_FRAME*positionPtr*3/BAUDRATE_BAUD/2);//warten
#endif
						bool checkDataBlock = checkRxData(buffer,positionPtr-2,buffer[positionPtr-2],dev_p->crc8Polynom);
						bool checkEndByte = buffer[positionPtr-1]==0xA6;
						if (checkDataBlock&&checkEndByte){
							memcpy(&(output_p[i*PAYLOAD_PER_FRAME]),buffer,positionPtr-2);
						} else {
							result = PROCESS_FAIL;
							break;
						}
					} else {
						result = PROCESS_FAIL;
						break;
					}
				} else{
					result = PROCESS_FAIL;
					break;
				}
				
				//Immer am Ende der Schleife
				outputLen -= PAYLOAD_PER_FRAME;
			}
		} else{
			result = DATA_INVALID;
		}
	}
	return result;
}

/**
 * \brief zum Schreiben in vielen Registern nur mit einem Protokoll (bis zum 255 Bytes unterstützt)
 * 
 * \param add die Adresse vom geschriebenen Gerät
 * \param reg die Adresse des ersten Registers im geschriebenen Datenblock
 * \param dev_p der Zeiger zum Handler
 * \param input_p das Eingabebuffer
 * \param inputLen die Länge vom Eingabebuffer
 * 
 * \return uint8_t 0: kein Fehler, sonst: Fehler
 */

volatile uint8_t payload = PAYLOAD_PER_FRAME;
volatile int8_t begin=0;
volatile int8_t end=0;
volatile uint16_t lenTemp=0;
volatile uint8_t processTime = 0;

uint8_t setMultiregister(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* input_p, uint16_t inputLen){
	//TODO zu testen
	uint8_t result = NO_ERROR;
	if((dev_p==NULL)||(input_p==NULL)){
		result = NULL_POINTER;
	} else{
		/*volatile int8_t*/ begin = searchReg(reg);
		/*volatile int8_t*/ end = searchEnd(begin, inputLen);
		
		if ((begin != -1)&&(end>=begin)){
			uint8_t buffer[MAX_SIZE_FRAME] = {0};//Zwischenspeicherbuffer
			uint16_t positionPtr = 0;//der Zeiger zur nächsten freien Position, die Länge der nutzbaren Bytes
			lenTemp = getTotalLen(begin,end);
			//Anzahl der Protocol im abhängig von Nutzdaten (erwartet Nutzdaten = 247)
			
			/*uint8_t*/ processTime = (lenTemp/(uint16_t)PAYLOAD_PER_FRAME) + ((lenTemp%(uint16_t)PAYLOAD_PER_FRAME)?1:0);
			uint8_t temp = 0;
			for (volatile uint8_t i = 0; i<processTime; i++){
				positionPtr = 0;
				//Header im Gesamtarray kopieren
				begin = begin + ((int8_t)i)*payload;
				uuaslProtocolHeader_t head = protocolHeaderPrint(add,begin,UUASL_W_REQ);
				temp = (inputLen>payload)?payload:inputLen;
				head.length = temp+7;
				positionPtr+= sizeof(head)/sizeof(uint8_t);
				memcpy((&buffer[0]),(uint8_t*)&head,positionPtr);
				
				//TODO Inhalt im Gesamtarray kopieren
				memcpy((&buffer[positionPtr]),&(input_p[i*payload]),temp);
				positionPtr+=temp;
				
				//Tail im Gesamtarray kopieren
				uint8_t crc8 = crc8Checksum(&(input_p[i*payload]),temp,dev_p->crc8Polynom);
				uuaslProtocolTail_t tail = writeProtocolTailPrint(crc8);
				memcpy((&buffer[positionPtr]),(uint8_t*)&tail,2);//magic number: Länge des Endteils/Tails in Byte
				positionPtr += sizeof(tail)/sizeof(uint8_t);
				
				//Senden der Daten
				result = (*(dev_p->transmitFunc_p))(buffer,positionPtr);
#if WAIT_FUNCTION_ACTIVE
				(*(dev_p->waitFunc_p))(FACTOR_TO_MICROSEC*CHARS_PER_FRAME*positionPtr*3/BAUDRATE_BAUD/2);//warten
#endif
				positionPtr = 1;//hier dient als die zu empfangenen Daten
				(*(dev_p->receiveFunc_p))(buffer, positionPtr);
#if WAIT_FUNCTION_ACTIVE
				(*(dev_p->waitFunc_p))(FACTOR_TO_MICROSEC*CHARS_PER_FRAME*positionPtr*3/BAUDRATE_BAUD/2);//warten
#endif
				if (buffer[0]!=0xA1){
					result = PROCESS_FAIL;
					break;
				}
				//Immer am Ende der Schleife
				inputLen -= payload;
			}
		} else{
			result = DATA_INVALID;
		}
	}
	return result;
}



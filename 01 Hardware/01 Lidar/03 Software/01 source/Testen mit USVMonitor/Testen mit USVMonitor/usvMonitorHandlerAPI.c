/*
 * usvMonitorHandlerAPI.c
 *
 * Created: 7/7/2023 8:42:59 AM
 * Author: Thach
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

static const slaveReg_t regSet[]={
	//Sensorblock
	{SEN_GESB_ADD,1},
	{SEN_LONGNITUDE_ADD,4},
	{SEN_LATITUDE_ADD,4},
	{SEN_SATFIX_ADD,1},
	{SEN_GPS_VEL_ADD,2},
	{SEN_COURSE_ANGLE_ADD,2},
	{SEN_TIMESTAMP_ADD,3},
	//F�hrungsgr��en der Antriebsregelung
	{REF_DRV_CTRL_REF_A_ADD,8},
	{REF_DRV_CTRL_REF_B_ADD,8},
	{REF_DRV_CTRL_VEL_ADD,2},
	{REF_DRV_CTRL_EPS_ADD,2},
	//Stellgr��en der Antriebsregelung
	{CTRL_DRV_CTRL_THRUST_ADD,2},
	{CTRL_DRV_CTRL_RUDDER_ANGLE_ADD,2},
	//lokaler Error Block
	{ESB_GPS_ADD,1},
	{ESB_COMPASS_ADD,1},
	{ESB_CTRL_ADD,1}
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

//keine Fehler ausgeben, man muss die Gueltigkeit vom Index vor dem Aufrufchecken
static inline uuaslReadProtocol_t readProtocolPrint(uint16_t add,uint16_t index){
	uuaslReadProtocol_t result ={
		.header.start =	0xA5,
		.header.slaveRegAdd = regSet[index].add,
		.header.rwaBytes.value_bf.slaveAddL = GET_SLAVE_ADD_LOW_PART(add),
		.header.rwaBytes.value_bf.slaveAddH = GET_SLAVE_ADD_HIGH_PART(add),
		.header.rwaBytes.value_bf.rw = UUASL_R_REQ,
		.header.length = 8,
		.dataLen = regSet[index].len,
		.tail.end = 0xA6
	};
	result.tail.checksum = crc8Checksum(&(result.dataLen), sizeof(result.dataLen), CRC8_POLYNOM);
	return result;
}

//keine Fehler ausgeben, man muss die Gueltigkeit vom Index vor dem Aufrufchecken
static inline uuaslProtocolHeader_t writeProtocolHeaderPrint(uint16_t add,uint16_t index){
	uuaslProtocolHeader_t result = {
		.start = 0xA5,
		.slaveRegAdd = regSet[index].add,
		.rwaBytes.value_bf.slaveAddL = GET_SLAVE_ADD_LOW_PART(add),
		.rwaBytes.value_bf.slaveAddH = GET_SLAVE_ADD_HIGH_PART(add),
		.rwaBytes.value_bf.rw = UUASL_W_REQ,
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
 * \brief Zum Schreiben in einem Register im Slave-Ger�t
 * 
 * \param add Adresse vom Slave
 * \param reg die slavespezifische ID (Adresse von Register vom Slave)
 * \param dev_p der Zeiger zum Handler
 * \param input_p der Zeiger zur Eingabedaten 
 * \param length die L�nge der Eingabe
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
				uint8_t txProtocol[MAX_SIZE_FRAME];//Gesamtarray
				uint16_t positionPtr=0;
				//Header im Gesamtarray kopieren
				uuaslProtocolHeader_t head = writeProtocolHeaderPrint(add,index);
				memcpy((&txProtocol[positionPtr]),(uint8_t*)&head,sizeof(head)/sizeof(uint8_t));
				positionPtr+= sizeof(head)/sizeof(uint8_t);
				//Inhalt im Gesamtarray kopieren
				memcpy((&txProtocol[positionPtr]),input_p,length);
				positionPtr+=length;
				//Tail im Gesamtarray kopieren
				uint8_t crc8 = crc8Checksum(input_p,length,dev_p->crc8Polynom);
				uuaslProtocolTail_t tail = writeProtocolTailPrint(crc8);
				memcpy((&txProtocol[positionPtr]),(uint8_t*)&tail,sizeof(tail)/sizeof(uint8_t));
				positionPtr += sizeof(tail)/sizeof(uint8_t);
				//Senden die Daten
				result = (*(dev_p->transmitFunc_p))(txProtocol,positionPtr);
				(*(dev_p->waitFunc_p))(500);//warten 0,5ms f�r Datensendung
			}
		} else{
			result = DATA_INVALID;
		}
	}
	return result;
}

/**
 * \brief Empfangen die Daten aus dem Slave-Ger�t, inklusiv CRC-Byte
 * 
 * \param add die Adresse vom Slave
 * \param reg die slavespezifische ID (Adresse von Register vom Slave)
 * \param dev_p der Zeiger zum Handler
 * \param output der empfangene Datenteil im gelesenen Register zusammen mit der CRC8-Checksum-Datei
 * \param outputLen die L�nge der empfangenen Daten inklusiv der Checksum
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
			volatile uint8_t rxBuffer[MAX_SIZE_FRAME]={0};
			uint16_t rxLength=5;
			//Bildung von Datenrahmen
			volatile uuaslReadProtocol_t protocol = readProtocolPrint(add,index);
			memcpy((uint8_t*)rxBuffer,(uint8_t*)&protocol,sizeof(protocol)/sizeof(uint8_t));			
			(*(dev_p->transmitFunc_p))((uint8_t*)rxBuffer, sizeof(protocol)/sizeof(uint8_t));
			(*(dev_p->waitFunc_p))(100);//Warte 0,1ms
			//Nach dem Request-Senden, empfangen erste 5 Bytes
			(*(dev_p->receiveFunc_p))((uint8_t*)rxBuffer, rxLength);
			(*(dev_p->waitFunc_p))(300);//warte 0,3ms
			//checken erste Byte
			if(rxBuffer[0]==0xA2){
				result = DATA_INVALID;
			} else if(rxBuffer[0]==0xA5){ //Wenn erfolgreich, checken weitere 4 Bytes
				rxLength=5;
				//Byte 3 beim Daten lesen: Bei Hinprotokoll 0x4X, bei R�ckprotokoll 0x0X => 0x4X XOR 0x0X = 0x40
				bool checkData = (rxBuffer[1]==protocol.header.slaveRegAdd) && \
								((rxBuffer[2]^protocol.header.rwaBytes.value[0])==0x40) && \
								(rxBuffer[3]==protocol.header.rwaBytes.value[1]);
				if(!checkData){
					result = PROCESS_FAIL;
				} else{
					//empfangen weitere n Bytes sowieCRC- und Endbyte
					rxLength = rxBuffer[4]-7+2;//offset im Register = 7
					(*(dev_p->receiveFunc_p))((uint8_t*)rxBuffer, rxLength);
					(*(dev_p->waitFunc_p))(300);
					if (rxBuffer[rxLength-1]==0xA6){
						//kopieren aller Datenbytes und deren Checksum-Code
						rxLength--;
						memcpy(output_p,(uint8_t*)rxBuffer,rxLength);
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


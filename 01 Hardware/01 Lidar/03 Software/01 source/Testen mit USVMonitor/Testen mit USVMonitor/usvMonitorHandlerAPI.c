/*
 * usvMonitorHandlerAPI.c
 *
 * Created: 7/7/2023 8:42:59 AM
 * Author: Thach
 */

#include "usvMonitorHandlerAPI.h"

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
	{ESB_CTRL_ADD,1}
};

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

//keine Fehler protocol ausgeben, man muss die Gueltigkeit checken
static inline uuaslReadProtocol_t readProtocolPrint(uint8_t add,uint16_t index){
	uuaslReadProtocol_t result ={
		.header.start =	0xA5,
		.header.slaveRegAdd = regSet[index].add,
		.header.rwaBytes.value_bf.slaveAddH = GET_SLAVE_ADD_HIGH_PART(add),
		.header.rwaBytes.value_bf.slaveAddL = GET_SLAVE_ADD_LOW_PART(add),
		.header.rwaBytes.value_bf.rw = UUASL_R_REQ,
		.header.length = 8,
		.dataLen = regSet[index].len,
		.tail.end = 0xA6
	};
	result.tail.checksum = crc8(&(result.dataLen), sizeof(result.dataLen), CRC8_POLYNOM);
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
	} else{
		result = NULL_POINTER;
	}
	return result;
	
}

//Refaktirisierung in Bearbeitung
uint8_t setData(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev){
	uint8_t result = NO_ERROR;
	int8_t index = searchReg(reg);
	if (index != -1)
	{
		//TODO checken read-only
	} else{
		result = DATA_INVALID;
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
			//Bildung von Datenrahmen
			uuaslReadProtocol_t protocol = readProtocolPrint(add,index);			
			(*(dev_p->transmitFunc_p))((uint8_t*)&protocol, sizeof(protocol)/sizeof(uint8_t));
			(*(dev_p->waitFunc_p))(100);//Warte 0,1ms
			//Nach dem Request-Senden, empfangen erste 5 Bytes
			uint8_t rxBuffer[MAX_SIZE_FRAME]={0};
			uint16_t rxLength=5;
			(*(dev_p->receiveFunc_p))(rxBuffer, &rxLength);
			(*(dev_p->waitFunc_p))(300);//warte 0,3ms
			//checken erste Bye
			if(rxBuffer[0]==0xA2){
				result = DATA_INVALID;
			} else if(rxBuffer[0]==0xA5){ //Wenn erfolgreich, checken weitere 4 Bytes
				rxLength=5;
				//Byte 3 beim Daten lesen: Bei Hinprotokoll 0x4X, bei Rückprotokoll 0x0X => 0x4X XOR 0x0X = 0x40
				bool checkData = (rxBuffer[1]==protocol.header.slaveRegAdd) && \
								((rxBuffer[2]^protocol.header.rwaBytes.value[0])==0x40) && \
								(rxBuffer[3]==protocol.header.rwaBytes.value[1]);
				if(!checkData){
					result = PROCESS_FAIL;
				} else{
					//empfangen weitere n Bytes sowieCRC- und Endbyte
					rxLength = rxBuffer[4]-7+2;//offset im Register = 7
					(*(dev_p->receiveFunc_p))(rxBuffer, &rxLength);
					(*(dev_p->waitFunc_p))(300);
					if (rxBuffer[rxLength-1]==0xA6){
						//kopieren aller Datenbytes und deren Checksum-Code
						rxLength--;
						memcpy(output_p,rxBuffer,rxLength);
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


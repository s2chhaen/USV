/*
 * usvMonitor.c
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


/**
 * \brief Initalisierung des Handlers
 * 
 * \param dev_p der Zeiger zum betrachteten usVMonitorHandler
 * \param config_p der Zeiger zur erwuenschten USART-Konfiguration
 * 
 * \return uint8_t 0: erfolgreich, sonst: nicht
 */
uint8_t initDev(usvMonitorHandler_t* dev_p, usvMonitorConfig_t* config_p){
	uint8_t result = NO_ERROR;
	dev->statusObj.uart = config->usartNo;
	bool devInit = USART_init(config->usartNo,config->baudrate, config->bits, config->parity, config->stopbit, config->sync, config->MPCM,config->address, config->PortMux);
	USART_set_send_Array_callback_fnc(dev->statusObj.uart,config->tx_p);
	USART_set_Bytes_to_receive(dev->statusObj.uart,MAX_BYTE_SEND);
	USART_set_receive_Array_callback_fnc(dev->statusObj.uart,config->rx_p);
	dev->statusObj.initState = (!devInit)?1:0;
	result = (!devInit)?NO_ERROR:PROCESS_FAIL;
	return result;
}

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

uint8_t getData(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev){
	uint8_t result = NO_ERROR;
	int8_t index = searchReg(reg);
	if (index!=-1){
		uuaslReadProtocol protocol={
			.header.start =	0xA5,
			.header.slaveRegAdd = regSet[index].add,
			.header.rwaBytes.value_bf.slaveAddH = GET_SLAVE_ADD_HIGH_PART(add),
			.header.rwaBytes.value_bf.slaveAddL = GET_SLAVE_ADD_LOW_PART(add),
			.header.rwaBytes.value_bf.rw = UUASL_R_REQ,
			.header.length = 8,
			.dataLen = regSet[index].len,
			.tail.end = 0xA6,
		};
		protocol.tail.checksum = crc8(&(protocol.dataLen), sizeof(protocol.dataLen), CRC8_POLYNOM);
		//Bildung von Datenrahmen
		(*(dev->transmitFunc_p))((uint8_t*)&protocol, sizeof(protocol)/sizeof(uint8_t));
		(*(dev->waitFunc_p))(10000);//Warte 10ms
	} else{
		result = DATA_INVALID;
	}
	return result;
}


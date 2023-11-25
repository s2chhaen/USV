/*
 * usvDataHandler.c
 *
 * Created: 7/7/2023 8:42:59 AM
 * Author: Thach
 * Version: 1.4
 * Revision: 1.1
 */

#include "usvDataHandler.h"

//Refaktorisierung in Bearbeitung
const uint16_t usartFIFOMaxLen = _FIFO_max_def - 1;
//Protocolbereich
volatile uint8_t protocol[MAX_FRAME_LEN] = {0};
volatile uint8_t usv_protocolToHandleBytes = 0;
volatile uint8_t usv_protocolIdx = 0; //Idx: Index
//Data Bereich Temp-Buffer
volatile uint8_t usv_tempBuffer[400] = {0};
volatile uint16_t usv_tempBufferToHandleBytes = 0;
volatile uint16_t usv_tempBufferIdx = 0; 
//Data Bereich RX-Buffer
volatile uint8_t* usv_rxBuffer;
volatile uint16_t* usv_rxBufferStrLen;
volatile uint16_t usv_rxBufferToHandledBytes = 0;
volatile uint16_t usv_rxBufferIdx = 0;
volatile uint8_t usv_rxPayloadLen = 0;

//Verwaltungsbereich
volatile uint8_t usv_checksumPolynom = 0;
volatile usvMgr_t usv_mgr = {0};
volatile uint16_t usv_nextReg = 0;
volatile uint8_t usv_savedAddr = 0;

/**
 * \brief die Position des Registers in der Liste suchen
 * 
 * \param reg die Adresse des Registers
 * 
 * \return int8_t -1: kein gefunden, sonst: die Position in der Liste
 */
static inline int16_t getRegLen(uint16_t reg){
	int16_t result = 0;
	switch(reg){
		//Sensorblock
		case SEN_GESB_ADD:
			result = 1;//0
			break;
		case SEN_LONGNITUDE_ADD:
			result = 4;//1
			break;
		case SEN_LATITUDE_ADD:
			result = 4;//2
			break;
		case SEN_SATFIX_ADD:
			result = 1;//3
			break;
		case SEN_GPS_VEL_ADD:
			result = 2;//4
			break;
		case SEN_COURSE_ANGLE_ADD:
			result = 2;//5
			break;
		case SEN_TIMESTAMP_ADD:
			result = 3;//6
			break;
		//Radar
		case RADAR_1_ADD:
			result = 1;//7
			break;
		case RADAR_2_ADD:
			result = 2;//8
			break;
		//Führungsgrößen der Antriebsregelung
		case REF_DRV_CTRL_REF_A_ADD:
			result = 8;//9
			break;
		case REF_DRV_CTRL_REF_B_ADD:
			result = 8;//10
			break;
		case REF_DRV_CTRL_VEL_ADD:
			result = 2;//11
			break;
		case REF_DRV_CTRL_EPS_ADD:
			result = 2;//12
			break;
		//Stellgrößen der Antriebsregelung
		case CTRL_DRV_CTRL_THRUST_ADD:
			result = 2;//13
			break;
		case CTRL_DRV_CTRL_RUDDER_ANGLE_ADD:
			result = 2;//14
			break;
		//lokaler Error Block
		case ESB_GPS_ADD:
			result = 1;//15
			break;
		case ESB_COMPASS_ADD:
			result = 1;//16
			break;
		case ESB_CTRL_ADD:
			result = 1;//17
			break;
		//Lidar
		case LIDAR_VALUE_ADD:
			result = 361;
			break;
		default:
			result = -1;
			break;
	}
	return result;
}

static inline bool checkRxData(uint8_t* data,uint8_t dataLen, uint8_t rxChecksumValue, uint8_t crc8Polynom){
	return crc8CodeGen(data,(uint16_t)dataLen)==rxChecksumValue;
}

static inline uint8_t usv_setProtocol(uint8_t add, uint16_t reg, uint8_t* input_p, uint8_t length, uint8_t wr){
	protocol[USV_START_BYTE_POS] = USV_PROTOCOL_START_BYTE;
	protocol[USV_OBJ_ID_BYTE_POS] = add;
	protocol[USV_REG_ADDR_AND_WR_LBYTE_POS] = USV_PROTOCOL_SET_SLAVE_ADD_LOW(reg);
	protocol[USV_REG_ADDR_AND_WR_HBYTE_POS] = USV_PROTOCOL_SET_SLAVE_ADD_HIGH(reg,wr);
	protocol[USV_FRAME_LEN_BYTE_POS] = length+PROTOCOL_OVERHEAD_LEN;
	memcpy((uint8_t*)&(protocol[USV_DATA_BEGIN_POS]),input_p,length);
	protocol[USV_DATA_BEGIN_POS+length] = crc8CodeGen(input_p,(uint16_t)length);
	protocol[USV_DATA_BEGIN_POS+1+length] = USV_PROTOCOL_END_BYTE;
	return USV_DATA_BEGIN_POS+1+length+1;//begin bei 0
}

static inline void usv_sendProtocol(){//trigger send new protocol
	uint8_t temp1 = 0;
	usv_protocolIdx = 0;
	if (usv_protocolToHandleBytes < usartFIFOMaxLen){
		temp1 = usv_protocolToHandleBytes;
		usv_protocolToHandleBytes = 0;
		USART_send_Array(usv_mgr.usartNo, 0, (uint8_t*)(&protocol[0]), temp1);
	} else{
		temp1 = usv_protocolIdx;
		usv_protocolToHandleBytes -= usartFIFOMaxLen;
		usv_protocolIdx = usartFIFOMaxLen;
		USART_send_Array(usv_mgr.usartNo, 0, (uint8_t*)(uint8_t*)(&protocol[0]), usartFIFOMaxLen);
	}
}

volatile uint8_t usv_mode = USV_SETTER_MODE;
//FSM - Setter
volatile uint8_t usv_fsmState = USV_FSM_SETTER_START_STATE;
usv_fsmStateHandlerFunc_t usv_cbTable[USV_FSM_STATE_NUM] = {0};//cb: callback/Rückruf

//FSM - Getter
volatile uint8_t usv_getterFsmState = USV_FSM_GETTER_START_STATE;
usv_fsmStateHandlerFunc_t usv_getterCbTable[USV_FSM_GETTER_STATE_NUM] = {0};//cb: callback/Rückruf
	
uint8_t fsm_setterStartStateHandlerFunc(){
	uint8_t retVal = USV_FSM_SETTER_READY_STATE;
	if (usv_mgr.lock){
		retVal = USV_FSM_SETTER_START_STATE;
	}
	return retVal;
}

uint8_t fsm_setterReadyStateHandlerFunc(){
	usv_mode = USV_SETTER_MODE;
	return USV_FSM_SETTER_TX_STATE;
}

uint8_t fsm_setterTxStateHandlerFunc(){
	uint8_t retVal = USV_FSM_SETTER_TX_STATE;
	if (usv_protocolToHandleBytes){
		txTempData[0] = (uint8_t*)&(protocol[usv_protocolIdx]);
		if (usv_protocolToHandleBytes < txTempMax_length){
			*txTempLength =  usv_protocolToHandleBytes;
			usv_protocolToHandleBytes = 0;
		} else{
			*txTempLength = txTempMax_length;
			usv_protocolToHandleBytes -= txTempMax_length;
			usv_protocolIdx += txTempMax_length;
		}
	} else{
		usv_protocolIdx = 0;
		retVal = USV_FSM_SETTER_RX_STATE;
		USART_set_Bytes_to_receive(usv_mgr.usartNo,1);
	}
	return retVal;
}

uint8_t fsm_setterRxStateHandlerFunc(){
	uint8_t retVal = USV_FSM_SETTER_START_STATE;
	uint8_t check = (rxTempLength == 1) && (rxTempData[0] == USV_PROTOCOL_ACK_BYTE);
	if (check){
		if (usv_tempBufferToHandleBytes){
			if (usv_tempBufferToHandleBytes < PROTOCOL_PAYLOAD_PER_FRAME){
				usv_protocolToHandleBytes = usv_setProtocol(usv_savedAddr, usv_nextReg, (uint8_t*) (&usv_tempBuffer[usv_tempBufferIdx]), usv_tempBufferToHandleBytes, USV_PROTOCOL_W_REQ);
				usv_tempBufferToHandleBytes = 0;
				usv_tempBufferIdx = 0;
			} else{
				usv_protocolToHandleBytes = usv_setProtocol(usv_savedAddr, usv_nextReg, (uint8_t*) (&usv_tempBuffer[usv_tempBufferIdx]), PROTOCOL_PAYLOAD_PER_FRAME,USV_PROTOCOL_W_REQ);
				usv_nextReg += PROTOCOL_PAYLOAD_PER_FRAME;
				usv_tempBufferIdx += PROTOCOL_PAYLOAD_PER_FRAME;
				usv_tempBufferToHandleBytes -= PROTOCOL_PAYLOAD_PER_FRAME;		
			}
			retVal = USV_FSM_SETTER_TX_STATE;
			usv_sendProtocol();
		} else{
			usv_mgr.res = 0;
			usv_mgr.lock = 0;
			retVal =  USV_FSM_SETTER_START_STATE;
		}
	} else{
		usv_mgr.res = 1;
		usv_mgr.lock = 0;
		usv_tempBufferToHandleBytes = 0;
		usv_protocolToHandleBytes = 0;
	}
	return retVal;
}

	} else{
		const uint16_t maxLen = getRegLen(USV_LAST_DATA_BLOCK_ADDR) + USV_LAST_DATA_BLOCK_ADDR;
		uint16_t tempLen = (reg+inputLen);
		int8_t checkReg = (getRegLen(reg) != -1) && (tempLen <= maxLen);
		if (checkReg){
			uint8_t processTime = inputLen/PROTOCOL_PAYLOAD_PER_FRAME + ((inputLen%PROTOCOL_PAYLOAD_PER_FRAME)?1:0);
			uint16_t temp4;
			__asm__("nop");
			for (volatile uint8_t i = 0; i < processTime; i++){
				temp4 = (inputLen >PROTOCOL_PAYLOAD_PER_FRAME)?PROTOCOL_PAYLOAD_PER_FRAME:inputLen;
				result = setAndCheckData(add, reg+i*PROTOCOL_PAYLOAD_PER_FRAME, temp4,dev_p, &(input_p[i*PROTOCOL_PAYLOAD_PER_FRAME]), temp4);
				if (result==NO_ERROR){
					inputLen -= PROTOCOL_PAYLOAD_PER_FRAME;
				} else{
					result = PROCESS_FAIL;
					break;
				}
			}
			__asm__("nop");
		} else{
			result = DATA_INVALID;
		}
	}
	return result;
}

volatile uint8_t rxRes = 0;
#pragma GCC push_options
#pragma GCC optimize ("O0")
static inline uint8_t getAndCheckData(uint8_t add, uint16_t reg, uint16_t regLen, usvMonitorHandler_t* dev_p, uint8_t* output_p, uint16_t outputLen){
	uint8_t result = NO_ERROR;
	uint8_t temp1, temp2;
	uint16_t temp3;
	protocol[USV_START_BYTE_POS] = USV_PROTOCOL_START_BYTE;
	protocol[USV_OBJ_ID_BYTE_POS] = add;//slave-id = addr
	temp1 = USV_PROTOCOL_SET_SLAVE_ADD_LOW(reg);
	protocol[USV_REG_ADDR_AND_WR_LBYTE_POS] = temp1;
	temp2 = USV_PROTOCOL_SET_SLAVE_ADD_HIGH(reg,USV_PROTOCOL_R_REQ);
	protocol[USV_REG_ADDR_AND_WR_HBYTE_POS] = temp2;
	protocol[USV_FRAME_LEN_BYTE_POS] = PROTOCOL_OVERHEAD_LEN+1;
	protocol[USV_FRAME_LEN_BYTE_POS+1] = (uint8_t)regLen;//Datenteil = Länge des Registers
	protocol[USV_FRAME_LEN_BYTE_POS+2] = crc8Checksum((uint8_t*)&(protocol[USV_FRAME_LEN_BYTE_POS+1]),1,dev_p->crc8Polynom);//CRC8-Code
	protocol[USV_FRAME_LEN_BYTE_POS+3] = USV_PROTOCOL_END_BYTE;
	//Senden der Daten
	(*(dev_p->transmitFunc_p))((uint8_t*)protocol, USV_FRAME_LEN_BYTE_POS+3+1,(USV_FRAME_LEN_BYTE_POS+3+1)*BYTE_TRANSFER_TIME_US*2);//begin bei 0
	
	//Nach dem Request-Senden, empfangen erste Byte
	rxRes = (*(dev_p->receiveFunc_p))((uint8_t*)&(protocol[USV_START_BYTE_POS]), 1,BYTE_TRANSFER_TIME_US*1+DST_PROG_WORK_TIME_US);//magic number 0=timeout, 1=Anzahl der empfangenen Bytes
	if(protocol[USV_START_BYTE_POS] == USV_PROTOCOL_START_BYTE){ //Wenn erfolgreich, checken weitere 4 Bytes
		//Byte 4 beim Daten lesen: Bei Hinprotokoll 0x4X, bei Rückprotokoll 0x0X => 0x4X XOR 0x0X = 0x40
		rxRes = (*(dev_p->receiveFunc_p))((uint8_t*)&(protocol[USV_OBJ_ID_BYTE_POS]), 4,BYTE_TRANSFER_TIME_US*4);
		bool checkByteReg = add == protocol[USV_OBJ_ID_BYTE_POS];
		bool checkByteAddAndRw = (temp1 == protocol[USV_REG_ADDR_AND_WR_LBYTE_POS]) && ((temp2 ^ protocol[USV_REG_ADDR_AND_WR_HBYTE_POS])==0x40);
		bool checkAll = checkByteReg && checkByteAddAndRw;
		if(checkAll){
			//empfangen weitere n Datenbytes sowie 1 CRC8- und 1 Endbyte
			rxRes = (*(dev_p->receiveFunc_p))((uint8_t*)&(protocol[USV_DATA_BEGIN_POS]), protocol[USV_FRAME_LEN_BYTE_POS]-5, (protocol[USV_FRAME_LEN_BYTE_POS]-5)*BYTE_TRANSFER_TIME_US);
			temp3 = USV_DATA_BEGIN_POS + protocol[USV_FRAME_LEN_BYTE_POS] - 7;
			bool checkDataBlock = checkRxData((uint8_t*)&(protocol[USV_DATA_BEGIN_POS]),protocol[USV_FRAME_LEN_BYTE_POS] - 7, protocol[temp3],dev_p->crc8Polynom);
			bool checkEnd = protocol[temp3+1] == USV_PROTOCOL_END_BYTE;
			if (checkEnd&&checkDataBlock){
				//kopieren aller Datenbytes in Ausgabe
				memcpy(output_p,(uint8_t*)&(protocol[USV_DATA_BEGIN_POS]),(protocol[USV_FRAME_LEN_BYTE_POS]-7));
			} else {
				result = PROCESS_FAIL;
			}
		} else{
			result = PROCESS_FAIL;
		}
	} else{
		result = PROCESS_FAIL;
	}
	return result;
}
#pragma GCC pop_options

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
#pragma GCC push_options
#pragma GCC optimize ("O3")
uint8_t getData(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* output_p, uint16_t outputLen){
	//TODO zu optimieren
	uint8_t result = NO_ERROR;
	if((dev_p==NULL)||(output_p==NULL)){
		result = NULL_POINTER;
	} else if (dev_p->initState==0)
	{
		result = HANDLER_NOT_INIT;
	} else{
		
		int16_t regLen = getRegLen(reg);
		if (regLen!=-1){
			result = getAndCheckData(add, reg, (uint16_t) regLen, dev_p, output_p, outputLen);
		} else{
			result = DATA_INVALID;
		}
	}
	return result;
}
#pragma GCC pop_options

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
uint8_t usv_initDev(usartConfig_t config, uint8_t crc8Polynom){
#pragma GCC push_options
#pragma GCC optimize ("O3")
uint8_t getMultiregister(uint8_t add, uint16_t reg, usvMonitorHandler_t* dev_p, uint8_t* output_p, uint16_t outputLen){
	uint8_t result = NO_ERROR;
	uint8_t temp = config.usartNo;
	usv_mgr.init = USART_init(temp,config.baudrate, config.usartChSize, config.parity, config.stopbit, config.sync, config.mpcm,config.address, config.portMux);
	usv_mgr.usartNo = temp;
	result = !usv_mgr.init;
	usv_checksumPolynom = crc8Polynom;
	crc8Init(crc8Polynom);
	USART_set_send_Array_callback_fnc(temp,&usartCallbackTx);
	USART_set_receive_Array_callback_fnc(temp,&usartCallbackRx);
	//Setter - Mode
	usv_cbTable[USV_FSM_SETTER_START_STATE] = &fsm_setterStartStateHandlerFunc;
	usv_cbTable[USV_FSM_SETTER_READY_STATE] = &fsm_setterReadyStateHandlerFunc;
	usv_cbTable[USV_FSM_SETTER_TX_STATE] = &fsm_setterTxStateHandlerFunc;
	usv_cbTable[USV_FSM_SETTER_RX_STATE] = &fsm_setterRxStateHandlerFunc;
	//Getter - Mode
	usv_getterCbTable[USV_FSM_GETTER_START_STATE] = &fsm_getterStartStateHandlerFunc;
	usv_getterCbTable[USV_FSM_GETTER_READY_STATE] = &fsm_getterReadyStateHandlerFunc;
	usv_getterCbTable[USV_FSM_GETTER_TX_STATE] = &fsm_getterTXStateHandlerFunc;
	usv_getterCbTable[USV_FSM_GETTER_RX_CHECK_1_OHD_STATE] = &fsm_getterRX1stCheckHandlerFunc;
	usv_getterCbTable[USV_FSM_GETTER_RX_CHECK_2_OHD_STATE] = &fsm_getterRX2ndCheckHandlerFunc;
	usv_getterCbTable[USV_FSM_GETTER_RX_DATA_STATE] = &fsm_getterRXDataHandlerFunc;
	usv_getterCbTable[USV_FSM_GETTER_RX_CHECK_3_OHD_STATE] = &fsm_getterRX3rdCheckHandlerFunc;
	return result;
}

uint8_t usv_setRegister(uint8_t add, uint16_t reg, uint8_t* input_p, uint16_t length){//Nur Protokoll senden
	uint8_t retVal = NO_ERROR;
	const uint16_t maxLen = getRegLen(USV_LAST_DATA_BLOCK_ADDR) + USV_LAST_DATA_BLOCK_ADDR;
	uint8_t check = (usv_cbTable[USV_FSM_SETTER_START_STATE]() == USV_FSM_SETTER_READY_STATE) && (usv_fsmState == USV_FSM_SETTER_START_STATE);
	if (check){ //TODO test again
		usv_mgr.lock = 1;
		check = ((reg+length) <= maxLen); // Länge check
		usv_mgr.res = 0;
		if (check){
			int16_t regLen = getRegLen(reg);
			if (regLen != -1){
				usv_protocolIdx = 0;
				usv_tempBufferIdx = 0;
				usv_nextReg = 0;
				memcpy((uint8_t*)usv_tempBuffer,input_p,length);
				if (length > PROTOCOL_PAYLOAD_PER_FRAME){
					usv_protocolToHandleBytes = usv_setProtocol(add,reg,(uint8_t*)(&usv_tempBuffer[0]),PROTOCOL_PAYLOAD_PER_FRAME,USV_PROTOCOL_W_REQ);
					usv_savedAddr = add;
					usv_nextReg = reg + PROTOCOL_PAYLOAD_PER_FRAME;
					usv_tempBufferIdx += PROTOCOL_PAYLOAD_PER_FRAME;
					usv_tempBufferToHandleBytes = length - PROTOCOL_PAYLOAD_PER_FRAME;
				} else{
					usv_protocolToHandleBytes = usv_setProtocol(add,reg,(uint8_t*)(&usv_tempBuffer[0]),length,USV_PROTOCOL_W_REQ);
					usv_tempBufferToHandleBytes = 0;
				}
				usv_fsmState = usv_cbTable[USV_FSM_SETTER_READY_STATE]();
				usv_sendProtocol();
			} else{
				usv_fsmState = USV_FSM_SETTER_START_STATE;
				usv_mgr.lock = 0;
				retVal = PROCESS_FAIL;
			}
		} else{
			usv_fsmState = USV_FSM_SETTER_START_STATE;
			usv_mgr.lock = 0;
			retVal = PROCESS_FAIL;
		}
	} else{
		retVal = PROCESS_FAIL;
	}
	return retVal;
}

				} else{
					break;
				}
			}
		} else{
			result = DATA_INVALID;
		}
	}
	return result;
}
#pragma GCC pop_options



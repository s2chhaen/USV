/*
 * usart1Beta.c
 *
 * Created: 6/27/2023 2:29:15 PM
 *  Author: Dan
 */ 

#include "usart1Beta.h"

usartModuleHAL_t obj1 = {
	.txFIFO = preinit_FIFO, 
	.adr_p = &(USART1)
};

//number of bytes, that is readable
static inline uint8_t readableBytesNumber(){
	return ((obj1.txFIFO.p_write - obj1.txFIFO.p_read + obj1.txFIFO.FIFO_max)%obj1.txFIFO.FIFO_max);
}

//number of bytes, that is writable
static inline uint8_t writableBytesNumber(){
	return ((obj1.txFIFO.p_read - obj1.txFIFO.p_write + obj1.txFIFO.FIFO_max-1)%obj1.txFIFO.FIFO_max);
}

//true = empty
static inline bool emptyCheck(){
	return (obj1.txFIFO.p_read == obj1.txFIFO.p_write);
}

//true = full
static inline bool fullCheck(){
	return (((obj1.txFIFO.p_read - obj1.txFIFO.p_write + obj1.txFIFO.FIFO_max-1)%obj1.txFIFO.FIFO_max)==0);
}

static inline void writeToFifo(uint8_t* input, uint8_t length){
	for (uint8_t i = 0; i<length;i++){
		obj1.txFIFO.dFIFO.data[obj1.txFIFO.p_write] = input[i];
		obj1.txFIFO.p_write++;
	}
}

static inline void readFromFifo(uint8_t* output){
	*output = obj1.txFIFO.dFIFO.data[obj1.txFIFO.p_read];
	obj1.txFIFO.p_read++;
}

static void getRegister8Bit(registerUSART_t name, register8Bit_t* output){
	switch(name){
		case STATUS_en:
			output = (register8Bit_t*)(&(obj1.adr_p->STATUS));
			break;
		case CTRLA_en:
			output = (register8Bit_t*)(&(obj1.adr_p->CTRLA));
			break;
		case CTRLB_en:
			output = (register8Bit_t*)(&(obj1.adr_p->CTRLB));
			break;
		case CTRLC_en:
			output = (register8Bit_t*)(&(obj1.adr_p->CTRLB));
			break;
		case CTRLD_en:
			output = (register8Bit_t*)(&(obj1.adr_p->CTRLC));
			break;
		default:
			break;
	}
}

static void getRegister16Bit(registerUSART_t name, register16Bit_t* output){
	switch(name){
		case BAUD_en:
			output = (register16Bit_t*)(&(obj1.adr_p->CTRLB));
			break;
		default:
			break;
	}
}

#define POLLING_METHOD 1
void initUsart1(){
	cli();
	CTRLA_t ctrlaConfig = {
		.Register.RXCIE = 0,
#ifdef POLLING_METHOD
		.Register.TXCIE = 0,
#else
		.Register.TXCIE = 1,
#endif
		.Register.DREIE = 0,
		.Register.RXSIE = 0,
		.Register.LBME = 0,
		.Register.ABEIE = 0,
		.Register.RS485 = 0,
	};
	CTRLB_t ctrlbConfig = {
		.Register.RXEN = 1,
		.Register.TXEN = 1,
		.Register.SFDEN = 1,
		.Register.ODME = 0,
		.Register.RXMODE = 0,
		.Register.MPCM = 0
	};
	CTRLC_t ctrlcConfig = {
		.Reg_AsyncMode.CMODE = 0x0,
		.Reg_AsyncMode.PMODE = 0x03,
		.Reg_AsyncMode.SBMODE = 0x0,
		.Reg_AsyncMode.CHSIZE = 0x03
	};
	//Ordnung BAUD,C,B,A
	PORTMUX.USARTROUTEA = 0;//choose Portmux default
	obj1.adr_p->BAUD = 256;//set 250kBaud;
	PORTC.DIR |= (1<<0);//Set TX/PC0 as output
	PORTC.PIN0CTRL |= (0x4<<0);//Disable input in PC0
	obj1.adr_p->DBGCTRL = 0x00;//set debug
	obj1.adr_p->EVCTRL = 0x00;//set event control
	obj1.adr_p->CTRLC = ctrlcConfig.value;//Asynchronous, 8Bit Frame, 1 stop-bit, odd Parity
	obj1.adr_p->CTRLB = ctrlbConfig.value;//no MPC, set Tx and Rx
	obj1.adr_p->CTRLA = ctrlaConfig.value;//Set Tx and Rx Interrupt
}
//using polling method
#ifdef POLLING_METHOD
uint8_t usartSendData(uint8_t* data, uint8_t length){
	uint8_t result = 0;
	length = length%32;//now can only transfer 32 Byte per time
	//another way
	STATUS_t* temp=NULL;
	getRegister8Bit(STATUS_en,(register8Bit_t*)temp);
	//while(!((obj1.adr_p->STATUS)&(1<<5)));//check if DREIF  = 1 (empty)
	while(temp->Register.DREIF == 0);
	obj1.adr_p->TXDATAL = data[0];//Can only transmit 1 byte per time
	return result
}
#else
uint8_t usartSendData(uint8_t* data, uint8_t length){
	uint8_t result = 0;
	if (length){
		uint8_t freeBytes = writableBytesNumber();
		if ((freeBytes+1)<length){
			result = LENGTH_EXCESS;
		} else if (data == NULL){
			result = NULL_POINTER;
		} else{
			STATUS_t* temp=NULL;
			getRegister8Bit(STATUS_en,(register8Bit_t*)temp);
			while((temp->Register.DREIF == 0)||(!emptyCheck()));
			obj1.adr_p->TXDATAL = data[0];
			if (length!=1){
				writeToFifo(&(data[1]),length-1);
			}
		}
	}
	return result;
}

ISR(USART1_RXC_vect){
	if (!emptyCheck()){
		uint8_t temp = 0;
		readFromFifo(&temp);
		obj1.adr_p->TXDATAL = temp;
	}
	 
}

#endif

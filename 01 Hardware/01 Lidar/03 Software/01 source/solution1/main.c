/*
 * solution1.c
 *
 * Created: 11.03.2023 16:31:29
 * Author : Riccardo
 * Bearbeiter: Thach
 */ 


#include "main.h"

/* Header vor dem Senden der eigentlichen Daten: 06 02 80 D6 02 B0 69 01 
anschließend folgen 722 Datenbyte --> 361 Datenpunkte, jeweils LSB / MSB

UART RS232 - ATMega: iUSART0
UART Slave - ATMega: iUSART1
UART RS422 - ATMega: iUSART2

*/

/*Globale Variablen**********************************************************
*uint8_t ack : wird mit Acknowledge Antwort vom Slave überschrieben
*uint8_t *datastream : enthält die vom USART empfangenen Sensordaten
*uint8_t *filtered_data : enthält die gefilterten Sensordaten (nach Finden des Headers)
*uint8_t *distances : enthält die berechneten Distanzen in cm
*uint8_t *rounded_distances : enthält die auf 0,5m gerundeten Distanzen
*uint8_t *converted_distances : enthält die von cm in m konvertierten Distanzen gemäß Forderung
*bool state = false : Abarbeitungsstatus der empfangenen Daten; ist true, wenn Daten gesendet werden können
*************************************************************/
volatile uint8_t ack = 0x0;
volatile uint8_t datastream[MAX_FRAME_LENGTH] = {0};
volatile uint8_t filtered_data[DATA_STREAM_SIZE] = {0};
volatile uint8_t distances[DATA_STREAM_SIZE] = {0};
volatile uint8_t rounded_distances[DATA_STREAM_SIZE] = {0};
volatile uint8_t converted_distances[DATA_STREAM_SIZE] = {0};
volatile bool state = false;
volatile uint16_t counter = 0;
static uint16_t txDataLength = 0;


//Implementation von CRC16-Checksum durch Anpassung Muster-Program in Buch von Hersteller
uint16_t checksumCrc16(uint8_t input[], uint32_t length){
	const uint16_t genPolynom = 0x8005;
	uint16_t uCrc16=0;
	uint8_t temp[]={0,0};
	for (uint32_t i=0;i<length;i++){
		temp[1]=temp[0];
		temp[0]=input[i];
		if (uCrc16&0x8000){
			uCrc16 = (uCrc16&0x7fff)<<1;
			uCrc16 ^= genPolynom;
			} else{
			uCrc16<<=1;
		}
		uCrc16 ^= (temp[0]|(temp[1]<<8));
	}
	return uCrc16;
}

/*CRC8**********************************************************
*Funktion bildet eine Checksumme
*************************************************************/
uint8_t CRC8( uint8_t *addr, uint8_t len){
	uint8_t crc = 0;
	while (len--){
		uint8_t inbyte = *addr++;
		uint8_t i;
		for (i = 8; i; i--){
			uint8_t mix = ( crc ^ inbyte ) & 0x80;
			crc <<= 1;
			if (mix)
			{
				crc ^= 0xD5;	// Generatorpolynom 0xD5
			}
			inbyte <<= 1;
		}
	}
	return crc;
}

//refactoring of function daten_empfangen
bool dataReceive(uint8_t adress, uint8_t data[], uint8_t length){
	bool result = true;
	/**
	uint16_t config = NO_OF_VALUE|(PARTIAL_SCAN_00<<11)|(PARTIAL_SCAN<<13)|(CM_U<<14);
	uint8_t configL = (uint8_t)(config&0x00FF);
	uint8_t configH = (uint8_t)((config&0xFF00)>>8);
	const headerInst_t inst1 = {{ACK_SYMBOL, STX_SYMBOL, ADR_SLV, LENGTH_LOW , LENGTH_HIGH, DATA_REQ_RESP,configL,configH}};
	USART_set_Bytes_to_receive(iUSART0,0);
	for (int i = 0;i<length;i++){
		datastream[i] = data[i];
	}

	for (int i = 0;i<HEADER_LENGTH;i++){
		if (datastream[i]!=inst1.data[i]){
			result = false;
			break;
		}
	}
	if(result){
		for (int i = HEADER_LENGTH;i<HEADER_LENGTH+722;i++){
			filtered_data[i] = datastream[i];
		}
		//processVal_t byteConvert = bytes_to_values((uint8_t *)filtered_data,MAX_FRAME_LENGTH,(uint8_t *)distances,DATA_STREAM_SIZE);
		//processVal_t roundValue = round_values((uint8_t *)distances,DATA_STREAM_SIZE);
		//processVal_t distancesConvert = cm_to_m((uint8_t *)rounded_distances,DATA_STREAM_SIZE);
		state = true;
	}
	USART_set_Bytes_to_receive(iUSART1,1);
	******/
	return result;
}

bool dataSend(uint8_t adress, uint8_t data[], uint8_t length){
	//static uint8_t counter=3;
	bool result = false;
	/***
	if((data[0]==NACK_SYMBOL)&&(counter)){
		length = sizeof(converted_distances);
		result = USART_send_Array(iUSART1 , 0, (uint8_t *)converted_distances, length);
		counter--;
	} else{
		ack = (data[0]==ACK_SYMBOL)?1:0;
		counter = 3;
	}
	*****/
	counter--;
	return result;
}

//refactoring of function setup()
uint8_t init(){
	uint8_t result;
	uint8_t prescaler = 1;
	uint16_t rxLength = 900;
	uint16_t txLength = 900;
	uint32_t baudrateSlave = 250000;
	init_Core_CLK(INTERN_CLK,prescaler);
	//result = USART_init(iUSART1,250000, USART_CHSIZE_8BIT_gc, USART_PMODE_ODD_gc, USART_SBMODE_1BIT_gc, DISBL_SYNC_TX, DISBL_MPC_MODE, 0, PORTMUX_USARTx_DEFAULT_gc);
	result = initDev(rxLength, txLength, iUSART1, baudrateSlave, USART_CHSIZE_8BIT_gc, USART_PMODE_ODD_gc, USART_SBMODE_1BIT_gc, DISBL_MPC_MODE, DISBL_MPC_MODE, 0, PORTMUX_USARTx_DEFAULT_gc);
	sei();//active the global interrupt, weil es bisher nicht aktiviert wird
	return result;
}

int main(void) {
	//uint8_t testMsg[16]={0x02,0x00,0x0A,0x00,0x20,0x00,0x53,0x49,0x43,0x4B,0x5F,0x4C,0x4D,0x53,0x5F,0xB2};
	//txDataLength = sizeof(testMsg)/sizeof(uint8_t);
#define DATA_FOR_TEST 1
//#define TEST_USART_HAL 1

#ifdef DATA_FOR_TEST
#define MAX_VALUE_TEST 64
	uint8_t test[MAX_VALUE_TEST]={0};
	for (int i = 0; i < MAX_VALUE_TEST; i++){
		test[i] = i;
	}
	txDataLength = sizeof(test)/sizeof(uint8_t);
#endif

	volatile bool test1=false;
	volatile uint8_t i=0;
	init();
#ifdef TEST_USART_HAL
	USART_send_Array(iUSART1, 0x0, test, txDataLength);
#else
	dataTx(test,txDataLength);
#endif	

    while (1){
		i=0;
		if (!test1){
			i++;
		}
    }
	return 0;
}


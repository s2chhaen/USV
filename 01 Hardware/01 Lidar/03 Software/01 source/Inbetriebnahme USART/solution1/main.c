/*
 * solution1.c
 *
 * Created: 11.03.2023 16:31:29
 * Author : Thach
 */ 


#include "main.h"


/**
 * \brief Bilden einer Checksum-Code für ein beliebiges Datenrahmen
 * \param input das zu checkende Array (Datenrahmen)
 * \param length seine Länge
 * \return uint16_t das Checksum-Code für das eingegebene Array
 */
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

/**
 * \brief Initalisierung für das ganze Programm
 * \return uint8_t 0:Fehlerfrei, sonst Fehler
 */
static uint8_t init(){
	uint8_t result;
	uint8_t prescaler = 1;
	uint16_t rxLength = 900;
	uint16_t txLength = 900;
	uint8_t timerResUs = 1;
	uint16_t timerPre = 1024;
	uint32_t baudrateSlave = 76800;
	//CPU-Init
	init_Core_CLK(INTERN_CLK,prescaler);
	//USART-Beobachter- und Stopuhr-Einheit-Init
	timerInit(timerResUs,timerPre);
	//Slave-Device-Init
	result = initDev(rxLength, txLength, iUSART1, baudrateSlave, USART_CHSIZE_8BIT_gc, USART_PMODE_ODD_gc, USART_SBMODE_1BIT_gc, DISBL_MPC_MODE, DISBL_MPC_MODE, 0, PORTMUX_USARTx_DEFAULT_gc);
	sei();//aktiviert das global Interrupt, weil es bisher nicht aktiviert wird
	return result;
}

#ifdef ACTIVE_ECHO_TEST
#define DATA_LENGTH 100
static uint8_t data[DATA_LENGTH]={0};
static uint16_t rxLength = sizeof(data)/sizeof(uint8_t);

/**
 * \brief empfangen eine Zeichenfolge mit beliebiger Länge und schicke sie mit ihrer Länge zurück 
 * \return void
 */
static void echoTest(){
	volatile bool checkReceive = dataRx((uint8_t*)data,(uint16_t*)&rxLength)==NO_ERROR;
	volatile uint16_t temp1 = 48;
	volatile uint16_t temp2 = 48;
	if(checkReceive){
		dataTx((uint8_t*)data,(uint16_t)rxLength);
		waitUs(1000);
		temp1+=rxLength/10;
		temp2+=rxLength%10;
		data[0]=temp1;
		data[1]=temp2;
		dataTx((uint8_t*)data,2);
		rxLength = sizeof(data)/sizeof(uint8_t);
		memset((uint8_t*)data,0,rxLength);
	}
}
#endif

int main(void) {
	init();
#ifdef ACTIVE_HAL_USART_TEST_SEG_1
#define MAX_VALUE_TEST 31
	uint8_t test[MAX_VALUE_TEST]={0};
	for (int i = 0; i < MAX_VALUE_TEST; i++){
		test[i] = i;
	}
	uint8_t txDataLength= sizeof(test)/sizeof(uint8_t);
	USART_send_Array(iUSART1, 0x0, test, txDataLength);
#endif
#ifdef ACTIVE_HAL_USART_TEST_SEG_2
#define MAX_LENGTH_BUFFER 50
	USART_set_Bytes_to_receive(iUSART1,31);
	volatile uint8_t rxBuffer[MAX_LENGTH_BUFFER]={0};
	volatile uint8_t* temp[1];
	temp[0] = &rxBuffer;
	volatile uint8_t rxLength;
	volatile uint8_t rxPtr=0;
	uint8_t adr = 0;
#endif
		
#ifdef ACTIVE_SLV_DEVICE_SEND_TEST_SEG
	#define MAX_VALUE_TEST 31
	uint8_t test[MAX_VALUE_TEST]={0};
	for (int i = 0; i < MAX_VALUE_TEST; i++){
		test[i] = i;
	}
	uint8_t txDataLength= sizeof(test)/sizeof(uint8_t);
	dataTx(test,txDataLength);
#endif
	volatile uint8_t ix = 0;
#ifdef ACTIVE_HAL_USART_TEST_SEG_2
	volatile bool checkRx = false;
#endif
    while (1){
		ix++;
#ifdef ACTIVE_HAL_USART_TEST_SEG_2
		checkRx = USART_receive_Array(iUSART1,&adr,(uint8_t**)temp,MAX_LENGTH_BUFFER,(uint8_t*)&rxLength);
		if(rxLength!=0){
			USART_send_Array(iUSART1, 0x0, rxBuffer,rxLength);
		}
#endif
#ifdef ACTIVE_ECHO_TEST
		echoTest();
#endif
    }
	return 0;
}


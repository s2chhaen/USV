// communication.c
// name:		communication.h
// description:	Dieses Modul bietet Funktionen zum Abrufen und
//				Schreiben von Daten von/auf dem Bus
// TODOs:
//				- SlaveID anpassen


#include "communication.h"
#include <stdlib.h>
#include "debug.h"

UART_HandleTypeDef handleUart;


// --- Einmalige Uebergabe des UART-Handle ---
void initCom(UART_HandleTypeDef handle){
	handleUart = handle;
}


// --- Aktualisiert den gesamten Sensorgroessen-Block (lesen vom Bus) ---
status updateSensorBlock(localData* pLocalData){
	uint8_t tmp[17];

	status Status = readData(busaddr_gesb, tmp, 17);
	if(Status == STATUS_OK){						// busaddr_gesb ist die erste adresse des blocks, 17 = Anzahl der Bytes, die empfangen werden sollen
		(*pLocalData).gesb = tmp[0];

		(*pLocalData).currPos.lon = convFixedToF32(&tmp[1], 4, 9, 17);		// 4 = Anzahl der Bytes, 9 = pIntegerBits, 17 = qFractionalBits

		(*pLocalData).currPos.lat = convFixedToF32(&tmp[5], 4, 9, 17);

		(*pLocalData).currGPSSpeed = convFixedToF32(&tmp[10], 2, 8, 8);		// SatFix bei temp[9] ->wird nicht benoetigt

		(*pLocalData).currHeadingAngle = convFixedToF32(&tmp[12], 2, 9, 7);

		(*pLocalData).timestamp = 0;
		(*pLocalData).timestamp = tmp[14];			// sekunden
		(*pLocalData).timestamp |= (((uint32_t)tmp[15]) << 8); 	// minute
		(*pLocalData).timestamp |= (((uint32_t)tmp[16]) << 16); // stunde
	}
	return Status;
}


// --- Aktualisiert den gesamten Fuehrungsgroessen-Block (lesen vom Bus) ---
status updateFuehrungsgrBlock(localData* pLocalData){
	uint8_t tmp[20];

	status Status = readData(busaddr_coordA, tmp, 20);
	if(Status == STATUS_OK){						// busaddr_coordA ist die erste adresse des blocks
		(*pLocalData).pointA.lat = convFixedToF32(&tmp[0], 4, 9, 17);
		(*pLocalData).pointA.lon = convFixedToF32(&tmp[4], 4, 9, 17);

		(*pLocalData).pointB.lat = convFixedToF32(&tmp[8], 4, 9, 17);
		(*pLocalData).pointB.lon = convFixedToF32(&tmp[12], 4, 9, 17);

		(*pLocalData).targetSpeed = convFixedToF32(&tmp[16], 2, 8, 8);

		(*pLocalData).epsilon = convFixedToF32(&tmp[18], 2, 8, 8);
	}
	return Status;
}


// --- Aktualisiert einzeln alle Werte, die gelesen werden ---
void updateLocalData(localData* pLocalData){
	uint8_t tmp[8];

	if(readData(busaddr_gesb, tmp, 1) == STATUS_OK){
		(*pLocalData).gesb = tmp[0];
	}

	if(readData(busaddr_targetSpeed, tmp, 2) == STATUS_OK){
		(*pLocalData).targetSpeed = convFixedToF32(&tmp[0], 2, 8, 8);
	}

	if(readData(busaddr_speedGPS, tmp, 2) == STATUS_OK){
		(*pLocalData).currGPSSpeed = convFixedToF32(&tmp[0], 2, 8, 8);
	}

	if(readData(busaddr_headingAngle, tmp, 2) == STATUS_OK){
		(*pLocalData).currHeadingAngle = convFixedToF32(&tmp[0], 2, 9, 7);
	}

	if(readData(busaddr_epsilon, tmp, 2) == STATUS_OK){
		(*pLocalData).epsilon = convFixedToF32(&tmp[0], 2, 8, 8);
	}

	if(readData(busaddr_timestamp, tmp, 3) == STATUS_OK){
		(*pLocalData).timestamp = tmp[0];
	}

	if(readData(busaddr_longitudeGPS, tmp, 4) == STATUS_OK){
		(*pLocalData).currPos.lon = convFixedToF32(&tmp[0], 4, 9, 17);
	}

	if(readData(busaddr_latitudeGPS, tmp, 4) == STATUS_OK){
		(*pLocalData).currPos.lat = convFixedToF32(&tmp[0], 4, 9, 17);
	}

    if(readData(busaddr_coordA, tmp, 8) == STATUS_OK){
		(*pLocalData).pointA.lat = convFixedToF32(&tmp[0], 4, 9, 17);
		(*pLocalData).pointA.lon = convFixedToF32(&tmp[4], 4, 9, 17);
	}

    if(readData(busaddr_coordB, tmp, 8) == STATUS_OK){
    	(*pLocalData).pointB.lat = convFixedToF32(&tmp[0], 4, 9, 17);
    	(*pLocalData).pointB.lon = convFixedToF32(&tmp[4], 4, 9, 17);
   	}
}


// --- Fordert Daten einzeln vom Bus an und speichert diese ---
status readData(busaddr addr, uint8_t* pDB, uint8_t lndB){
	// --- Sendebuffer erstellen ---
	uint8_t txDB[8];												// maximalen Speicher für txDatenbuffer (txDB) reservieren = 8 Bytes

	// --- txDatenbuffer fuellen ---
	txDB[0] = STARTBYTE;											// Startbyte
	txDB[1] = SLAVEID;												// SLAVEID
	txDB[3] = (READDATA + addr) >> 8;								// Daten-schreiben-Command (0x8000) + Adresse (12bit) -->hoechsten 8 bit (4Bit-Command und hoechsten 4 Bit Adresse)
	txDB[2] = (READDATA + addr);									// Daten-schreiben-Command (0x8000) + Adresse (12bit) -->niedrigsten 8 bit (niedrigsten 8 Bit Adresse)
	txDB[4] = 8;													// Framelaenge = 8 Bytes
	txDB[5] = lndB;													// Datenlaengenanfrage
	txDB[6] = CRC8(&txDB[5], 1);									// Checksumme von der Datenlängenanfrage bilden: CRC8(Datenlänge, Anzahl der Datenbytes)
	txDB[7] = ENDBYTE;												// Endbyte

	// --- Datenbuffer ueber UART senden ---
	HAL_StatusTypeDef TxStatusResult = HAL_UART_Transmit(&handleUart, (uint8_t *)txDB, 8, TIMEOUT_TIME);
	if (TxStatusResult != HAL_OK){
		sendStrToUSB("ErrorTX\r", 8);
		if (TxStatusResult == HAL_TIMEOUT)
			sendStrToUSB("Timeout\r", 8);
		return STATUS_TX_ERROR;
	}

	// --- Empfangsbuffer erstellen ---
	uint8_t rxDB[7+lndB];											// maximalen Speicher für rxDatenbuffer (rxDB) reservieren

	// --- Empfangsbuffer ueber UART-Funktion fuellen ---
	HAL_StatusTypeDef RxStatusResult = HAL_UART_Receive(&handleUart, (uint8_t *)rxDB, 7+lndB, TIMEOUT_TIME);
	if (RxStatusResult != HAL_OK){
		sendStrToUSB("ErrorRX\r", 8);
		if (RxStatusResult == HAL_TIMEOUT)
			sendStrToUSB("Timeout\r", 8);
		return STATUS_RX_ERROR;										// Letzte empfangene Byte ist nicht das Endbyte
	}

	// --- Relevante Daten in Databuffer extrahieren ---
	for (uint8_t i=0; i<lndB; i++){
		pDB[i] = rxDB[5+i];
	}

	// --- Empfangene Daten auswerten ---
	if (rxDB[0] != STARTBYTE)							// Startbyte auswerten
		return STATUS_ERROR;
	if (rxDB[1] != SLAVEID)								// SlaveID auswerten
		return STATUS_ERROR;
	if (rxDB[3] != (uint8_t)(addr >> 8))				// Null-Bits (0x0) und die ersten 4 Adressbits auswerten
		return STATUS_ERROR;
	if (rxDB[2] != txDB[2])								// niedrigsten 8 Bits der Adresse auswerten
		return STATUS_ERROR;
	if (rxDB[4] != (lndB+7))							// Framelange auswerten
		return STATUS_ERROR;
	if (CRC8(pDB, lndB) != rxDB[4+lndB+1])				// Checksumme auswerten
		return STATUS_ERROR;
	if (rxDB[4+lndB+2] != ENDBYTE)						// Endbyte auswerten
		return STATUS_ERROR;

	return STATUS_OK;
}


// --- Aktualisiert den gesamten Stellgroessen-Block (schreiben auf den Bus) ---
status updateStellgrBlock(localData* pLocalData){
	uint8_t tmp[4];

	convF32ToFixed((*pLocalData).thrust, tmp, 2, 1, 15);
	convF32ToFixed((*pLocalData).rudder, &tmp[2], 2, 1, 15);

	return sendData(busaddr_thrust, tmp, 4); 					// busaddr_thrust ist die erste adresse des blocks
}


// --- Aktualisiert einzeln alle Werte, die geschrieben werden ---
void updateBusData(localData* pLocalData){
	uint8_t tmp[2];

	convF32ToFixed((*pLocalData).rudder, tmp, 2, 1, 15); 			// 2 = Anzahl der Bytes, 1 = pIntegerBits, 15 = qFractionalBits, 1 = signed
	sendData(busaddr_rudder, tmp, 2);

	convF32ToFixed((*pLocalData).thrust, tmp, 2, 1, 15);
	sendData(busaddr_thrust, tmp, 2);

	sendData(busaddr_esb, &(*pLocalData).esb, 1);
}


// --- Sendet Daten an den Bus ---
status sendData(busaddr addr, uint8_t* pDB, uint8_t n){
	// --- Sendebuffer erstellen ---
	uint8_t txDB[7+n];							// maximalen Speicher für txDatenbuffer (txDB) reservieren

	// --- txDatenbuffer fuellen ---
	txDB[0] = STARTBYTE;						// Startbyte
	txDB[1] = SLAVEID;							// SlaveID
	txDB[3] = (WRITEDATA + addr) >> 8;			// Daten-schreiben-Command (0x8000) + Adresse (12bit) -->hoechsten 8 bit (4Bit-Command und hoechsten 4 Bit Adresse)
	txDB[2] = (WRITEDATA + addr);				// Daten-schreiben-Command (0x8000) + Adresse (12bit) -->niedrigsten 8 bit (niedrigsten 8 Bit Adresse)
	txDB[4] = 7 + n;							// Framelaenge

	for(uint8_t i=0; i<n; i++)	{				// die zu sendenden Datenbytes in txDatenbuffer schreiben
		txDB[5+i] = pDB[i];
	}

	txDB[5+n] = CRC8(&txDB[5], n);				// Checksumme von den Datenbytes erstellen: CRC(Daten, Anzahl Datenbytes)
	txDB[5+n+1] = ENDBYTE;						// Endbyte

	// --- Datenbuffer ueber UART senden ---
	HAL_StatusTypeDef TxStatusResult = HAL_UART_Transmit(&handleUart, (uint8_t *)txDB, (5+n+1+1), TIMEOUT_TIME);
	if (TxStatusResult != HAL_OK)
		return STATUS_TX_ERROR;

	// --- Empfangsbuffer erstellen ---
	uint8_t rxDB = 0;							// fuer das Empfangen der USV-Slave-Antwort

	// --- Antwort des USV-Slave empfangen (ACK oder NACK) ---
	HAL_StatusTypeDef RxStatusResult = HAL_UART_Receive(&handleUart, &rxDB, sizeof(rxDB), TIMEOUT_TIME);
	if (RxStatusResult != HAL_OK)
		return STATUS_RX_ERROR;

	// --- Antwort auswerten ---
	if (rxDB == ACK)							// ACK: Acknowledgement
		return STATUS_OK;

	return STATUS_ERROR;						// Wenn Antwort vom Slave: NACK (No Acknowledgement)
}


// --- Berechnet die CRC-8 Check-Summe ---
uint8_t CRC8(uint8_t* data, uint8_t size){
	  uint8_t i;
	  uint8_t polynom = 0xD5;
	  uint8_t crc = 0x00;

	  while(size--){
	    crc = crc ^ *data++;					// crc-Byte mit Daten bitweises XOR (1^1=0, 0^0=0, 1^0=1, 0^1=1)

	    for(i=0; i<8; i++){
	      if (crc & 0x80)
	        crc = (crc << 1) ^ polynom;
	      else
	        crc = (crc << 1);
	    }
	  }

	  return crc;
}

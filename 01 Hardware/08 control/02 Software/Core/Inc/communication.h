// name:		communication.h
// description:	Dieses Modul bietet Funktionen zum Abrufen und
//				Schreiben von Daten von/auf dem Bus

#ifndef INC_COMMUNICATION_H_
#define INC_COMMUNICATION_H_

#include "stdio.h"
#include "calculation.h" 		// fuer coord_t
#include "conversion.h" 		// fuer Umwandlung Fixed <-> Float
#include "stm32g4xx_hal.h"  	// fuer Status wie HAL_OK

#define TIMEOUT_TIME 300

// Konstanten Frameaufbau
#define STARTBYTE 0xA5
#define ENDBYTE 0xA6
#define SLAVEID 0x01 //TODO
#define WRITEDATA 0x8000
#define READDATA 0x4000
#define ACK 0xA1
#define NACK 0xA2

// lokales Datenabbild vom Bus
typedef struct{
	uint8_t gesb;			// globales Error Status Byte
	float currPosHeight;	// aktuelle Hoehe ueber NN
	float currGPSSpeed;		// aktuelle GPS Geschwindigkeit in m/s
	float currHeadingAngle;	// aktueller Kurswinkel in rad
	uint32_t timestamp;		// Zeitstempel
	coord_t currPos;		// Koordinaten aktuelle Position in Dezimalgrad
	coord_t pointA;			// Koordinaten Punkt A in Dezimalgrad
	coord_t pointB;			// Koordinaten Punkt A in Dezimalgrad
	//coord_t pointTP;		// ...gehoert nicht zum Datenabbild!
	float targetSpeed;		// Fuehrungsgroesse Geschwindigkeit in m/s
	float epsilon;			// Epsilon
	float thrust;			// Stellwert Schub
	float rudder;			// Stellwert Ruder
	uint8_t esb;			// Error Status Byte
} localData;

// Adresswerte des Bus
typedef enum{
	busaddr_gesb = 0x000U,			// globales Error Status Byte
	busaddr_longitudeGPS = 0x001U, 	// aktuelle Pos. Laengengrad
	busaddr_latitudeGPS = 0x005U, 	// aktuelle Pos. Breitengrad
	busaddr_speedGPS = 0x00AU,		// aktuelle GPS-Geschwindigkeit
	busaddr_headingAngle = 0x00CU,	// aktueller Kurswinkel
	busaddr_timestamp = 0x00E,		// Zeitstempel
	busaddr_coordA = 0x100U,		// Koordinate Punkt A
	busaddr_coordB = 0x108U,		// Koordinate Punkt B
	busaddr_targetSpeed = 0x110U,	// Soll-Geschwindigkeit
	busaddr_epsilon = 0x112U,		// Epsilon-Wert
	busaddr_thrust = 0x120U,		// Stellwert Schub
	busaddr_rudder = 0x122U,		// Stellwert Ruder
	busaddr_esb = 0x202U			// Error Status Byte
} busaddr;

// Statuswerte der Uebertragung
typedef enum{
	STATUS_OK,
	STATUS_TIMEOUT,
	STATUS_ERROR,
	STATUS_TX_ERROR,
	STATUS_RX_ERROR
} status;


// description: Einmalige Uebergabe des UART-Handle
// param: 		UART-Handle
// return:		-
void initCom(UART_HandleTypeDef);

// description: Aktualisiert den gesamten Sensorgroessen-Block
//				(lesen vom Bus)
// param: 		Datenziel
// return:		-
status updateSensorBlock(localData*);

// description: Aktualisiert den gesamten Fuehrungsgroessen-Block
//				(lesen vom Bus)
// param: 		Datenziel
// return:		-
status updateFuehrungsgrBlock(localData*);

// description: Aktualisiert den gesamten Stellgroessen-Block
//				(schreiben auf den Bus)
// param: 		Datenquelle
// return:		-
status updateStellgrBlock(localData*);

// description:	Routine zum Empfangen, Speichern und Konvertierung von Daten
// param:		Datenziel
// return:		-
void updateLocalData(localData*);

// description:	Routine zum Senden und Konvertieren von Daten
// param:		Datenquelle
// return:		-
void updateBusData(localData*);

// description:	Fordert Daten vom Bus an und speichert diese
//				(wird in updateLocalData genutzt)
// param: 		Busadresse, Datenziel, Anzahl der Bytes
// return: 		Status
//status readData(busaddr, localData*, UART_HandleTypeDef);
status readData(busaddr , uint8_t* , uint8_t );

// description:	Sendet Daten an den Bus
//				(wird in updateBusData genutzt)
// param: 		Busadresse, Datenquelle, Anzahl der Bytes
// return: 		Status
//status sendData(busaddr, localData*, UART_HandleTypeDef);
status sendData(busaddr , uint8_t* , uint8_t );

// description:	Berechnet die CRC-8 Check-Summe (wird in sendData
//				und readData genutzt)
// param: 		Daten, Datengroese
// return: 		CRC-8 Check-Summe
uint8_t CRC8(uint8_t*, uint8_t);

#endif /* INC_COMMUNICATION_H_ */

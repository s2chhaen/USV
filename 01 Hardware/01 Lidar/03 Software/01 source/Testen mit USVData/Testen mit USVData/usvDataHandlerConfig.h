/*
 * usvDataHandlerConfig.h
 *
 * Created: 7/30/2023 11:05:30 PM
 *  Author: Thach
 * Version: 1.0
 * Revision: 1.1
 */ 


#ifndef USVDATAHANDLERCONFIG_H_
#define USVDATAHANDLERCONFIG_H_

/*Konfigurationsparameter*/
#define FACTOR_TO_MICROSEC 1000000UL
#define BAUDRATE_BAUD 250000UL
#define CHARS_PER_FRAME 11

/*Wenn die Send- und Empfangfunktion keinen Vorgangskontrollmechanismus besitzt, dann muss man die
* statische Verzögerungsfunktion verwenden, zur Warte auf die Prozesserledigung*/
#define WAIT_FUNCTION_ACTIVE 0



#endif /* USVDATAHANDLERCONFIG_H_ */
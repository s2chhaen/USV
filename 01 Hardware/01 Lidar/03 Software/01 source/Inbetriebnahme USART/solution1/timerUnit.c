/*
 * timerUnit.c: Quellcode für das timerUnit-Modul
 *
 * Created: 6/29/2023 11:57:34 PM
 * Author: Thach
 * Version: 1.1
 * Revision: 1.1
 */ 

#include "timerUnit.h"

//Zuweisung der Zähler/Timer Typ A von Mikrocontroller zur Verwaltungsobjekt des Moduls
volatile timer_t objTCA ={
	.adr=&(TCA0),
	.resolutionUs=1
};

#ifdef ACTIVE_USART_WATCHER
slaveDevice_t* obj_p;//Zeiger zur der zu beobachtenden USART-Einheit,für dieses Modul ist nur eine USART-Einheit notwendig
volatile uint32_t usartWatcher = 0;//USART-Einheitswächter,für dieses Modul ist nur eine USART-Einheit notwendig
#endif

volatile tickGenerator counter[NO_OF_SUBTIMER];//Zähler-Array


/**
 * \brief Wiederherstellung aller Zähler
 * 
 * 
 * \return void
 */
static void resetAllGenerator(){
	for (uint8_t i = 0; i<NO_OF_SUBTIMER;i++)
	{
		counter[i].value = 0;
		counter[i].lock = 0;
	}
}


/**
 * \brief Suche eines freien Zähler im Array
 * 
 * 
 * \return int8_t der erste freie Zähler im Array, -1: keine gefunden
 */
static int8_t searchFreeGenerator(){
	int result = -1;
	for (uint8_t i = 0;i<NO_OF_SUBTIMER;i++){
		if(!counter[i].lock){
			result = i;
			break;
		}
	}
	return result;
}

/**
 * \brief Befreiung eines gesperrten Zählers
 * 
 * \param i die Position des Zählers im Array
 * 
 * \return void
 */
static inline void unlockGenerator(uint8_t i){
	counter[i].lock = 0;
}

/**
 * \brief Initalization des Moduls
 * 
 * \param resolutionUs die erwünschte Auflösung
 * \param prescaler der Prescacler zur Bestimmung der Taktfrequenz vom Timer
 * 
 * \return void
 */
void timerInit(uint8_t resolutionUs, uint16_t prescaler){
	objTCA.adr->SINGLE.INTCTRL &= ~(1<<0);//Vorlaeufig deaktiviert wird Overflow-Interrupt
	resetAllGenerator();//Alle Zähler wiederhergestellt
#ifdef ACTIVE_USART_WATCHER
	usartWatcher = 0;//Für dieses Modul ist nur eine USART-Einheit notwendig
#endif
	//Prescaler aufgerundet
	if (prescaler==0)
	{
		prescaler = 1;
	} else if ((prescaler>2)&&(prescaler<4))
	{
		prescaler = 4;
	} else if ((prescaler>4)&&(prescaler<8))
	{
		prescaler = 8;
	} else if ((prescaler>8)&&(prescaler<16))
	{
		prescaler = 16;
	} else if ((prescaler>16)&&(prescaler<64))
	{
		prescaler = 64;
	} else if ((prescaler>64)&&(prescaler<256))
	{
		prescaler = 256;
	} else if ((prescaler>256)&&(prescaler<1024))
	{
		prescaler = 1024;
	} else{
		prescaler = 1024;
	}
	//Konfiguration
	TCA0_CTRLA_t configCTRLA;
	switch(prescaler){
		case 1:
			configCTRLA.valueBitField.CLKSEL = DIV1;
			break;
		case 2:
			configCTRLA.valueBitField.CLKSEL = DIV2;
			break;
		case 4:
			configCTRLA.valueBitField.CLKSEL = DIV4;
			break;
		case 8:
			configCTRLA.valueBitField.CLKSEL = DIV8;
			break;
		case 16:
			configCTRLA.valueBitField.CLKSEL = DIV16;
			break;
		case 64:
			configCTRLA.valueBitField.CLKSEL = DIV64;
			break;
		case 256:
			configCTRLA.valueBitField.CLKSEL = DIV256;
			break;
		case 1024:
			configCTRLA.valueBitField.CLKSEL = DIV1024;
			break;
		default:
			configCTRLA.valueBitField.CLKSEL = DIV1;
			break;
	}
	configCTRLA.valueBitField.ENABLE = 1;
	TCA0_INTCTRL_t configINTCTRL = {.valueBitField.OVF=1};
	objTCA.resolutionUs = resolutionUs;
	//Berechnung des Wertes für PER (Periode)
	uint16_t value = (uint16_t)((CLK_CPU/prescaler)*(resolutionUs));
	uint16_t compensation = (CLK_CPU%prescaler)?1:0;
	objTCA.adr->SINGLE.PER = (value+compensation);
	objTCA.initStatus = 1;
	//immer am Ende
	objTCA.adr->SINGLE.CTRLA = configCTRLA.value;//Modul aktiviert
	objTCA.adr->SINGLE.INTCTRL = configINTCTRL.value;//Overflow-Interrupt wird wieder aktiviert 
}

#ifdef ACTIVE_USART_WATCHER

/**
 * \brief Hinzufüge der zu überwachenden USART-Einheit (von Slave-Gerät/slaveDevice-Modul) 
 * \warning nur aktiv und verwendbar beim Text-Beendung mit timeout Modus vom slaveDevice-Modul
 *
 * \param input_p der Zeiger zum slaveDevice-Modul
 * 
 * \return uint8_t 0: kein Fehler, sonst: Fehler
 */
uint8_t setWatchedObj(slaveDevice_t *input_p){
	uint8_t result = NO_ERROR;
	if (input_p!=NULL)
	{
		obj_p = input_p;
	} else{
		result = NULL_POINTER;
	}
	return result;
}

/**
 * \brief Festlegung der Zeit für Empfangen eines Bytes
 * \warning nur aktiv und verwendbar beim Text-Beendung mit timeout Modus vom slaveDevice-Modul
 *
 * \param us die erwünschte Zeit in Mikrosekunden
 * 
 * \return void
 */
void setUsartWatcherTimeout(uint32_t us){
	if (obj_p!=NULL){
		objTCA.adr->SINGLE.INTCTRL &= ~(1<<0);//Vorlaeufig deaktiviert wird Overflow-Interrupt
		usartWatcher = us/objTCA.resolutionUs + (us%objTCA.resolutionUs)?1:0;
		objTCA.adr->SINGLE.INTCTRL |= (1<<0);//Overflow-Interrupt wird wieder aktiviert
	}
}

/**
 * \brief Rückgabe der bleibenden Zeit zum Empfangen eines Bytes
 * \warning nur aktiv und verwendbar beim Text-Beendung mit timeout Modus vom slaveDevice-Modul
 * 
 * \return uint32_t die bleibende Zeit zum Empfangen eines Bytes
 */
uint32_t getUsartWatcherTimeout(){
	return usartWatcher;
}

#endif

/**
 * \brief Verzögerung der Programmausführung in einem bestimmten Zeitraum
 * 
 * \param us die erwünschte Verzögerungszeit in Mikrosekunden
 * 
 * \return uint8_t 0:erfolgreich durchgeführt, sonst: Fehler
 */
uint8_t waitUs(uint32_t us){
	uint8_t result = NO_ERROR;
	int8_t i = 0;
	i = searchFreeGenerator();//suchen die freie Stelle
	//Falls gefunden, macht weiter sonst gibt Fehler zurueck
	if (i!=-1){
		objTCA.adr->SINGLE.INTCTRL &= ~(1<<0);//Vorlaeufig deaktiviert wird Overflow-Interrupt
		counter[i].lock = 1;
		counter[i].value = us/objTCA.resolutionUs + (us%objTCA.resolutionUs)?1:0;
		objTCA.adr->SINGLE.INTCTRL |= (1<<0);//Overflow-Interrupt wird wieder aktiviert 
		while (counter[i].value);
	} else{
		result = ALL_SLOT_FULL;
	}
	return result;
}

/**
 * \brief Verzögerung der Programmausführung in einem bestimmten Zyklen
 * \warning noch zu verbessern, weil die Zyklen nicht echtzeitig
 *
 * \param cycle die erwünschte Verzögerungszeit in Zyklen
 * 
 * \return void
 */
void waitCycle(uint32_t cycle){
	//TODO: noch zu verbessern, weil die Zyklen nicht echtzeitig
	for (uint32_t i = 0; i<cycle;i++);
}

/**
 * \brief Interrupt-Service-Routine für Overflow-Interrupt von TCA0
 * \detailed beim Stopuhr: Nach einer Zeit von resolutionUs wird der Wert vom Counter dekrementiert
 *  bis zum 0. Beim USART-Wächter: nach einer Zeit von timeout-Zeit beendet es eine Zeichenfolge
 *  und beginnt neues Empfangen einer neuen Zeichenfolge, wenn kein Zeichen mehr empfängt
 */
ISR(TCA0_OVF_vect){
#ifdef ACTIVE_USART_WATCHER
	uint8_t loopMax = MAX(NO_OF_SUBTIMER,NO_OF_USART);
#else
	uint8_t loopMax = NO_OF_SUBTIMER;
#endif
	for (int i = 0; i<loopMax;i++){
		if (i<NO_OF_SUBTIMER){
			if (counter[i].lock){
				counter[i].value--;
			}
			if (!counter[i].value){
				unlockGenerator(i);
			}
		}
#ifdef ACTIVE_USART_WATCHER
		if (obj_p!=NULL){
			if (i<NO_OF_USART){
				if (usartWatcher){
					usartWatcher--;
				} else{
					//Lese-Flag checken
					if ((obj_p->rxObj.rxByte[obj_p->rxObj.writeFIFOPtr] != 0)){
						//noch nicht gelesen, dann schreibt in naechste leere Zelle, wenn keine leere mehre, dann Voll-Flag gesetzt
						obj_p->rxObj.writeFIFOPtr = (obj_p->rxObj.writeFIFOPtr+1)%NO_OF_RX_BUFFER;
						obj_p->statusObj.rxFIFOState = (obj_p->rxObj.writeFIFOPtr == obj_p->rxObj.readFIFOPtr)?FULL:FILLED;
					}
				}
			}
		}
#endif
	}
	objTCA.adr->SINGLE.INTFLAGS |=  TCA_SINGLE_OVF_bm;//Löschen von Interrupt-Flag
}


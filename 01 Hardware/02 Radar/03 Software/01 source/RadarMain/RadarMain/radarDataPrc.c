/*
 * radarDataPrc.c
 * Description: Quellcode-Datei der radarDataPrc.c-Datei
 * Created: 12/19/2023 5:57:53 PM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.2
 */ 

#include "radarDataPrc.h"

/**
 * \brief Umwandlung der Geschwindigkeit und des Abstandes in Q4.4 und UQ7.4-Format
 * und Speichern der beiden in einem Ausgabe-Buffer 
 *
 * \param vel die Geschwindigkeit in Floating-Point-Format
 * \param dis der Abstand in Floating-Point-Format
 * \param output_p das Ausgabe-Buffer
 * \param outputLen die Länge des Ausgabe-Buffers
 * 
 * \return extern void
 */
void data_compareNReturn(float vel, float dis, uint8_t* output_p, uint8_t outputLen){
	if (outputLen >= 3){//TODO test again
		int8_t tempVel = (int8_t)(vel * (1 << VEL_FIXED_POINT_BIT));
		uint16_t tempDis = (uint16_t) (dis *(1 << DIS_FIXED_POINT_BIT));
		output_p[0] = tempDis & 0xff;
		output_p[1] = tempDis << 8;
		memcpy((void*)&output_p[2],(void*)&tempVel,1);
	}
}
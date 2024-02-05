/*
 * radarDataPrc.c
 *
 * Created: 12/19/2023 5:57:53 PM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.1
 */ 

#include "radarDataPrc.h"

extern void data_compareNReturn(float vel, float dis, uint8_t* output_p, uint8_t outputLen){
	if (outputLen >= 3){
		int8_t tempVel = (int8_t)(vel * (1 << VEL_FIXED_POINT_BIT));
		uint16_t tempDis = (uint16_t) (dis *(1 << DIS_FIXED_POINT_BIT));
		output_p[0] = tempDis & 0xff;
		output_p[1] = tempDis << 8;
		memcpy((void*)&output_p[2],(void*)&tempVel,1);
	}
}
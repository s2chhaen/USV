/*
 * radarDataPrc.h
 *
 * Created: 12/19/2023 5:54:04 PM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.1
 */ 


#ifndef RADARDATAPRC_H_
#define RADARDATAPRC_H_

#include <stdint.h>
#include <string.h>

/* Abkürzungen: PRC: Processing, VEL:velocity,
 * Dis: Distance
 */

//Nachkomma (Fraction-Part)-Param
#define VEL_FIXED_POINT_BIT 4
#define DIS_FIXED_POINT_BIT 4

extern void data_compareNReturn(float vel, float dis, uint8_t* output_p, uint8_t outputLen);


#endif /* RADARDATAPRC_H_ */
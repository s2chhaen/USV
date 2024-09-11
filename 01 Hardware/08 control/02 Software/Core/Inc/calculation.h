// name:		calculation.h

#ifndef INC_CALCULATION_H_
#define INC_CALCULATION_H_
#include <math.h>
#include "stdio.h"

// Typ zum speichern von Koordinaten im Dezimalgradformat
typedef struct{
	float lat; // Breitengrad (latitude)
	float lon; // Laengengrad (longitude)
} coord_t;

typedef struct{
	float x;
	float y;
} vec2D_t;

// Hilfsfunktionen fuer TP
vec2D_t subVec(float, float, float, float);
vec2D_t mulVecScalar(float, float, float);
float mulVecDot(float, float, float, float);
float lenVec(float, float);

// description:	Berechnet die Entfernung zwischen 2 Koordinaten
// param: 		Koordinate_A, Koordinate_B
// return: 		Luftlinienentfernung
//float calcDistance(coord_t, coord_t);

// description:	Berechnet die Luftlinie zwischen 2 Koordinaten
//				-> kuerzester Weg auf einer Kugel zw. 2 Punkten
// param: 		Koordinate_A, Koordinate_B
// return: 		Luftlinienentfernung
float calcOrthodromicDistance(coord_t, coord_t); // beeline, linear distance


// description:	Berechnet den Kurswinkel (Peilung) um von Koordinate A nach
//				Koordinate B zu kommen
// param: 		Koordinate_A, Koordinate_B
// return: 		Winkel in grd (Wertebereich 0 bis 360)
float calcBearing(coord_t, coord_t);

// description:	Berechnet den Mittelwert der im Buffer befindlichen
//				Werte
// param: 		Buffer, Size
// return: 		arithmetisches Mittel
float calcMean(uint32_t *buffer, uint8_t);

// description:	Berechnet einen Zwischenpunkt auf der idealen Bahn,
//				welcher dazu dient die oertliche Abweichung von der Bahn
//				zu minimieren
//				...Berechnung mittels euklidische Geometrie
// param: 		Koordinate_A, Koordinate_B, Boot-Position
// return: 		Zwischenpunkt/Zielpunkt/Richtungspunkt
coord_t calcTargetpoint(coord_t, coord_t, coord_t);



#endif /* INC_CALCULATION_H_ */

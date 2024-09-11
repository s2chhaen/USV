// calculation.c

#include "calculation.h"

float EARTHRADIUS = 6378.137; // Aequatorradius in km (WGS 84)

float DEG_TO_RAD = (M_PI / 180);
float RAD_TO_DEG = (180 / M_PI);


float calcOrthodromicDistance(coord_t coordA, coord_t coordB){
	// Breite
	coordA.lat *= DEG_TO_RAD;
	coordB.lat *= DEG_TO_RAD;
	// Laenge
	coordA.lon *= DEG_TO_RAD;
	coordB.lon *= DEG_TO_RAD;

	float distance = EARTHRADIUS * acos(sin(coordA.lat) * sin(coordB.lat) + cos(coordA.lat) * cos(coordB.lat) * cos(coordB.lon - coordA.lon));
	return distance;
}


float calcBearing(coord_t coordA, coord_t coordB){
	// Breite
	coordA.lat *= DEG_TO_RAD;
	coordB.lat *= DEG_TO_RAD;
	// Laenge
	coordA.lon *= DEG_TO_RAD;
	coordB.lon *= DEG_TO_RAD;

	// https://www.delius-klasing.de/media/pdf/0e/3b/d3/978-3-88412-494-9-nautische-formelsammlung-navigation-stand-12-2016.pdf
	float bearing = atan2(sin(coordB.lon-coordA.lon)*cos(coordB.lat),(cos(coordA.lat)*sin(coordB.lat))-
			(sin(coordA.lat)*cos(coordB.lat)*cos(coordB.lon-coordA.lon)));
	bearing = fmod((bearing*RAD_TO_DEG + 360.0), 360);
	return bearing;
}


float calcMean(uint32_t * buffer, uint8_t size){
	float mean = 0;
	for (uint8_t i = 0 ; i < size; i++) {
		mean += buffer[i];
	}
	mean /= size;
	return mean;
}


// Hilfsfunktionen fuer TP
vec2D_t subVec(float x1, float y1, float x2, float y2){
	vec2D_t result = {x1-x2, y1-y2};
	return result;
}

vec2D_t mulVecScalar(float x, float y, float scalar){
	vec2D_t result = {x*scalar, y*scalar};
	return result;
}

float mulVecDot(float x1, float y1, float x2, float y2){
	return x1*x2+y1*y2;
}

float lenVec(float x, float y){
	return sqrtf(x*x + y*y);
}

coord_t calcTargetpoint(coord_t coordA, coord_t coordB, coord_t coordPos){
	// Naeherungsweise Berechnung in der euklidische Ebene
	float distLat = 71.5; 	// 71,5km Abstand zwischen 2 Laengenkreisen
	float distLon = 111.3; 	// 111,3km Abstand zwischen 2 Breitenkreisen

	// Vektor AB: Start-Pos. zu Ziel-Pos.
	vec2D_t AB = subVec(coordB.lat*distLon, coordB.lon*distLat, coordA.lat*distLon, coordA.lon*distLat); // getauscht
	float lenAB = lenVec(AB.x, AB.y);

	// Einheitsvektor Richtung Start-Pos. zu Ziel-Pos.
	vec2D_t eAB = mulVecScalar(AB.x, AB.y, 1/lenAB);

	// Vektor AC: Start-Pos. zu aktuelle Pos.
	vec2D_t AC = subVec(coordPos.lat*distLon, coordPos.lon*distLat, coordA.lat*distLon, coordA.lon*distLat);

	// Vektor AD: Start-Pos. zu akt. Pos auf Bahn projeziert
	vec2D_t AD = mulVecScalar(eAB.x, eAB.y, mulVecDot(AC.x, AC.y, eAB.x, eAB.y));
	float lenAD = lenVec(AD.x, AD.y);

	// Vektor CD: Aktuelle Pos. zu akt. Pos auf Bahn projeziert
	vec2D_t CD = subVec(AD.x, AD.y, AC.x, AC.y);

	// Abweichung von idealer Bahn (Lot von akt. Pos [Punkt] auf ideale Bahn [Gerade])
	float error = lenVec(CD.x, CD.y);

	float offsetMin = 0.01;	// TODO Festlegung treffen
	float offsetMax = lenAB - lenAD; // darf nicht ueber B hinaus
	float offset =  error * (-0.5) + offsetMax; // TODO Festlegung wie aggressiv die ideale Bahn angesteuert werden soll


	if(offset > offsetMax) offset = offsetMax;
	else if(offset < offsetMin) offset = offsetMin;

	coord_t TP = {  coordA.lat + ((lenAD + offset)*eAB.x)/distLon,
					coordA.lon + ((lenAD + offset)*eAB.y)/distLat };
	return TP;
}

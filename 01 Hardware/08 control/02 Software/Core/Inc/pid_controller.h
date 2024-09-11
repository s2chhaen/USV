// name:		pid_controller.h
// description:	Dieses Modul beinhaltet den PID Regelalgorithmus
//				inkl. Parametrisierung

#ifndef INC_PID_CONTROLLER_H_
#define INC_PID_CONTROLLER_H_

#include "stdio.h"

// Reglerinstanz inkl. Parameter
typedef struct {
	float ta; 			// Abtastzeit in s
	float kp; 			// Verstaerkung P-Anteil
	float ki; 			// Verstaerkung I-Anteil
	float kd;			// Verstaerkung D-Anteil
	float esum; 		// Integrations-Summe I-Anteil
	float eold;			// vorherige Regelabweichung
	float esum_limit;	// Begrenzung Begrenzung
	float y_min; 		// untere Stellgroes I-Anteil
	float y_max; 		// obere Stellgroessensen Begrenzung
} controller;

// description:	Berechnet die Stellgroesse nach dem PID Regelalgorithmus
// param: 		PID-Regler-Instanz, w Fuehrungsgroesse, x Regelgroesse
// return: 		y Stellgroesse
float pid_controll(controller*, float, float);

#endif /* INC_PID_CONTROLLER_H_ */


// --- nicht in Benutzung ---
/*
typedef struct {
	Fixed32 Ta;
	Fixed32 y_max;
	Fixed32 y_min;
	Fixed32 Kp;
	Fixed32 Ki;
	Fixed32 Kd;
	Fixed32 e_old;
	Fixed32 esum;
	Fixed32 esum_limit;
} FixedPid;


Fixed32 FixedPid_Calculate(FixedPid* , Fixed32, Fixed32);
*/

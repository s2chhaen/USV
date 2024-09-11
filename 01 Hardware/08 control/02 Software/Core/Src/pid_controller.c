// pid_controller.c

#include "pid_controller.h"
#include "debug.h"

float pid_controll(controller* ini, float w, float x){
	// Regelabweichung = Soll - Ist
	float e = w - x;
	sendStrToUSB("e:", 3);
	sendFloatToUSB(e);

	// --- P-Term ---
	float Pout = (*ini).kp*e;

	// --- I-Term ---
	(*ini).esum = (*ini).esum + e; // Integration
	// Anti-WindUp-Massnahme
	if((*ini).esum < -((*ini).esum_limit)) // untere Begrenzung I-Anteil
		(*ini).esum = -(*ini).esum_limit;
	else if( (*ini).esum > (*ini).esum_limit) // obere Begrenzung I-Anteil
		(*ini).esum = (*ini).esum_limit;
	float Iout = (*ini).ki*(*ini).ta*(*ini).esum;

	// --- D-Term ---
	float Dout = (*ini).kd*((e-(*ini).eold)/(*ini).ta);

	// --- Stellgr. ---
	float y = Pout + Iout + Dout;
	// Stellgr.-Begrenzung
	if(y < (*ini).y_min) y = (*ini).y_min; // untere
	else if(y > (*ini).y_max) y = (*ini).y_max; // obere

	// e sichern
	(*ini).eold = e;

	return y;
}


// --- nicht in Benutzung ---
// ---------------------------------------------------------------------
/*
#include "fixed.h"
Fixed32 FixedPid_Calculate(FixedPid* self, Fixed32 w, Fixed32 x) {
    Fixed32 e = w - x; // abweichung = soll - ist

    // --- P-Term ---
    Fixed32 Pout = Fixed32_Mul(self->Kp, e); // Pout = Kp * e;

    // --- I-Term ---
    self->esum += Fixed32_Mul(e, self->Ta);  // esum += e * Ta;
    // Anti-Wind-Up Massnahme
    if(self->esum > self->esum_limit) self->esum = self->esum_limit;
    else if(self->esum < -(self->esum_limit)) self->esum = -(self->esum_limit);
    Fixed32 Iout = Fixed32_Mul(self->Ki, self->esum); // Iout = Ki * esum

    // --- D-Term ---
    Fixed32 derivative = Fixed32_Div(e - self->e_old, self->Ta); // (e-e_old)/Ta
    Fixed32 Dout = Fixed32_Mul(self->Kd, derivative);

    // --- Stellgr. ---
    Fixed32 y = Pout + Iout + Dout;
    // Stellgr.-Begrenzung
    if(y > self->y_max) y = self->y_max;
    else if(y < self->y_min) y = self->y_min;

    // e sichern
    self->e_old = e;
    return y;
}
*/
// ---------------------------------------------------------------------

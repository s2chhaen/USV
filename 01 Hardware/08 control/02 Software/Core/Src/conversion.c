// conversion.c

#include "conversion.h"

// union zur Vereinfachung des casts
typedef union unint32_t{
	int32_t val;
	uint8_t Bytes[4];
}unint32_t;

float convFixedToF32(uint8_t* bytes, uint8_t byteCount, uint8_t intBits, uint8_t fracBits){
	unint32_t fixed = {0}; // -> Achtung: somit max 4 bytes moeglich!

	// Byte-Array zusammensetzen
	for(uint8_t i = 0; i < byteCount; i++){
		fixed.Bytes[i] = bytes[i];
	}

	// Konvertierung fixed -> f32
	float f32 = (float)fixed.val;
	float divider = (float)(1 << (byteCount*8 - intBits));

	return f32 /= divider;
}

void convF32ToFixed(float value, uint8_t* bytes, uint8_t byteCount, uint8_t intBits, uint8_t fracBits){
	unint32_t fixed = {0};

	if(value >= (1 << intBits)-1){ // >= groesster darstellbarer fixed Wert
		fixed.val = (1 << (intBits + fracBits - 1))-1; // max Wert
	}
	else if(value <= -(1 << (intBits - 1))){ // >= kleinster darstellbarer fixed Wert
		fixed.val = (1 << (intBits + fracBits - 1)); // min Wert
	}
	else{ // Konvertierung f32 -> fixed
		fixed.val = round(value * (1 << fracBits));
	}

//	// Aufteilen in Byte-Array
//	for(uint8_t i = 0; i < byteCount; i++){
//		bytes[byteCount-1 - i] = (uint8_t) (fixed >> 8*i);
//	}

	for(uint8_t i = 0; i < byteCount; i++){
		bytes[i] = fixed.Bytes[i];
	}
}

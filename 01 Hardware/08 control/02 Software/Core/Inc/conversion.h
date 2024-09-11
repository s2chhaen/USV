#ifndef INC_CONVERSION_H_
#define INC_CONVERSION_H_

#include "stdio.h"
#include <math.h>

// Quelle:
// https://embeddedartistry.com/blog/2018/07/12/simple-fixed-point-conversion-in-c/

// Wert, Addr des Fixed Arrays, GroesseArray, IntegerBits, FracBits,
void convF32ToFixed(float, uint8_t*, uint8_t, uint8_t, uint8_t);

// Addr des Fixed Arrays, GroesseArray, IntegerBits, FracBits, Vorzeichen (1 = -, 0 bzw rest = +)
float convFixedToF32(uint8_t* bytes, uint8_t byteCount, uint8_t intBits, uint8_t fracBits);

#endif /* INC_CONVERSION_H_ */

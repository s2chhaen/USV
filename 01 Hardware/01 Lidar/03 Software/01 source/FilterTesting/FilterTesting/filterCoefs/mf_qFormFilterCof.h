/*
 * Filter Coefficients (C Source) generated by the Filter Design and Analysis Tool
 * Generated by MATLAB(R) 9.14 and Signal Processing Toolbox 9.2.
 * Generated on: 22-Sep-2023 15:59:13
 */

/*
 * Discrete-Time FIR Filter (real)
 * -------------------------------
 * Filter Structure    : Direct-Form II
 * Numerator Length    : 3
 * Denominator Length  : 3
 * Stable              : Yes
 * Linear Phase        : Yes (Type 1)
 */

/* General type conversion for MATLAB generated C-code  */
#include "tmwtypes.h"
/* 
 * Expected path to tmwtypes.h 
 * C:\Program Files\MATLAB\R2023a\extern\include\tmwtypes.h 
 */
/*
 * Warning - Filter coefficients were truncated to fit specified data type.  
 *   The resulting response may not match generated theoretical response.
 *   Use the Filter Design & Analysis Tool to design accurate
 *   int16 filter coefficients.
 */
const int mf_numLen = 3;
const int16_T mf_num[3] = {
     8192,  16384,   8192
};
const int mf_denLen = 3;
const int16_T mf_den[3] = {
    32767,      0,      0
};

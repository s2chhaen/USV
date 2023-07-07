////////////////////////////////////////////////////////////////////////////////////////////////////
// MinMax.h
//
// Created: 04.12.2019 10:27:12
// Author: franke
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef MINMAX_H_
#define MINMAX_H_

/** Convenience macro to determine the larger of two values.
*
*  \attention This macro should only be used with operands that do not have side effects from being evaluated
*             multiple times.
*
*  \param[in] a  First value to compare
*  \param[in] b  Second value to compare
*
*  \return The larger of the two input parameters
*/
#if !defined(MAX) || defined(__DOXYGEN__)
#define MAX(a, b) ({ __typeof__ (a) _a = (a); \
					__typeof__ (b) _b = (b); \
					_a > _b ? _a : _b; })
#endif

/** Convenience macro to determine the smaller of two values.
*
*  \attention This macro should only be used with operands that do not have side effects from being evaluated
*             multiple times.
*
*  \param[in] a  First value to compare.
*  \param[in] b  Second value to compare.
*
*  \return The smaller of the two input parameters
*/
#if !defined(MIN) || defined(__DOXYGEN__)
#define MIN(a, b) ({ __typeof__ (a) _a = (a); \
					__typeof__ (b) _b = (b); \
					_a < _b ? _a : _b; })
#endif

/** Convenience macro to determine a value in the range of two values.
*
*  \attention This macro should only be used with operands that do not have side effects from being evaluated
*             multiple times.
*
*  \param[in] v  Value to limit.
*  \param[in] bu  Maximum value to compare.
*  \param[in] bl  Minimum value to compare.
*
*  \return The smaller of the two input parameters
*/
#if !defined(BOUND) || defined(__DOXYGEN__)
#define BOUND(x,bl,bu) ({ __typeof__ (x) _x = (x); \
						__typeof__ (bl) _bl = (bl); \
						__typeof__ (bu) _bu = (bu); \
						MIN(MAX(_x,_bu),_bl); })
#endif
#endif /* MINMAX_H_ */

//EOF//

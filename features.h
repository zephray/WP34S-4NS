/* This file is part of 34S.
 * 
 * 34S is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * 34S is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with 34S.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FEATURES_H__
#define FEATURES_H__

#if !defined(REALBUILD) && !defined(WINGUI) && !defined(QTGUI) && !defined(IOS)
#define CONSOLE
#endif

/*
 *  Select optional features here
 */

// Allow for any generic argument taking commands in XROM
// #define XROM_RARG_COMMANDS

// Define this to support a STOPWATCH function like the StopWatch on the HP-41C
// Time Module or the HP-55
#if !defined(REALBUILD) || (defined(XTAL) /* && !defined(INFRARED) */)
#define INCLUDE_STOPWATCH
#else
//#define INCLUDE_STOPWATCH
#endif

// Include the pixel plotting commands
#define INCLUDE_PLOTTING

// Build a tiny version of the device
// #define TINY_BUILD

// Include a catalogue of the internal commands
// If not defined, these commands are put into P.FCN, TEST and CPX X.FCN instead.
// #define INCLUDE_INTERNAL_CATALOGUE
// #define INCLUDE_RELATIVE_CALLS

// Include a mechanism for a user defined catalogue
// 2-3 flash pages (512 - 768 bytes) in total.
// #define INCLUDE_USER_CATALOGUE

// Include the CNSTS command to access constants via indirection
#define INCLUDE_INDIRECT_CONSTS

// Replace dispatch functions for calling niladic, monadic, dyadic and triadic
// functions and their complex variants with one universal dispatch function.
// It saves approximately 280 bytes in the firmware.
// This is an EXPERIMENTAL FEATURE that hasn't yet received adequate testing.
//#define UNIVERSAL_DISPATCH

// Code to allow access to caller's local data from xIN-code
// #define ENABLE_COPYLOCALS

#ifndef TINY_BUILD

// Include the Mantissa and exponent function
// Space cost is approximately 180 bytes
#define INCLUDE_MANTISSA

// Include the xroot function for reals, integers and complex numbers
// Space cost is approximately 400 bytes
#define INCLUDE_XROOT

// Include the user mode serial commands SOPEN, SCLOSE, RECV1, SEND1, aRECV, aSEND
// Space cost approximately 700 bytes.
// #define INCLUDE_USER_IO

// Include the SAVEM/RESTM user mode save and restore commands
#define INCLUDE_USER_MODE

// Include the Gudermannian functions and their inverses in the real
// and complex domain.
#define INCLUDE_GUDERMANNIAN

// Include first and second order Bessel functions Jn, In, Yn and Kn for
// both real and complex arguments.  These are implemented in XROM.
// #define INCLUDE_XROM_BESSEL

// Inlcude real and complex flavours of the digamma function.  These are
// implemented in XROM.  The first setting is sufficient for accuracy for
// single precision, the second needs to be enabled as well to get good
// results for double precision..
// #define INCLUDE_XROM_DIGAMMA
// #define XROM_DIGAMMA_DOUBLE_PRECISION

// Include a fused multiply add instruction
// This isn't vital since this can be done using a complex addition.
// Space cost 108 bytes.
// #define INCLUDE_MULADD

// Include a date function to determine the date of Easter in a given year
//#define INCLUDE_EASTER

// Include code to use a Ridder's method step after a bisection in the solver.
// For some functions this seems to help a lot, for others there is limited
// benefit.
#define USE_RIDDERS

// Include code to find integer factors
// Space cost 480 bytes.
// #define INCLUDE_FACTOR

// Include matrix functions better implemented in user code
// #define SILLY_MATRIX_SUPPORT

// Include matrix row/row operations.
// M.R<->, M.R*, M.R+
#define MATRIX_ROWOPS

// Include the LU decomposition as a user command
// M.LU
#define MATRIX_LU_DECOMP

// Include fast path code to calculate factorials and gamma functions
// for positive integers using a string of multiplications.
#define GAMMA_FAST_INTEGERS

// Include the flash register recall routines RCF and their variants
// #define INCLUDE_FLASH_RECALL

// Include iBACK, etc. as user visible commands
// #define INCLUDE_INDIRECT_BRANCHES

// Include the upper tail cumulative distribution functions
#define INCLUDE_CDFU

// Include code to support the 41/42's MOD operation
#define INCLUDE_MOD41

// Include code to support integer mode multiplication and exponentation modulo
// operations.
#define INCLUDE_INT_MODULO_OPS

// Include code to support integer (truncated) division
#define INCLUDE_INTEGER_DIVIDE

// Make EEX key enter PI if pressed when command line empty
//#define INCLUDE_EEX_PI

// Change the fraction separator to the old Casio form _|
//#define INCLUDE_CASIO_SEPARATOR

// Switch the seven-segment display to fraction mode as soon as two decimal marks are entered
#define PRETTY_FRACTION_ENTRY

// Make two successive decimals a..b enter an improper fraction a/b, not a 0/b (also enables PRETTY_FRACTION_ENTRY)
//#define INCLUDE_DOUBLEDOT_FRACTIONS

// Chamge ALL display mode to limited significant figures mode
//#define INCLUDE_SIGFIG_MODE

// Turn on constant y-register display (not just for complex results)
#define INCLUDE_YREG_CODE

// Temporarily disable y-register display when a shift key or the CPX key is pressed 
//#define SHIFT_AND_CMPLX_SUPPRESS_YREG

// Use fractions in y-register display
#define INCLUDE_YREG_FRACT

// Don't show angles as fractions after a rectangular to polar coordinate conversion (also enables RP_PREFIX)
#define ANGLES_NOT_SHOWN_AS_FRACTIONS

// Use HMS mode in y-register display
#define INCLUDE_YREG_HMS

// Show prefix for gradian mode when y-register is displayed (without this gradian mode is indicated by neither the RAD nor the 360 annunciators being shown)
#define SHOW_GRADIAN_PREFIX

// Right-justify seven-segment exponent (007 rather than 7  )
//#define INCLUDE_RIGHT_EXP

// Rectangular - Polar y-reg prefix change:
#define RP_PREFIX

// h ./, in DECM mode switches E3 separator on/off (instead of chnaging radix symbol)
//#define MODIFY_K62_E3_SWITCH

// Indicate four-level stack by a '.' and eight-level stack by a ':'
//#define SHOW_STACK_SIZE

// BEG annunciators indicates BIG stack size rather than beginning of program
//#define MODIFY_BEG_SSIZE8

/*
 * This setting allows to change default mode to one of the other 2
 * possibilities. The date mode equal to DEFAULT_DATEMODE will not be
 * announced, the other 2 will be.
 * If left undefined, it defaults to DMY mode.
 * See enum date_modes for values of
 *	DATE_DMY=0,	DATE_YMD=1,	DATE_MDY=2
*/
//#define DEFAULT_DATEMODE 0

/* This setting supresses the date mode display entirely if enabled.
 */
//#define NO_DATEMODE_INDICATION


/*******************************************************************/
/* Below here are the automatic defines depending on other defines */
/*******************************************************************/

#if defined(INCLUDE_DOUBLEDOT_FRACTIONS)
#define PRETTY_FRACTION_ENTRY
#endif

#if defined(INCLUDE_COMPLEX_ZETA) && ! defined(INCLUDE_ZETA)
/* Complex zeta implies real zeta */
#define INCLUDE_ZETA
#endif

#if defined(INCLUDE_BERNOULLI) && ! defined(INCLUDE_ZETA)
/* Bernoulli numbers need real zeta */
#define INCLUDE_ZETA
#endif

#if defined(XROM_DIGAMMA_DOUBLE_PRECISION) && ! defined(INCLUDE_XROM_DIGAMMA)
/* Accurate digamma needs normal digamma */
#define INCLUDE_XROM_DIGAMMA
#endif

#if defined(INCLUDE_PLOTTING) || defined(INFRARED)
#define PAPER_WIDTH 166
#endif

#if defined(INCLUDE_YREG_CODE) && defined(INCLUDE_YREG_FRACT) && defined(ANGLES_NOT_SHOWN_AS_FRACTIONS)
#define RP_PREFIX
#endif

#if defined(INCLUDE_YREG_CODE) || defined(RP_PREFIX) || defined(SHOW_STACK_SIZE)
#define INCLUDE_FONT_ESCAPE
#endif

#endif  /* TINY_BUILD*/
#endif  /* FEATURES_H__ */

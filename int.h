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


#ifndef __INT_H__
#define __INT_H__

#include "xeq.h"

#define MAX_WORD_SIZE	64

extern unsigned long long int extract_value(const long long int val, int *const sign);
extern long long int build_value(const unsigned long long int x, const int sign);
extern long long int mask_value(const long long int);
extern unsigned int word_size(void);
extern unsigned int int_base(void);
extern enum arithmetic_modes int_mode(void);

extern void set_carry(int);
extern void set_overflow(int);
extern int get_carry(void);
extern int get_overflow(void);

extern int check_overflow(long long int);

extern long long int intAdd(long long int y, long long int x);
extern long long int intSubtract(long long int y, long long int x);
extern long long int intMultiply(long long int y, long long int x);
extern long long int intDivide(long long int y, long long int x);
extern long long int intPower(long long int y, long long int x);
extern long long int intMod(long long int y, long long int x);
extern long long int intMin(long long int y, long long int x);
extern long long int intMax(long long int y, long long int x);
extern long long int intMAdd(long long int z, long long int y, long long int x);

extern long long int intmodop(long long int z, long long int y, long long int x);

extern long long int intGCD(long long int y, long long int x);
extern long long int intLCM(long long int y, long long int x);

extern long long int intSqr(long long int x);
extern long long int intCube(long long int x);

extern long long int intChs(long long int x);
extern long long int intAbs(long long int x);
extern long long int intNot(long long int x);
#if 1
extern long long int intBooleanOp(long long int y, long long int x);
#else
extern long long int intAnd(long long int y, long long int x);
extern long long int intOr(long long int y, long long int x);
extern long long int intXor(long long int y, long long int x);
extern long long int intNand(long long int y, long long int x);
extern long long int intNor(long long int y, long long int x);
extern long long int intEquiv(long long int y, long long int x);
#endif

extern long long int intFP(long long int x);
#define intIP	mask_value
extern long long int intSign(long long int x);

extern void introt(unsigned arg, enum rarg op);

extern long long int intNumBits(long long int x);
extern long long int intSqrt(long long int x);
extern long long int intLog2(long long int x);
extern long long int int2pow(long long int x);
extern long long int intLog10(long long int x);
extern long long int int10pow(long long int x);
extern long long int intFib(long long int x);
extern long long int int_1pow(long long int x);

extern int isPrime(unsigned long long int x);

extern long long int intMirror(long long int x);
extern void intmsks(unsigned arg, enum rarg op);

extern void int_justify(enum nilop);

extern void intbits(unsigned arg, enum rarg op);

extern void intDblMul(enum nilop);
extern long long int intDblDiv(long long int, long long int, long long int);
extern long long int intDblRmdr(long long int, long long int, long long int);

#ifdef INCLUDE_FACTOR
extern unsigned long long int doFactor(unsigned long long int);
extern long long int intFactor(long long int);
#endif

extern long long int intRecv(long long int x);


#endif

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

#ifndef __decn_h__
#define __decn_h__

#include "xeq.h"

// true if the number is a NaN or infinite
#define decNumberIsSpecial(x)	((x)->bits & DECSPECIAL)

extern int dn_lt0(const decNumber *x);
extern int dn_le0(const decNumber *x);
#define dn_gt0(x) (! dn_le0(x))
#define dn_ge0(x) (! dn_lt0(x))
extern int dn_eq0(const decNumber *x);
extern int dn_eq1(const decNumber *x);
extern int dn_eq(const decNumber *x, const decNumber *y);
extern int dn_lt(const decNumber *x, const decNumber *y);
extern int dn_abs_lt(const decNumber *x, const decNumber *tol);
#define dn_gt(x, y) dn_lt(y, x)
#define dn_le(x, y) (! dn_lt(y, x))
#define dn_ge(x, y) (! dn_lt(x, y))

extern decNumber *dn_add(decNumber *r, const decNumber *a, const decNumber *b);
extern decNumber *dn_subtract(decNumber *r, const decNumber *a, const decNumber *b);
extern decNumber *dn_multiply(decNumber *r, const decNumber *a, const decNumber *b);
extern decNumber *dn_divide(decNumber *r, const decNumber *a, const decNumber *b);
extern decNumber *dn_compare(decNumber *r, const decNumber *a, const decNumber *b);
extern decNumber *dn_min(decNumber *r, const decNumber *a, const decNumber *b);
extern decNumber *dn_max(decNumber *r, const decNumber *a, const decNumber *b);
extern decNumber *dn_abs(decNumber *r, const decNumber *a);
extern decNumber *dn_minus(decNumber *r, const decNumber *a);
extern decNumber *dn_plus(decNumber *r, const decNumber *a);
extern decNumber *dn_sqrt(decNumber *r, const decNumber *a);
extern decNumber *dn_exp(decNumber *r, const decNumber *a);
extern decNumber *dn_power(decNumber *r, const decNumber *a, const decNumber *b);

extern decNumber *dn_average(decNumber *r, const decNumber *a, const decNumber *b);


extern void int_to_dn(decNumber *, int);
extern int dn_to_int(const decNumber *);
extern void ullint_to_dn(decNumber *, unsigned long long int);
extern unsigned long long int dn_to_ull(const decNumber *, int *);

extern void decNumberPI(decNumber *pi);
extern void decNumberPIon2(decNumber *pion2);
extern int is_int(const decNumber *);

extern decNumber *decNumberMantissa(decNumber *r, const decNumber *a);
extern decNumber *decNumberExponent(decNumber *r, const decNumber *a);
extern decNumber *decNumberULP(decNumber *r, const decNumber *a);
extern decNumber *decNumberNeighbour(decNumber *, const decNumber *, const decNumber *);

extern decNumber *decNumberXRoot(decNumber *r, const decNumber *a, const decNumber *b);
extern long long int intXRoot(long long int y, long long int x);

extern int relative_error(const decNumber *x, const decNumber *y, const decNumber *tol);
extern int absolute_error(const decNumber *x, const decNumber *y, const decNumber *tol);
extern const decNumber *convergence_threshold(void);

extern decNumber *decNumberMAdd(decNumber *r, const decNumber *z, const decNumber *y, const decNumber *x);

extern decNumber *decNumberMod(decNumber *res, const decNumber *x, const decNumber *y);
extern decNumber *decNumberBigMod(decNumber *res, const decNumber *x, const decNumber *y);

extern decNumber *decNumberRnd(decNumber *r, const decNumber *x);
extern decNumber *decNumberRecip(decNumber *r, const decNumber *x);

extern decNumber *decNumberFloor(decNumber *r, const decNumber *x);
extern decNumber *decNumberCeil(decNumber *r, const decNumber *x);
extern decNumber *decNumberTrunc(decNumber *r, const decNumber *x);
extern decNumber *decNumberRoundDigits(decNumber *r, const decNumber *x, const int digits, const enum rounding round);
extern decNumber *decNumberRoundDecimals(decNumber *r, const decNumber *x, const int n, const enum rounding round);
extern decNumber *decNumberRound(decNumber *r, const decNumber *x);
extern decNumber *decNumberFrac(decNumber *r, const decNumber *x);

extern decNumber *decNumberGCD(decNumber *r, const decNumber *x, const decNumber *y);
extern decNumber *decNumberLCM(decNumber *r, const decNumber *x, const decNumber *y);

extern decNumber *decNumberPow_1(decNumber *r, const decNumber *x);
extern decNumber *decNumberPow2(decNumber *r, const decNumber *x);
extern decNumber *decNumberPow10(decNumber *r, const decNumber *x);
extern decNumber *decNumberLn1p(decNumber *r, const decNumber *x);
extern decNumber *decNumberExpm1(decNumber *r, const decNumber *x);
extern decNumber *dn_log2(decNumber *r, const decNumber *x);
extern decNumber *dn_log10(decNumber *r, const decNumber *x);
extern decNumber *decNumberLogxy(decNumber *r, const decNumber *x, const decNumber *y);

extern decNumber *decNumberSquare(decNumber *r, const decNumber *x);
extern decNumber *decNumberCube(decNumber *r, const decNumber *x);
extern decNumber *decNumberCubeRoot(decNumber *r, const decNumber *x);

extern decNumber *decNumberDRG(decNumber *res, const decNumber *x);

extern decNumber *decNumberSin(decNumber *res, const decNumber *x);
extern decNumber *decNumberCos(decNumber *res, const decNumber *x);
extern decNumber *decNumberTan(decNumber *res, const decNumber *x);
#if 0
extern decNumber *decNumberSec(decNumber *res, const decNumber *x);
extern decNumber *decNumberCosec(decNumber *res, const decNumber *x);
extern decNumber *decNumberCot(decNumber *res, const decNumber *x);
#endif
extern decNumber *decNumberArcSin(decNumber *res, const decNumber *x);
extern decNumber *decNumberArcCos(decNumber *res, const decNumber *x);
extern decNumber *decNumberArcTan(decNumber *res, const decNumber *x);
extern decNumber *decNumberArcTan2(decNumber *res, const decNumber *x, const decNumber *y);
extern decNumber *decNumberSinc(decNumber *res, const decNumber *x);

extern decNumber *do_atan2(decNumber *at, const decNumber *ain, const decNumber *b);


extern void op_r2p(enum nilop op);
extern void op_p2r(enum nilop op);

extern decNumber *decNumberSinh(decNumber *res, const decNumber *x);
extern decNumber *decNumberCosh(decNumber *res, const decNumber *x);
extern decNumber *decNumberTanh(decNumber *res, const decNumber *x);
#if 0
extern decNumber *decNumberSech(decNumber *res, const decNumber *x);
extern decNumber *decNumberCosech(decNumber *res, const decNumber *x);
extern decNumber *decNumberCoth(decNumber *res, const decNumber *x);
#endif
extern decNumber *decNumberArcSinh(decNumber *res, const decNumber *x);
extern decNumber *decNumberArcCosh(decNumber *res, const decNumber *x);
extern decNumber *decNumberArcTanh(decNumber *res, const decNumber *x);

extern decNumber *decNumberFactorial(decNumber *r, const decNumber *xin);
extern decNumber *decNumberGamma(decNumber *res, const decNumber *x);
extern decNumber *decNumberLnGamma(decNumber *res, const decNumber *x);
extern decNumber *decNumberLnBeta(decNumber *res, const decNumber *x, const decNumber *y);

extern decNumber *decNumberERF(decNumber *res, const decNumber *x);
extern decNumber *decNumberERFC(decNumber *res, const decNumber *x);
extern decNumber *decNumberGammap(decNumber *res, const decNumber *a, const decNumber *x);

extern decNumber *decNumberD2G(decNumber *res, const decNumber *x);
extern decNumber *decNumberD2R(decNumber *res, const decNumber *x);
extern decNumber *decNumberG2D(decNumber *res, const decNumber *x);
extern decNumber *decNumberG2R(decNumber *res, const decNumber *x);
extern decNumber *decNumberR2D(decNumber *res, const decNumber *x);
extern decNumber *decNumberR2G(decNumber *res, const decNumber *x);

extern void decNumber2Fraction(decNumber *n, decNumber *d, const decNumber *x);

extern decNumber *decNumberComb(decNumber *res, const decNumber *x, const decNumber *y);
extern decNumber *decNumberPerm(decNumber *res, const decNumber *x, const decNumber *y);

extern decNumber *decNumberHMS2HR(decNumber *res, const decNumber *x);
extern decNumber *decNumberHR2HMS(decNumber *res, const decNumber *x);
extern decNumber *decNumberHMSAdd(decNumber *res, const decNumber *x, const decNumber *y);
extern decNumber *decNumberHMSSub(decNumber *res, const decNumber *x, const decNumber *y);

extern decNumber *decNumberNot(decNumber *res, const decNumber *x);
extern decNumber *decNumberBooleanOp(decNumber *res, const decNumber *x, const decNumber *y);

extern void dn_sincos(const decNumber *v, decNumber *sinv, decNumber *cosv);
extern void sincosTaylor(const decNumber *a, decNumber *s, decNumber *c);
extern void dn_sinhcosh(const decNumber *v, decNumber *sinhv, decNumber *coshv);
extern void do_asin(decNumber *, const decNumber *);
extern void do_acos(decNumber *, const decNumber *);
extern void do_atan(decNumber *, const decNumber *);

extern void dn_elliptic(decNumber *sn, decNumber *cn, decNumber *dn, const decNumber *u, const decNumber *k);

extern decNumber *set_NaN(decNumber *);
extern decNumber *set_inf(decNumber *);
extern decNumber *set_neginf(decNumber *);

extern decNumber *dn_inc(decNumber *);
extern decNumber *dn_dec(decNumber *);
extern decNumber *dn_p1(decNumber *, const decNumber *);
extern decNumber *dn_m1(decNumber *, const decNumber *);
extern decNumber *dn_1m(decNumber *, const decNumber *);
extern decNumber *dn_1(decNumber *);
extern decNumber *dn__1(decNumber *);
extern decNumber *dn_p2(decNumber *, const decNumber *);
extern decNumber *dn_mul2(decNumber *, const decNumber *);
extern decNumber *dn_div2(decNumber *, const decNumber *);
extern decNumber *dn_mul100(decNumber *, const decNumber *);
extern decNumber *dn_mul1000(decNumber *, const decNumber *);
extern decNumber *dn_mulPI(decNumber *, const decNumber *);
extern decNumber *dn_mulpow10(decNumber *, const decNumber *, int);

extern decNumber *decFactor(decNumber *r, const decNumber *x);

extern decNumber *dn_ln(decNumber *r, const decNumber *x);


extern decNumber *decRecv(decNumber *r, const decNumber *x);

#endif

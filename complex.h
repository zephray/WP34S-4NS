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

#ifndef COMPLEX_H
#define COMPLEX_H

/* Dyadic operation */
extern void cmplxAdd(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *c, const decNumber *d);
extern void cmplxSubtract(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *c, const decNumber *d);
extern void cmplxMultiply(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *c, const decNumber *d);
extern void cmplxDivide(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *c, const decNumber *d);
extern void cmplxPower(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *c, const decNumber *d);
extern void cmplxLogxy(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *c, const decNumber *d);
extern void cmplxXRoot(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *c, const decNumber *d);

/* And some shortcuts to the above for one argument real */
#if 0
extern void cmplxDivideReal(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *r);
extern void cmplxDivideRealBy(decNumber *rx, decNumber *ry,
		const decNumber *a,
		const decNumber *c, const decNumber *d);
extern void cmplxMultiplyReal(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *r);
extern void cmplxPowerReal(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *r);
extern void cmplxPowerReal(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *r);
#endif

/* Conversion to and from polar representation */
extern void cmplxArg(decNumber *arg, const decNumber *a, const decNumber *b);
extern void cmplxR(decNumber *r, const decNumber *a, const decNumber *b);
//extern void cmplxFromPolar(decNumber *x, decNumber *y, const decNumber *r, const decNumber *t);
extern void cmplxToPolar(decNumber *r, decNumber *t, const decNumber *x, const decNumber *y);


/* Monadic operations */
extern void cmplxAbs(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);
extern void cmplxMinus(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);

extern void cmplxRecip(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);

extern void cmplxSqrt(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);
extern void cmplxCubeRoot(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);

extern void cmplxLn(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);
extern void cmplxExp(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);
extern void cmplx_1x(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);

extern void cmplxSin(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);
extern void cmplxCos(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);
extern void cmplxTan(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);
extern void cmplxSinc(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);

extern void cmplxSinh(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);
extern void cmplxCosh(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);
extern void cmplxTanh(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);

extern void cmplxGamma(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);
extern void cmplxLnGamma(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b);

extern void cmplx_NaN(decNumber *, decNumber *);

#endif

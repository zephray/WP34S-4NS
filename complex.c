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

#include "decn.h"
#include "complex.h"
#include "xeq.h"
#include "consts.h"

#if 0
#include <stdio.h>
static FILE *debugf = NULL;

static void open_debug(void) {
	if (debugf == NULL) {
		debugf = fopen("/dev/ttys000", "w");
	}
}
static void dump1(const decNumber *a, const char *msg) {
	char b[100];

	open_debug();
	decNumberToString(a, b);
	fprintf(debugf, "%s: %s\n", msg, b);
}

static void dump2(const decNumber *a, const decNumber *b, const char *msg) {
	char b1[100], b2[100];

	open_debug();
	decNumberToString(a, b1);
	decNumberToString(b, b2);
	fprintf(debugf, "%s: %s / %s\n", msg, b1, b2);
}
#endif

#define COMPLEX_DIGITS	72

typedef struct {
	decNumber n;
	decNumberUnit extra[((COMPLEX_DIGITS-DECNUMDIGITS+DECDPUN-1)/DECDPUN)];
} complexNumber;

static int setComplexContext(void) {
	int digits = Ctx.digits;
	if (is_dblmode())
		Ctx.digits = COMPLEX_DIGITS;
	return digits;
}

static void resetComplexContext(int x) {
	Ctx.digits = x;
}

static void complexResult(decNumber *x, decNumber *y, complexNumber *xin, complexNumber *yin, int saved) {
	resetComplexContext(saved);
	dn_plus(x, &xin->n);
	dn_plus(y, &yin->n);
}



void cmplx_NaN(decNumber *x, decNumber *y) {
	set_NaN(x);
	set_NaN(y);
}

#ifndef TINY_BUILD
// r - (a + i b) = (r - a) - i b
#if 0
static void cmplxSubtractFromReal(decNumber *rx, decNumber *ry,
		const decNumber *r,
		const decNumber *a, const decNumber *b) {
	dn_subtract(rx, r, a);
	dn_minus(ry, b);
}
#endif

// (a + i b) * r = (a * r) + i (b * r)
static void cmplxMultiplyReal(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *r) {
	dn_multiply(rx, a, r);
	dn_multiply(ry, b, r);
}

#if 0
static void cmplxDiv2(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b) {
	cmplxMultiplyReal(rx, ry, a, b, &const_0_5);
}
#endif

#if 0
// (a + i b) / c = ( a / r ) + i ( b / r)
static void cmplxDivideReal(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *r) {
	dn_divide(rx, a, r);
	dn_divide(ry, b, r);
}
#endif

// a / (c + i d) = (a*c) / (c*c + d*d) + i (- a*d) / (c*c + d*d)
static void cmplxDivideRealBy(decNumber *rx, decNumber *ry,
		const decNumber *a,
		const decNumber *c, const decNumber *d) {
	decNumber t1, t2, t3, den;

	dn_multiply(&t1, c, c);
	dn_multiply(&t2, d, d);
	dn_add(&den, &t1, &t2);

	dn_multiply(&t1, a, c);
	dn_divide(rx, &t1, &den);

	dn_multiply(&t2, a, d);
	dn_minus(&t3, &t2);
	dn_divide(ry, &t3, &den);
}
#endif

// (a + i b) + (c + i d) = (a + c) + i (b + d)
void cmplxAdd(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *c, const decNumber *d) {
	dn_add(rx, a, c);
	dn_add(ry, b, d);
}

// (a + i b) - (c + i d) = (a - c) + i (b - d)
void cmplxSubtract(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *c, const decNumber *d) {
	dn_subtract(rx, a, c);
	dn_subtract(ry, b, d);
}

// (a + i b) * (c + i d) = (a * c - b * d) + i (a * d + b * c)
void cmplxMultiply(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *c, const decNumber *d) {
	complexNumber t1, t2, u1, u2, x, y;

	const int save = setComplexContext();
	dn_multiply(&t1.n, a, c);
	dn_multiply(&t2.n, b, d);
	dn_subtract(&x.n, &t1.n, &t2.n);

	dn_multiply(&u1.n, a, d);
	dn_multiply(&u2.n, b, c);
	dn_add(&y.n, &u1.n, &u2.n);

	complexResult(rx, ry, &x, &y, save);
}

// (a + i b) / (c + i d) = (a*c + b*d) / (c*c + d*d) + i (b*c - a*d) / (c*c + d*d)
void cmplxDivide(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *c, const decNumber *d) {
	complexNumber t1, t2, t3, t4, den;

	const int save = setComplexContext();
	dn_multiply(&t1.n, c, c);
	dn_multiply(&t2.n, d, d);
	dn_add(&den.n, &t1.n, &t2.n);

	dn_multiply(&t3.n, a, c);
	dn_multiply(&t2.n, b, d);
	dn_add(&t1.n, &t3.n, &t2.n);

	dn_multiply(&t4.n, b, c);
	dn_multiply(&t2.n, a, d);
	dn_subtract(&t3.n, &t4.n, &t2.n);

	dn_divide(&t2.n, &t1.n, &den.n);
	dn_divide(&t4.n, &t3.n, &den.n);

	complexResult(rx, ry, &t2, &t4, save);
}

void cmplxArg(decNumber *arg, const decNumber *a, const decNumber *b) {
	do_atan2(arg, b, a);
}

void cmplxR(decNumber *r, const decNumber *a, const decNumber *b) {
	complexNumber a2, b2, s;

	dn_multiply(&a2.n, a, a);
	dn_multiply(&b2.n, b, b);
	dn_add(&s.n, &a2.n, &b2.n);
	dn_sqrt(r, &s.n);
}

//void cmplxFromPolar(decNumber *x, decNumber *y, const decNumber *r, const decNumber *t) {
//	decNumber s, c

//	dn_sincos(t, &s, &c);
//	dn_multiply(x, r, &c);
//	dn_multiply(y, r, &s);
//}

void cmplxToPolar(decNumber *r, decNumber *t, const decNumber *x, const decNumber *y) {
	do_atan2(t, y, x);
	cmplxR(r, y, x);
}



#ifndef TINY_BUILD
// ( a + i * b ) ^ r
static void cmplxPowerReal(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *r) {
	cmplxPower(rx, ry, a, b, r, &const_0);
}
#endif

// a ^ b = e ^ (b ln(a))
void cmplxPower(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *c, const decNumber *d) {
#ifndef TINY_BUILD
	decNumber e1, e2, f1, f2;

	cmplxLn(&e1, &e2, a, b);
	cmplxMultiply(&f1, &f2, &e1, &e2, c, d);
	cmplxExp(rx, ry, &f1, &f2);
#endif
}

#ifdef INCLUDE_XROOT
// (a, b) ^ 1 / (c, d)
void cmplxXRoot(decNumber *rx, decNumber *ry,
		const decNumber *a, const decNumber *b,
		const decNumber *c, const decNumber *d) {
	decNumber g, h;

	cmplxRecip(&g, &h, c, d);
	cmplxPower(rx, ry, a, b, &g, &h);
}
#endif


// abs(a + i b) = sqrt(a^2 + b^2)
void cmplxAbs(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b) {
	cmplxR(rx, a, b);
	decNumberZero(ry);
}

// - (a + i b) = - a - i b
void cmplxMinus(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b) {
	dn_minus(rx, a);
	dn_minus(ry, b);
}

// 1 / (c + i d) = c / (c*c + d*d) + i (- d) / (c*c + d*d)
void cmplxRecip(decNumber *rx, decNumber *ry, const decNumber *c, const decNumber *d) {
#ifndef TINY_BUILD
	complexNumber t, u, v, den;

	const int save = setComplexContext();

	dn_multiply(&u.n, c, c);
	dn_multiply(&v.n, d, d);
	dn_add(&den.n, &u.n, &v.n);
	dn_minus(&t.n, d);

	dn_divide(&u.n, c, &den.n);
	dn_divide(&v.n, &t.n, &den.n);

	complexResult(rx, ry, &u, &v, save);
#endif
}

// sqrt(a + i b) = +- (sqrt(r + a) + i sqrt(r - a) sign(b)) sqrt(2) / 2
//		where r = sqrt(a^2 + b^2)
void cmplxSqrt(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b) {
#ifndef TINY_BUILD
	complexNumber fac, t1, u, v, x, y;

	if (dn_eq0(b)) {
		// Detect a purely real input and shortcut the computation
		if (decNumberIsNegative(a)) {
			dn_minus(&t1.n, a);
			dn_sqrt(ry, &t1.n);
			decNumberZero(rx);
		} else {
			dn_sqrt(rx, a);
			decNumberZero(ry);
		}
		return;
	} else {
		const int save = setComplexContext();
		cmplxR(&fac.n, a, b);

		dn_subtract(&v.n, &fac.n, a);
		dn_sqrt(&u.n, &v.n);
		dn_add(&v.n, &fac.n, a);
		if (decNumberIsNegative(b)) {
			dn_minus(&t1.n, &u.n);
			dn_multiply(&y.n, &t1.n, &const_root2on2);
		} else
			dn_multiply(&y.n, &u.n, &const_root2on2);

		dn_sqrt(&t1.n, &v.n);
		dn_multiply(&x.n, &t1.n, &const_root2on2);

		complexResult(rx, ry, &x, &y, save);
	}
#endif
}

// Fairly naive implementation...
void cmplxCubeRoot(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b) {
#ifndef TINY_BUILD
	decNumber t;

	decNumberRecip(&t, &const_3);
	cmplxPowerReal(rx, ry, a, b, &t);
#endif
}


// sin(a + i b) = sin(a) cosh(b) + i cos(a) sinh(b)
// cos(a + i b) = cos(a) cosh(b) - i sin(a) sinh(b)
static void cmplx_sincos(const decNumber *a, const decNumber *b, decNumber *sx, decNumber *sy, decNumber *cx, decNumber *cy) {
	decNumber sa, ca, sb, cb;

	if (dn_eq0(a) && decNumberIsInfinite(b)) {
		decNumberZero(sx);
		decNumberCopy(sy, b);
		set_inf(cx);
		decNumberZero(cy);
	} else {
		dn_sincos(a, &sa, &ca);
		dn_sinhcosh(b, &sb, &cb);

		dn_multiply(sx, &sa, &cb);
		dn_multiply(sy, &ca, &sb);
		dn_multiply(cx, &ca, &cb);
		dn_multiply(&ca, &sa, &sb);
		dn_minus(cy, &ca);
	}
}

// sin(a + i b) = sin(a) cosh(b) + i cos(a) sinh(b)
void cmplxSin(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b) {
	decNumber z;
	cmplx_sincos(a, b, rx, ry, &z, &z);
}

// cos(a + i b) = cos(a) cosh(b) - i sin(a) sinh(b)
void cmplxCos(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b) {
	decNumber z;
	cmplx_sincos(a, b, &z, &z, rx, ry);
}

// tan(a + i b) = (sin(a) cosh(b) + i cos(a) sinh(b)) / (cos(a) cosh(b) - i sin(a) sinh(b))
void cmplxTan(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b) {
#ifndef TINY_BUILD
	decNumber c1, c2, s1, s2;

	if (dn_eq0(a) && decNumberIsInfinite(b)) {
		decNumberZero(rx);
		if (decNumberIsNegative(b))
			dn__1(ry);
		else
			dn_1(ry);
	} else {
		cmplx_sincos(a, b, &s1, &s2, &c1, &c2);
		cmplxDivide(rx, ry, &s1, &s2, &c1, &c2);
	}
#endif
}


// sinc(a + i b) = sin(a + i b) / (a + i b)
void cmplxSinc(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b) {
#ifndef TINY_BUILD
	decNumber s1, s2;

	if (dn_eq0(b)) {
		decNumberSinc(rx, a);
		decNumberZero(ry);
	} else {
		cmplxSin(&s1, &s2, a, b);
		cmplxDivide(rx, ry, &s1, &s2, a, b);
	}
#endif
}

// sinh(a + i b) = sinh(a) cos(b) + i cosh(a) sin(b)
// cosh(a + i b) = cosh(a) cos(b) + i sinh(a) sin(b)
static void cmplx_sinhcosh(const decNumber *a, const decNumber *b, decNumber *sx, decNumber *sy, decNumber *cx, decNumber *cy) {
	decNumber sa, ca, sb, cb;

	dn_sinhcosh(a, &sa, &ca);
	dn_sincos(b, &sb, &cb);

	dn_multiply(sx, &sa, &cb);
	dn_multiply(sy, &ca, &sb);
	dn_multiply(cx, &ca, &cb);
	dn_multiply(cy, &sa, &sb);
}

// sinh(a + i b) = sinh(a) cos(b) + i cosh(a) sin(b)
void cmplxSinh(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b) {
	decNumber z;
	cmplx_sinhcosh(a, b, rx, ry, &z, &z);
}

// cosh(a + i b) = cosh(a) cos(b) + i sinh(a) sin(b)
void cmplxCosh(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b) {
	decNumber z;
	cmplx_sinhcosh(a, b, &z, &z, rx, ry);
}

// tanh(a + i b) = (tanh(a) + i tan(b))/(1 + i tanh(a) tan(b))
void cmplxTanh(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b) {
#ifndef TINY_BUILD
	decNumber ta, tb, t2;

	if (dn_eq0(b)) {
		decNumberTanh(rx, a);
		decNumberZero(ry);
	} else {
		dn_sincos(b, &ta, &t2);
		dn_divide(&tb, &ta, &t2);
		decNumberTanh(&ta, a);

		dn_multiply(&t2, &ta, &tb);

		cmplxDivide(rx, ry, &ta, &tb, &const_1, &t2);
	}
#endif
}


void cmplx_1x(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b) {
	decNumber t, u, s, c;

	decNumberMod(&u, a, &const_2);
	dn_mulPI(&t, &u);
	sincosTaylor(&t, &s, &c);
	dn_mulPI(&u, b);
	dn_minus(&t, &u);
	dn_exp(&u, &t);
	cmplxMultiplyReal(rx, ry, &c, &s, &u);
}

// ln(a + i b) = ln(sqrt(a*a + b*b)) + i (2*arctan(signum(b)) - arctan(a/b))
// signum(b) = 1 if b>0, 0 if b=0, -1 if b<0, atan(1) = pi/4
void cmplxLn(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b) {
#ifndef TINY_BUILD
	decNumber u;

	if (dn_eq0(b)) {
		if (dn_eq0(a)) {
			cmplx_NaN(rx, ry);
		} else if (decNumberIsNegative(a)) {
			dn_minus(&u, a);
			dn_ln(rx, &u);
			decNumberPI(ry);
		} else {
			decNumberZero(ry);
			dn_ln(rx, a);
		}
		return;
	}
	if (decNumberIsSpecial(a)  || decNumberIsSpecial(b)) {
		if (decNumberIsNaN(a) || decNumberIsNaN(b)) {
			cmplx_NaN(rx, ry);
		} else {
			if (decNumberIsNegative(b))
				set_neginf(ry);
			else
				set_inf(ry);
			set_inf(rx);
		}
	} else {
		cmplxToPolar(&u, ry, a, b);
		dn_ln(rx, &u);
	}
#endif
}

// e ^ ( a + i b ) = e^a cos(b) + i e^a sin(b)
void cmplxExp(decNumber *rx, decNumber *ry, const decNumber *a, const decNumber *b) {
#ifndef TINY_BUILD
	decNumber e, s, c;

	if (dn_eq0(b)) {
		dn_exp(rx, a);
		decNumberZero(ry);
	} else if (decNumberIsSpecial(a) || decNumberIsSpecial(b)) {
		cmplx_NaN(rx, ry);
	} else {
		dn_exp(&e, a);
		dn_sincos(b, &s, &c);
		dn_multiply(rx, &e, &c);
		dn_multiply(ry, &e, &s);
	}
#endif
}

#ifndef TINY_BUILD
static void c_lg(decNumber *rx, decNumber *ry, const decNumber *x, const decNumber *y) {
	decNumber s1, s2, t1, u1, u2, v1, v2, r;
	int k;

	decNumberZero(&u1);
	decNumberZero(&u2);
	dn_add(&t1, x, &const_21);		// (t1, y)
	for (k=20; k>=0; k--) {
		extern const decNumber *const gamma_consts[];
		cmplxDivideRealBy(&s1, &s2, gamma_consts[k], &t1, y);
		dn_dec(&t1);
		cmplxAdd(&u1, &u2, &u1, &u2, &s1, &s2);
	}
	dn_add(&t1, &u1, &const_gammaC00);	// (t1, u2)
	cmplxLn(&s1, &s2, &t1, &u2);		// (s1, s2)

	dn_add(&r, x, &const_gammaR);		// (r, y)
	cmplxLn(&u1, &u2, &r, y);
	dn_add(&t1, x, &const_0_5);		// (t1, y)
	cmplxMultiply(&v1, &v2, &t1, y, &u1, &u2);
	cmplxSubtract(&u1, &u2, &v1, &v2, &r, y);
	cmplxAdd(rx, ry, &u1, &u2, &s1, &s2);
}
#endif


void cmplxGamma(decNumber *rx, decNumber *ry, const decNumber *xin, const decNumber *y) {
#ifndef TINY_BUILD
	decNumber x, s1, s2, t1, t2, u1, u2;
	int reflec = 0;

	// Check for special cases
	if (decNumberIsSpecial(xin) || decNumberIsSpecial(y)) {
		if (decNumberIsNaN(xin) || decNumberIsNaN(y))
			cmplx_NaN(rx, ry);
		else {
			if (decNumberIsInfinite(xin)) {
				if (decNumberIsInfinite(y))
					cmplx_NaN(rx, ry);
				else if (decNumberIsNegative(xin))
					cmplx_NaN(rx, ry);
				else {
					set_inf(rx);
					decNumberZero(ry);
				}
			} else {
				decNumberZero(rx);
				decNumberZero(ry);
			}
		}
		return;
	}

	// Correct out argument and begin the inversion if it is negative
	if (decNumberIsNegative(xin)) {
		reflec = 1;
		dn_1m(&t1, xin);
		if (dn_eq0(y) && is_int(&t1)) {
			cmplx_NaN(rx, ry);
			return;
		}
		dn_m1(&x, &t1);
	} else
		dn_m1(&x, xin);

	// Sum the series
	c_lg(&s1, &s2, &x, y);
	cmplxExp(rx, ry, &s1, &s2);

	// Finally invert if we started with a negative argument
	if (reflec) {
		cmplxMultiplyReal(&t1, &t2, xin, y, &const_PI);
		cmplxSin(&s1, &s2, &t1, &t2);
		cmplxMultiply(&u1, &u2, &s1, &s2, rx, ry);
		cmplxDivideRealBy(rx, ry, &const_PI, &u1, &u2);
	}
#endif
}

void cmplxLnGamma(decNumber *rx, decNumber *ry, const decNumber *xin, const decNumber *y) {
#ifndef TINY_BUILD
	decNumber x, s1, s2, t1, t2, u1, u2;
	int reflec = 0;

	// Check for special cases
	if (decNumberIsSpecial(xin) || decNumberIsSpecial(y)) {
		if (decNumberIsNaN(xin) || decNumberIsNaN(y))
			cmplx_NaN(rx, ry);
		else {
			if (decNumberIsInfinite(xin)) {
				if (decNumberIsInfinite(y))
					cmplx_NaN(rx, ry);
				else if (decNumberIsNegative(xin))
					cmplx_NaN(rx, ry);
				else {
					set_inf(rx);
					decNumberZero(ry);
				}
			} else {
				decNumberZero(rx);
				decNumberZero(ry);
			}
		}
		return;
	}


	// Correct out argument and begin the inversion if it is negative
	if (decNumberIsNegative(xin)) {
		reflec = 1;
		dn_1m(&t1, xin);
		if (dn_eq0(y) && is_int(&t1)) {
			cmplx_NaN(rx, ry);
			return;
		}
		dn_m1(&x, &t1);
	} else
		dn_m1(&x, xin);

	c_lg(rx, ry, &x, y);

	// Finally invert if we started with a negative argument
	if (reflec) {
		cmplxMultiplyReal(&t1, &t2, xin, y, &const_PI);
		cmplxSin(&s1, &s2, &t1, &t2);
		cmplxDivideRealBy(&u1, &u2, &const_PI, &s1, &s2);
		cmplxLn(&t1, &t2, &u1, &u2);
		cmplxSubtract(rx, ry, &t1, &t2, rx, ry);
	}
#endif
}


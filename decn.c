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
#include "xeq.h"
#include "consts.h"
#include "complex.h"
#include "stats.h"
#include "int.h"
#include "serial.h"
#include "lcd.h"

// #define DUMP1
#ifdef DUMP1
#include <stdio.h>
static FILE *debugf = NULL;

static void open_debug(void) {
	if (debugf == NULL) {
		debugf = fopen("/dev/ttys001", "w");
	}
}
static void dump1(const decNumber *a, const char *msg) {
	char buf[2000], *b = buf;

	open_debug();
	if (decNumberIsNaN(a)) b= "NaN";
	else if (decNumberIsInfinite(a)) b = decNumberIsNegative(a)?"-inf":"inf";
	else
		decNumberToString(a, b);
	fprintf(debugf, "%s: %s\n", msg ? msg : "???", b);
	fflush(debugf);
}
#else
#define dump1(a,b)
#endif


#define MOD_DIGITS	450		/* Big enough for 1e384 mod small integer */
#define BIGMOD_DIGITS	820		/* Big enough for maxreal mod minreal */
#define SINCOS_DIGITS	51		/* Extra digits to give an accurate COS at pi/2 -- only needs to be 46 or 47 but this is the next even size up */


/* Some basic conditional tests */
int dn_lt0(const decNumber *x) {
	return decNumberIsNegative(x) && ! decNumberIsZero(x);
}

int dn_le0(const decNumber *x) {
	return decNumberIsNegative(x) || decNumberIsZero(x);
}

int dn_eq0(const decNumber *x) {
	return decNumberIsZero(x);
}

int dn_eq(const decNumber *x, const decNumber *y) {
	decNumber a;
	return decNumberIsZero(dn_compare(&a, x, y));
}

int dn_eq1(const decNumber *x) {
	return dn_eq(x, &const_1);
}

int dn_lt(const decNumber *x, const decNumber *y) {
	decNumber a;
	return dn_lt0(dn_compare(&a, x, y));
}

int dn_abs_lt(const decNumber *x, const decNumber *tol) {
	decNumber a;
	return dn_lt(dn_abs(&a, x), tol);
}

int relative_error(const decNumber *x, const decNumber *y, const decNumber *tol) {
	decNumber a, b;

	if (dn_eq0(x))
		return dn_abs_lt(y, tol);
	dn_subtract(&a, x, y);
	dn_divide(&b, &a, x);
	return dn_abs_lt(&b, tol);
}

int absolute_error(const decNumber *x, const decNumber *y, const decNumber *tol) {
	decNumber a;
	return dn_abs_lt(dn_subtract(&a, x, y), tol);
}

const decNumber *convergence_threshold(void) {
	return is_dblmode() ? &const_1e_32 : &const_1e_24;
}


/* Some wrapper rountines to save space
 */
decNumber *dn_add(decNumber *r, const decNumber *a, const decNumber *b) {
	return decNumberAdd(r, a, b, &Ctx);
}

decNumber *dn_subtract(decNumber *r, const decNumber *a, const decNumber *b) {
	return decNumberSubtract(r, a, b, &Ctx);
}

decNumber *dn_multiply(decNumber *r, const decNumber *a, const decNumber *b) {
	return decNumberMultiply(r, a, b, &Ctx);
}

decNumber *dn_divide(decNumber *r, const decNumber *a, const decNumber *b) {
	return decNumberDivide(r, a, b, &Ctx);
}

decNumber *dn_compare(decNumber *r, const decNumber *a, const decNumber *b) {
	return decNumberCompare(r, a, b, &Ctx);
}

decNumber *dn_min(decNumber *r, const decNumber *a, const decNumber *b) {
	return decNumberMin(r, a, b, &Ctx);
}

decNumber *dn_max(decNumber *r, const decNumber *a, const decNumber *b) {
	return decNumberMax(r, a, b, &Ctx);
}

decNumber *dn_abs(decNumber *r, const decNumber *a) {
	return decNumberAbs(r, a, &Ctx);
}

decNumber *dn_minus(decNumber *r, const decNumber *a) {
	return decNumberMinus(r, a, &Ctx);
}

decNumber *dn_plus(decNumber *r, const decNumber *a) {
	return decNumberPlus(r, a, &Ctx);
}

decNumber *dn_sqrt(decNumber *r, const decNumber *a) {
	return decNumberSquareRoot(r, a, &Ctx);
}

decNumber *dn_exp(decNumber *r, const decNumber *a) {
	return decNumberExp(r, a, &Ctx);
}


decNumber *dn_average(decNumber *r, const decNumber *a, const decNumber *b) {
	decNumber z;

	dn_add(&z, a, b);
	return dn_div2(r, &z);
}


#if 0
/* Define a table of small integers.
 * This should be equal or larger than any of the summation integers required in the
 * various series approximations to avoid needless computation.
 */
#define MAX_SMALL_INT	9
static const decNumber *const small_ints[MAX_SMALL_INT+1] = {
	&const_0, &const_1, &const_2, &const_3, &const_4,
	&const_5, &const_6, &const_7, &const_8, &const_9,
};

void ullint_to_dn(decNumber *x, unsigned long long int n) {
	/* Check to see if the number is small enough to be in our table */
	if (n <= MAX_SMALL_INT) {
		decNumberCopy(x, small_ints[n]);
	} else {
		/* Got to do this the long way */
		decNumber z;
		int shift = 0;

		decNumberZero(x);

		while (n != 0) {
			const int r = n % 10;
			n /= 10;
			if (r != 0) {
				dn_mulpow10(&z, small_ints[r], shift);
				dn_add(x, x, &z);
			}
			++shift;
		}
	}
}
#endif

void int_to_dn(decNumber *x, int n) {
	int sgn;

	/* Account for negatives */
	if (n < 0) {
		sgn = 1;
		n = -n;
	} else
		sgn = 0;

	ullint_to_dn(x, n);

	if (sgn)
		dn_minus(x, x);
}

int dn_to_int(const decNumber *x) {
	decNumber y;
#if 0
	char buf[64];

	decNumberRescale(&y, x, &const_0, &Ctx);
	decNumberToString(&y, buf);
	return s_to_i(buf);
#else
	return decGetInt(decNumberTrunc(&y, x));
#endif
}

unsigned long long int dn_to_ull(const decNumber *x, int *sgn) {
	decNumber y, z;
	char buf[64];

	decNumberTrunc(&z, x);
	decNumberRescale(&y, &z, &const_0, &Ctx);
	if (decNumberIsNegative(&z)) {
		dn_minus(&y, &y);
		*sgn = 1;
	} else
		*sgn = 0;
	decNumberToString(&y, buf);
	return s_to_ull(buf, 10);
}


decNumber *set_NaN(decNumber *x) {
	if (x != NULL)
		decNumberCopy(x, &const_NaN);
	return x;
}

decNumber *set_inf(decNumber *x) {
	return decNumberCopy(x, &const_inf);
}

decNumber *set_neginf(decNumber *x) {
	return decNumberCopy(x, &const__inf);
}


void decNumberPI(decNumber *pi) {
	decNumberCopy(pi, &const_PI);
}
void decNumberPIon2(decNumber *pion2) {
	decNumberCopy(pion2, &const_PIon2);
}

/* Check if a number is an integer.
 */
int is_int(const decNumber *x) {
	enum rounding a = Ctx.round;
	decNumber r, y;

	if (decNumberIsNaN(x))
		return 0;
	if (decNumberIsInfinite(x))
		return 1;

	Ctx.round = DEC_ROUND_DOWN;
	decNumberToIntegralValue(&y, x, &Ctx);
	Ctx.round = a;

	dn_subtract(&r, x, &y);
	if (! dn_eq0(&r))
		return 0;
	return 1;
}

/* Utility routine that checks if the X register is even or odd or neither.
 * Returns positive if even, zero if odd, -1 for special, -2 for fractional.
 */
int is_even(const decNumber *x) {
	decNumber y, z;

	if (decNumberIsSpecial(x))
		return -1;
	dn_abs(&z, x);
	decNumberMod(&y, &z, &const_2);
	if (dn_eq0(&y))
		return 1;
	if (dn_eq1(&y))
		return 0;
	return -2;
}

decNumber *dn_inc(decNumber *x) {
	return dn_add(x, x, &const_1);
}

decNumber *dn_dec(decNumber *x) {
	return dn_subtract(x, x, &const_1);
}

decNumber *dn_p1(decNumber *r, const decNumber *x) {
	return dn_add(r, x, &const_1);
}

decNumber *dn_m1(decNumber *r, const decNumber *x) {
	return dn_subtract(r, x, &const_1);
}

decNumber *dn_1m(decNumber *r, const decNumber *x) {
	return dn_subtract(r, &const_1, x);
}

decNumber *dn_1(decNumber *r) {
	return decNumberCopy(r, &const_1);
}

decNumber *dn__1(decNumber *r) {
	return decNumberCopy(r, &const__1);
}

decNumber *dn_p2(decNumber *r, const decNumber *x) {
	return dn_add(r, x, &const_2);
}

decNumber *dn_mul2(decNumber *r, const decNumber *x) {
	return dn_multiply(r, x, &const_2);
}

decNumber *dn_div2(decNumber *r, const decNumber *x) {
	return dn_multiply(r, x, &const_0_5);
}

decNumber *dn_mul100(decNumber *r, const decNumber *x) {
	return dn_mulpow10(r, x, 2);
}

decNumber *dn_mulPI(decNumber *r, const decNumber *x) {
	return dn_multiply(r, x, &const_PI);
}

decNumber *dn_mulpow10(decNumber *r, const decNumber *x, int p) {
	decNumberCopy(r, x);
	r->exponent += p;
	return r;
}

/* Mantissa of a number
 */
#ifdef INCLUDE_MANTISSA
decNumber *decNumberMantissa(decNumber *r, const decNumber *x) {
	if (decNumberIsSpecial(x))
		return set_NaN(r);
	if (dn_eq0(x))
		return decNumberCopy(r, x);
	decNumberCopy(r, x);
	r->exponent = 1 - r->digits;
	return r;
}

/* Exponenet of a number
 */
decNumber *decNumberExponent(decNumber *r, const decNumber *x) {
	if (decNumberIsSpecial(x))
		return set_NaN(r);
	if (dn_eq0(x))
		return decNumberZero(r);
	int_to_dn(r, x->exponent + x->digits - 1);
	return r;
}

/* 1 ULP
 */
static int realULP(decNumber *r, const decNumber *x) {
	int dblmode;
	int subnormal = 0;
	int expshift;
	int minexp;

	if (decNumberIsSpecial(x)) {
		if (decNumberIsInfinite(x))
			set_inf(r);
		else
			set_NaN(r);
		return 0;
	}

	dblmode = is_dblmode();

	if (dblmode) {
		expshift = DECIMAL128_Pmax;
		minexp = DECIMAL128_Emin - DECIMAL128_Pmax + 1;
		if (x->exponent < DECIMAL128_Emin)
			subnormal = 1;
	} else {
		expshift = DECIMAL64_Pmax;
		minexp = DECIMAL64_Emin - DECIMAL64_Pmax + 1;
		if (x->exponent < DECIMAL64_Emin)
			subnormal = 1;
	}

	dn_1(r);
	if (dn_eq0(x) || subnormal)
		r->exponent = minexp;
	else
		r->exponent = x->exponent + x->digits - expshift;
	return subnormal;
}

decNumber *decNumberULP(decNumber *r, const decNumber *x) {
	realULP(r, x);
	return r;
}

decNumber *decNumberNeighbour(decNumber *r, const decNumber *y, const decNumber *x) {
	decNumber ulp;
	int down, subnormal;

	if (decNumberIsNaN(y))
		return set_NaN(r);
	if (decNumberIsSpecial(x))
		return decNumberCopy(r, x);
	dn_compare(&ulp, y, x);
	if (dn_eq0(&ulp))
		return decNumberCopy(r, y);

	down = decNumberIsNegative(&ulp) ? 1 : 0;
	subnormal = realULP(&ulp, x);

	if (! subnormal && x->digits == 1 && x->lsu[0] == 1)
		ulp.exponent -= (! decNumberIsNegative(x)) == down;
	if (down)
		dn_subtract(r, x, &ulp);
	else
		dn_add(r, x, &ulp);
	return r;
}
#endif

/* Multiply Add: x + y * z
 */
#ifdef INCLUDE_MULADD
decNumber *decNumberMAdd(decNumber *r, const decNumber *z, const decNumber *y, const decNumber *x) {
	decNumber t;

	dn_multiply(&t, x, y);
	return dn_add(r, &t, z);
}
#endif


/* Reciprocal of a number.
 * Division is correctly rounded so we just use that instead of coding
 * something special (that could be more efficient).
 */
decNumber *decNumberRecip(decNumber *r, const decNumber *x) {
	return dn_divide(r, &const_1, x);
}

/* Reciprocal of a function's result.
 * This routine calls the specified function and then multiplicatively
 * inverts its result.
 */
#if 0
static decNumber *dn_recip(decNumber *r, const decNumber *x,
		decNumber *(*func)(decNumber *r, const decNumber *x)) {
	decNumber z;

	(*func)(&z, x);
	return decNumberRecip(r, &z);
}
#endif

/* A plethora of round to integer functions to support the large variety
 * of possibilities in this area.  Start with a common utility function
 * that saves the current rounding mode, rounds as required and restores
 * the rounding mode properly.
 */
static decNumber *round2int(decNumber *r, const decNumber *x, enum rounding mode) {
	enum rounding a = Ctx.round;

	Ctx.round = mode;
	decNumberToIntegralValue(r, x, &Ctx);
	Ctx.round = a;
	return r;
}

/* Floor - truncate to minus infinity.
 */
decNumber *decNumberFloor(decNumber *r, const decNumber *x) {
	return round2int(r, x, DEC_ROUND_FLOOR);
}

/* Ceiling - truncate to plus infinity.
 */
decNumber *decNumberCeil(decNumber *r, const decNumber *x) {
	return round2int(r, x, DEC_ROUND_CEILING);
}

/* Trunc - truncate to zero.
 */
decNumber *decNumberTrunc(decNumber *r, const decNumber *x) {
	return round2int(r, x, DEC_ROUND_DOWN);
}

/* Round - round 0.5 up.
 */
decNumber *decNumberRound(decNumber *r, const decNumber *x) {
	return round2int(r, x, DEC_ROUND_HALF_UP);
}

/* Intg - round 0.5 even.
 */
static decNumber *decNumberIntg(decNumber *r, const decNumber *x) {
	return round2int(r, x, DEC_ROUND_HALF_EVEN);
}

/* Frac - round 0.5 up.
 */
decNumber *decNumberFrac(decNumber *r, const decNumber *x) {
	decNumber y;

	round2int(&y, x, DEC_ROUND_DOWN);
	return dn_subtract(r, x, &y);
}


static void dn_gcd(decNumber *r, const decNumber *x, const decNumber *y, int bigmod) {
	decNumber b, t;

	decNumberCopy(&b, y);
	decNumberCopy(r, x);
	while (! dn_eq0(&b)) {
		decNumberCopy(&t, &b);
		if (bigmod)
			decNumberBigMod(&b, r, &t);
		else
			decNumberMod(&b, r, &t);
		decNumberCopy(r, &t);
	}
}

static int dn_check_gcd(decNumber *r, const decNumber *x, const decNumber *y,
		decNumber *a, decNumber *b) {
	if (decNumberIsSpecial(x) || decNumberIsSpecial(y)) {
		if (decNumberIsNaN(x) || decNumberIsNaN(y))
			set_NaN(r);
		else
			set_inf(r);
	} else if (!is_int(x) || !is_int(y))
		set_NaN(r);
	else {
		dn_abs(a, x);
		dn_abs(b, y);
		return 0;
	}
	return 1;
}

decNumber *decNumberGCD(decNumber *r, const decNumber *x, const decNumber *y) {
	decNumber a, b;

	if (dn_check_gcd(r, x, y, &a, &b))
		return r;

	if(dn_eq0(x))
		decNumberCopy(r, &b);
	else if (dn_eq0(y))
		decNumberCopy(r, &a);
	else
		dn_gcd(r, &a, &b, 1);
	return r;
}

decNumber *decNumberLCM(decNumber *r, const decNumber *x, const decNumber *y) {
	decNumber gcd, a, b, t;

	if (dn_check_gcd(r, x, y, &a, &b))
		return r;

	if(dn_eq0(x) || dn_eq0(y))
		decNumberCopy(r, x);
	dn_gcd(&gcd, &a, &b, 1);
	dn_divide(&t, &a, &gcd);
	return dn_multiply(r, &t, &b);
}


/* The extra logarithm and power functions */

/* Raise y^x */
decNumber *dn_power_internal(decNumber *r, const decNumber *y, const decNumber *x, const decNumber *logy) {
	decNumber s, t, my;
	int isxint, xodd, ynegative;
	int negate = 0;

	if (dn_eq1(y))
		return dn_1(r);				// 1^x = 1

	if (dn_eq0(x))
		return dn_1(r);				// y^0 = 1

	if (decNumberIsNaN(x) || decNumberIsNaN(y))
		return set_NaN(r);

	if (dn_eq1(x))
		return decNumberCopy(r, y); 		// y^1 = y

	if (decNumberIsInfinite(x)) {
		int ylt1;
		if (dn_eq(y, &const__1))
			return dn_1(r);			// -1 ^ +/-inf = 1

		ylt1 = dn_abs_lt(y, &const_1);
		if (decNumberIsNegative(x)) {
			if (ylt1)
				return set_inf(r);		// y^-inf |y|<1 = +inf
			return decNumberZero(r);		// y^-inf |y|>1 = +0
		}
		if (ylt1)
			return decNumberZero(r);		// y^inf |y|<1 = +0
		return set_inf(r);				// y^inf |y|>1 = +inf
	}

	isxint = is_int(x);
	ynegative = decNumberIsNegative(y);
	if (decNumberIsInfinite(y)) {
		if (ynegative) {
			xodd = isxint && is_even(x) == 0;
			if (decNumberIsNegative(x)) {
				decNumberZero(r);		// -inf^x x<0 = +0
				if (xodd)			// -inf^x odd x<0 = -0
					return decNumberCopy(r, &const__0);
				return r;
			}
			if (xodd)
				return set_neginf(r);		// -inf^x odd x>0 = -inf
			return set_inf(r);			// -inf^x x>0 = +inf
		}
		if (decNumberIsNegative(x))
			return decNumberZero(r);		// +inf^x x<0 = +0
		return set_inf(r);				// +inf^x x>0 = +inf
	}

	if (dn_eq0(y)) {
		xodd = isxint && is_even(x) == 0;
		if (decNumberIsNegative(x)) {
			if (xodd && ynegative)
				return set_neginf(r);		// -0^x odd x<0 = -inf
			return set_inf(r);			// 0^x x<0 = +inf
		}
		if (xodd && ynegative)
			return decNumberCopy(r, &const__0);	// -0^x odd x>0 = -/+0
		return decNumberZero(r);			// 0^x x>0 = +0
	}

	if (ynegative) {
		if (!isxint)
			return set_NaN(r);			// y^x y<0, x not odd int = NaN
		if (is_even(x) == 0)				// y^x, y<0, x odd = - ((-y)^x)
			negate = 1;
		dn_minus(&my, y);
		y = &my;
	}
	if (logy == NULL) {
		dn_ln(&t, y);
		logy = &t;
	}
	dn_multiply(&s, logy, x);
	dn_exp(r, &s);
	if (negate)
		return dn_minus(r, r);
	return r;
}

decNumber *dn_power(decNumber *r, const decNumber *y, const decNumber *x) {
	return dn_power_internal(r, y, x, NULL);
}


/* ln(1+x) */
decNumber *decNumberLn1p(decNumber *r, const decNumber *x) {
	decNumber u, v, w;

	if (decNumberIsSpecial(x) || dn_eq0(x)) {
		return decNumberCopy(r, x);
	}
	dn_p1(&u, x);
	dn_m1(&v, &u);
	if (dn_eq0(&v)) {
		return decNumberCopy(r, x);
	}
	dn_divide(&w, x, &v);
	dn_ln(&v, &u);
	return dn_multiply(r, &v, &w);
}

/* exp(x)-1 */
decNumber *decNumberExpm1(decNumber *r, const decNumber *x) {
	decNumber u, v, w;

	if (decNumberIsSpecial(x)) {
		return decNumberCopy(r, x);
	}
	dn_exp(&u, x);
	dn_m1(&v, &u);
	if (dn_eq0(&v)) {
		return decNumberCopy(r, x);
	}
	if (dn_eq(&v, &const__1)) {
		return dn__1(r);
	}
	dn_multiply(&w, &v, x);
	dn_ln(&v, &u);
	return dn_divide(r, &w, &v);
}


decNumber *do_log(decNumber *r, const decNumber *x, const decNumber *base) {
	decNumber y;

	if (decNumberIsSpecial(x)) {
		if (decNumberIsNaN(x) || decNumberIsNegative(x))
			return set_NaN(r);
		return set_inf(r);
	}
	dn_ln(&y, x);
	return dn_divide(r, &y, base);
}

/* Natural logarithm.
 *
 * Take advantage of the fact that we store our numbers in the form: m * 10^e
 * so log(m * 10^e) = log(m) + e * log(10)
 * do this so that m is always in the range 0.1 <= m < 2.  However if the number
 * is already in the range 0.5 .. 1.5, this step is skipped.
 *
 * Then use the fact that ln(x^2) = 2 * ln(x) to range reduce the mantissa
 * into 1/sqrt(2) <= m < 2.
 *
 * Finally, apply the series expansion:
 *   ln(x) = 2(a+a^3/3+a^5/5+...) where a=(x-1)/(x+1)
 * which converges quickly for an argument near unity.
 */
decNumber *dn_ln(decNumber *r, const decNumber *x) {
	decNumber z, t, f, n, m, i, v, w, e;
	const decNumber *tol = is_dblmode() ? &const_1e_37 : &const_1e_32;
	int expon;

	if (decNumberIsSpecial(x)) {
		if (decNumberIsNaN(x) || decNumberIsNegative(x))
			return set_NaN(r);
		return set_inf(r);
	}
	if (dn_le0(x)) {
		if (decNumberIsNegative(x))
			return set_NaN(r);
		return set_neginf(r);
	}
	decNumberCopy(&z, x);
	decNumberCopy(&f, &const_2);
	dn_m1(&t, x);
	dn_abs(&v, &t);
	if (dn_gt(&v, &const_0_5)) {
		expon = z.exponent + z.digits;
		z.exponent = -z.digits;
	} else
		expon = 0;

/* The too high case never happens
	while (dn_le(&const_2, &z)) {
		dn_mul2(&f, &f);
		dn_sqrt(&z, &z);
	}
*/
	// Range reduce the value by repeated square roots.
	// Making the constant here larger will reduce the number of later
	// iterations at the expense of more square root operations.
	while (dn_le(&z, &const_root2on2)) {
		dn_mul2(&f, &f);
		dn_sqrt(&z, &z);
	}
	dn_p1(&t, &z);
	dn_m1(&v, &z);
	dn_divide(&n, &v, &t);
	decNumberCopy(&v, &n);
	decNumberSquare(&m, &v);
	decNumberCopy(&i, &const_3);

	for (;;) {
		dn_multiply(&n, &m, &n);
		dn_divide(&e, &n, &i);
		dn_add(&w, &v, &e);
		if (relative_error(&w, &v, tol))
			break;
		decNumberCopy(&v, &w);
		dn_p2(&i, &i);
	}
	dn_multiply(r, &f, &w);
	if (expon == 0)
		return r;
	int_to_dn(&e, expon);
	dn_multiply(&w, &e, &const_ln10);
	return dn_add(r, r, &w);
}

decNumber *dn_log2(decNumber *r, const decNumber *x) {
	return do_log(r, x, &const_ln2);
}

decNumber *dn_log10(decNumber *r, const decNumber *x) {
	return do_log(r, x, &const_ln10);
}

decNumber *decNumberLogxy(decNumber *r, const decNumber *y, const decNumber *x) {
	decNumber lx;

	dn_ln(&lx, x);
	return do_log(r, y, &lx);
}

decNumber *decNumberPow2(decNumber *r, const decNumber *x) {
	return dn_power_internal(r, &const_2, x, &const_ln2);
}

decNumber *decNumberPow10(decNumber *r, const decNumber *x) {
	return dn_power_internal(r, &const_10, x, &const_ln10);
}

decNumber *decNumberPow_1(decNumber *r, const decNumber *x) {
	int even = is_even(x);
	decNumber t, u;

	if (even == 1)
		return dn_1(r);
	if (even == 0)
		return dn__1(r);
	decNumberMod(&u, x, &const_2);
	dn_mulPI(&t, &u);
	sincosTaylor(&t, NULL, r);
	return r;
}

/* Square - this almost certainly could be done more efficiently
 */
decNumber *decNumberSquare(decNumber *r, const decNumber *x) {
	return dn_multiply(r, x, x);
}

/* Cube - again could be done more efficiently */
decNumber *decNumberCube(decNumber *r, const decNumber *x) {
	decNumber z;

	decNumberSquare(&z, x);
	return dn_multiply(r, &z, x);
}

/* Cube root */
decNumber *decNumberCubeRoot(decNumber *r, const decNumber *x) {
	decNumber third, t;

	decNumberRecip(&third, &const_3);

	if (decNumberIsNegative(x)) {
		dn_minus(r, x);
		dn_power(&t, r, &third);
		return dn_minus(r, &t);
	}
	return dn_power(r, x, &third);
}

#ifdef INCLUDE_XROOT
decNumber *decNumberXRoot(decNumber *r, const decNumber *a, const decNumber *b) {
	decNumber s, t;

	decNumberRecip(&s, b);

	if (decNumberIsNegative(a)) {
		if (is_even(b) == 0) {
			dn_minus(r, a);
			dn_power(&t, r, &s);
			return dn_minus(r, &t);
		}
		return set_NaN(r);
	}
	return dn_power(r, a, &s);
}
#endif



decNumber *decNumberMod(decNumber *res, const decNumber *x, const decNumber *y) {
	/* Declare a structure large enough to hold a really long number.
	 * This structure is likely to be larger than is required.
	 */
	struct {
		decNumber n;
		decNumberUnit extra[((MOD_DIGITS-DECNUMDIGITS+DECDPUN-1)/DECDPUN)];
	} out;

	int digits = Ctx.digits;

	Ctx.digits = MOD_DIGITS;
	decNumberRemainder(&out.n, x, y, &Ctx);
	Ctx.digits = digits;

	return decNumberPlus(res, &out.n, &Ctx);
}


decNumber *decNumberBigMod(decNumber *res, const decNumber *x, const decNumber *y) {
	/* Declare a structure large enough to hold a really long number.
	 * This structure is likely to be larger than is required.
	 */
	struct {
		decNumber n;
		decNumberUnit extra[((BIGMOD_DIGITS-DECNUMDIGITS+DECDPUN-1)/DECDPUN)];
	} out;

	int digits = Ctx.digits;

	Ctx.digits = BIGMOD_DIGITS;
	decNumberRemainder(&out.n, x, y, &Ctx);
	Ctx.digits = digits;

#ifdef INCLUDE_MOD41
	decNumberPlus(res, &out.n, &Ctx);
	if (XeqOpCode == (OP_DYA | OP_MOD41) && decNumberIsNegative(x) != decNumberIsNegative(y) && !dn_eq0(res))
		dn_add(res, res, y);
	return res;
#else
	return 	decNumberPlus(res, &out.n, &Ctx);
#endif
}


/* Calculate sin and cos by Taylor series
 */
typedef struct {
	decNumber n;
	decNumberUnit extra[((SINCOS_DIGITS-DECNUMDIGITS+DECDPUN-1)/DECDPUN)];
} sincosNumber;

void sincosTaylor(const decNumber *a, decNumber *sout, decNumber *cout) {
	sincosNumber a2, t, j, z, s, c;
	int i, fins = sout == NULL, finc = cout == NULL;
	const int digits = Ctx.digits;
	Ctx.digits = SINCOS_DIGITS;

	dn_multiply(&a2.n, a, a);
	dn_1(&j.n);
	dn_1(&t.n);
	dn_1(&s.n);
	dn_1(&c.n);

	for (i=1; !(fins && finc) && i < 1000; i++) {
		const int odd = i & 1;

		dn_inc(&j.n);
		dn_divide(&z.n, &a2.n, &j.n);
		dn_multiply(&t.n, &t.n, &z.n);
		if (!finc) {
			decNumberCopy(&z.n, &c.n);
			if (odd)
				dn_subtract(&c.n, &c.n, &t.n);
			else
				dn_add(&c.n, &c.n, &t.n);
			if (dn_eq(&c.n, &z.n))
				finc = 1;
		}

		dn_inc(&j.n);
		dn_divide(&t.n, &t.n, &j.n);
		if (!fins) {
			decNumberCopy(&z.n, &s.n);
			if (odd)
				dn_subtract(&s.n, &s.n, &t.n);
			else
				dn_add(&s.n, &s.n, &t.n);
			if (dn_eq(&s.n, &z.n))
				fins = 1;
		}
	}
	Ctx.digits = digits;
	if (sout != NULL)
		dn_multiply(sout, &s.n, a);
	if (cout != NULL)
		dn_plus(cout, &c.n);
}


/*
 *  Common helper for angle conversions
 *  Checks the present mode and converts accordingly
 */
static decNumber *decNumberDRG_internal(decNumber *res, const decNumber *x, s_opcode op) {
#define DRG(op,mode) (((op) << 2) | (mode))
	
	switch (DRG(argKIND(op), get_trig_mode())) {

	case DRG(OP_2DEG,  TRIG_RAD):
	case DRG(OP_RAD2,  TRIG_DEG):
		return decNumberR2D(res, x);

	case DRG(OP_2DEG,  TRIG_GRAD):
	case DRG(OP_GRAD2, TRIG_DEG):
		return decNumberG2D(res, x);

	case DRG(OP_2RAD,  TRIG_DEG):
	case DRG(OP_DEG2,  TRIG_RAD):
		return decNumberD2R(res, x);

	case DRG(OP_2RAD,  TRIG_GRAD):
	case DRG(OP_GRAD2, TRIG_RAD):
		return decNumberG2R(res, x);

	case DRG(OP_2GRAD, TRIG_DEG):
	case DRG(OP_DEG2,  TRIG_GRAD):
		return decNumberD2G(res, x);

	case DRG(OP_2GRAD, TRIG_RAD):
	case DRG(OP_RAD2,  TRIG_GRAD):
		return decNumberR2G(res, x);

	default:
		return decNumberCopy(res, x);
	}
#undef DRG
}


/* Check for right angle multiples and if exact, return the apropriate
 * quadrant constant directly.
 */
static int right_angle(decNumber *res, const decNumber *x,
		const decNumber *quad, const decNumber *r0, const decNumber *r1,
		const decNumber *r2, const decNumber *r3) {
	decNumber r;
	const decNumber *z;

	decNumberRemainder(&r, x, quad, &Ctx);
	if (!dn_eq0(&r))
		return 0;

	if (dn_eq0(x))
		z = r0;
	else {
		dn_add(&r, quad, quad);
		dn_compare(&r, &r, x);
		if (dn_eq0(&r))
			z = r2;
		else if (decNumberIsNegative(&r))
			z = r3;
		else	z = r1;
	}
	decNumberCopy(res, z);
	return 1;
}

/* Convert the number into radians.
 * We take the opportunity to reduce angles modulo 2 * PI here
 * For degrees and gradians, the reduction is exact and easy.
 * For radians, it involves a lot more computation.
 * For degrees and gradians, we return exact results
 * for right angles and multiples thereof.
 */
static int cvt_2rad(decNumber *res, const decNumber *x,
		const decNumber *r0, const decNumber *r1,
		const decNumber *r2, const decNumber *r3) {
	decNumber fm;
	const decNumber *circle, *right;

	switch (get_trig_mode()) {

	case TRIG_RAD:
		decNumberMod(res, x, &const_2PI);
		break;

	case TRIG_DEG:
		circle = &const_360;
		right = &const_90;
		goto convert;

	case TRIG_GRAD:
		circle = &const_400;
		right = &const_100;
	convert:
		decNumberMod(&fm, x, circle);
		if (decNumberIsNegative(&fm))
			dn_add(&fm, &fm, circle);
		if (r0 != NULL && right_angle(res, &fm, right, r0, r1, r2, r3))
			return 0;
		decNumberDRG_internal(res, &fm, OP_2RAD);
		break;
	}
	return 1;
}

static void cvt_rad2(decNumber *res, const decNumber *x) {
	decNumberDRG_internal(res, x, OP_RAD2);	
}

/* Calculate sin and cos of the given number in radians.
 * We need to do some range reduction to guarantee that our Taylor series
 * converges rapidly.
 */
void dn_sincos(const decNumber *v, decNumber *sinv, decNumber *cosv)
{
	decNumber x;

	if (decNumberIsSpecial(v))
		cmplx_NaN(sinv, cosv);
	else {
		decNumberMod(&x, v, &const_2PI);
		sincosTaylor(&x, sinv, cosv);
	}
}

decNumber *decNumberSin(decNumber *res, const decNumber *x) {
	decNumber x2;

	if (decNumberIsSpecial(x))
		return set_NaN(res);
	else {
		if (cvt_2rad(&x2, x, &const_0, &const_1, &const_0, &const__1))
			sincosTaylor(&x2, res, NULL);
		else
			decNumberCopy(res, &x2);
	}
	return res;
}

decNumber *decNumberCos(decNumber *res, const decNumber *x) {
	decNumber x2;

	if (decNumberIsSpecial(x))
		return set_NaN(res);
	else {
		if (cvt_2rad(&x2, x, &const_1, &const_0, &const__1, &const_0))
			sincosTaylor(&x2, NULL, res);
		else
			decNumberCopy(res, &x2);
	}
	return res;
}

decNumber *decNumberTan(decNumber *res, const decNumber *x) {
	sincosNumber x2, s, c;

	if (decNumberIsSpecial(x))
		return set_NaN(res);
	else {
		const int digits = Ctx.digits;
		Ctx.digits = SINCOS_DIGITS;
		if (cvt_2rad(&x2.n, x, &const_0, &const_NaN, &const_0, &const_NaN)) {
			sincosTaylor(&x2.n, &s.n, &c.n);
			dn_divide(&x2.n, &s.n, &c.n);
		}
		Ctx.digits = digits;
		dn_plus(res, &x2.n);
	}
	return res;
}

#if 0
decNumber *decNumberSec(decNumber *res, const decNumber *x) {
	return dn_recip(res, x, &decNumberCos);
}

decNumber *decNumberCosec(decNumber *res, const decNumber *x) {
	return dn_recip(res, x, &decNumberSin);
}

decNumber *decNumberCot(decNumber *res, const decNumber *x) {
	return dn_recip(res, x, &decNumberTan);
}
#endif

decNumber *decNumberSinc(decNumber *res, const decNumber *x) {
	decNumber s;

	decNumberSquare(&s, x);
	if (dn_eq1(dn_p1(&s, &s)))
		return dn_1(res);
	decNumberMod(res, x, &const_2PI);
	sincosTaylor(res, &s, NULL);
	return dn_divide(res, &s, x);
}

void do_atan(decNumber *res, const decNumber *x) {
	decNumber a, b, a2, t, j, z, last;
	int doubles = 0;
	int invert;
	int neg = decNumberIsNegative(x);
	int n;

	// arrange for a >= 0
	if (neg)
		dn_minus(&a, x);
	else
		decNumberCopy(&a, x);

	// reduce range to 0 <= a < 1, using atan(x) = pi/2 - atan(1/x)
	invert = dn_gt(&a, &const_1);
	if (invert)
		dn_divide(&a, &const_1, &a);

	// Range reduce to small enough limit to use taylor series
	// using:
	//  tan(x/2) = tan(x)/(1+sqrt(1+tan(x)^2))
	for (n = 0; n < 1000; n++) {
		if (dn_le(&a, &const_0_1))
			break;
		doubles++;
		// a = a/(1+sqrt(1+a^2)) -- at most 3 iterations.
		dn_multiply(&b, &a, &a);
		dn_inc(&b);
		dn_sqrt(&b, &b);
		dn_inc(&b);
		dn_divide(&a, &a, &b);
	}

	// Now Taylor series
	// tan(x) = x(1-x^2/3+x^4/5-x^6/7...)
	// We calculate pairs of terms and stop when the estimate doesn't change
	decNumberCopy(res, &const_3);
	decNumberCopy(&j, &const_5);
	dn_multiply(&a2, &a, &a);	// a^2
	decNumberCopy(&t, &a2);
	dn_divide(res, &t, res);	// s = 1-t/3 -- first two terms
	dn_1m(res, res);

	do {	// Loop until there is no digits changed
		decNumberCopy(&last, res);

		dn_multiply(&t, &t, &a2);
		dn_divide(&z, &t, &j);
		dn_add(res, res, &z);
		dn_p2(&j, &j);

		dn_multiply(&t, &t, &a2);
		dn_divide(&z, &t, &j);
		dn_subtract(res, res, &z);
		dn_p2(&j, &j);

	} while (!dn_eq(res, &last));
	dn_multiply(res, res, &a);

	while (doubles) {
		dn_add(res, res, res);
		doubles--;
	}

	if (invert) {
		dn_subtract(res, &const_PIon2, res);
	}

	if (neg)
		dn_minus(res, res);
}

void do_asin(decNumber *res, const decNumber *x) {
	decNumber abx, z;

	if (decNumberIsNaN(x)) {
		set_NaN(res);
		return;
	}

	dn_abs(&abx, x);
	if (dn_gt(&abx, &const_1)) {
		set_NaN(res);
		return;
	}

	// res = 2*atan(x/(1+sqrt(1-x*x)))
	dn_multiply(&z, x, x);
	dn_1m(&z, &z);
	dn_sqrt(&z, &z);
	dn_inc(&z);
	dn_divide(&z, x, &z);
	do_atan(&abx, &z);
	dn_mul2(res, &abx);
}

void do_acos(decNumber *res, const decNumber *x) {
	decNumber abx, z;

	if (decNumberIsNaN(x)) {
		set_NaN(res);
		return;
	}

	dn_abs(&abx, x);
	if (dn_gt(&abx, &const_1)) {
		set_NaN(res);
		return;
	}

	// res = 2*atan((1-x)/sqrt(1-x*x))
	if (dn_eq1(x))
		decNumberZero(res);
	else {
		dn_multiply(&z, x, x);
		dn_1m(&z, &z);
		dn_sqrt(&z, &z);
		dn_1m(&abx, x);
		dn_divide(&z, &abx, &z);
		do_atan(&abx, &z);
		dn_mul2(res, &abx);
	}
}

decNumber *decNumberArcSin(decNumber *res, const decNumber *x) {
	decNumber z;

	do_asin(&z, x);
	cvt_rad2(res, &z);
	return res;
}

decNumber *decNumberArcCos(decNumber *res, const decNumber *x) {
	decNumber z;

	do_acos(&z, x);
	cvt_rad2(res, &z);
	return res;
}

decNumber *decNumberArcTan(decNumber *res, const decNumber *x) {
	decNumber z;

	if (decNumberIsSpecial(x)) {
		if (decNumberIsNaN(x))
			return set_NaN(res);
		else {
			decNumberCopy(&z, &const_PIon2);
			if (decNumberIsNegative(x))
				dn_minus(&z, &z);
		}
	} else
		do_atan(&z, x);
	cvt_rad2(res, &z);
	return res;
}

decNumber *do_atan2(decNumber *at, const decNumber *y, const decNumber *x) {
	decNumber r, t;
	const int xneg = decNumberIsNegative(x);
	const int yneg = decNumberIsNegative(y);

	if (decNumberIsNaN(x) || decNumberIsNaN(y)) {
		return set_NaN(at);
	}
	if (dn_eq0(y)) {
		if (yneg) {
			if (dn_eq0(x)) {
				if (xneg) {
					decNumberPI(at);
					dn_minus(at, at);
				} else
					decNumberCopy(at, y);
			} else if (xneg) {
				decNumberPI(at);
				dn_minus(at, at);
			} else
				decNumberCopy(at, y);
		} else {
			if (dn_eq0(x)) {
				if (xneg)
					decNumberPI(at);
				else
					decNumberZero(at);
			} else if (xneg)
				decNumberPI(at);
			else
				decNumberZero(at);
		}
		return at;
	}
	if (dn_eq0(x)) {
		decNumberPIon2(at);
		if (yneg)
			dn_minus(at, at);
		return at;
	}
	if (decNumberIsInfinite(x)) {
		if (xneg) {
			if (decNumberIsInfinite(y)) {
				decNumberPI(&t);
				dn_multiply(at, &t, &const_0_75);
				if (yneg)
					dn_minus(at, at);
			} else {
				decNumberPI(at);
				if (yneg)
					dn_minus(at, at);
			}
		} else {
			if (decNumberIsInfinite(y)) {
				decNumberPIon2(&t);
				dn_div2(at, &t);
				if (yneg)
					dn_minus(at, at);
			} else {
				decNumberZero(at);
				if (yneg)
					dn_minus(at, at);
			}
		}
		return at;
	}
	if (decNumberIsInfinite(y)) {
		decNumberPIon2(at);
		if (yneg)
			dn_minus(at, at);
		return at;
	}

	dn_divide(&t, y, x);
	do_atan(&r, &t);
	if (xneg) {
		decNumberPI(&t);
		if (yneg)
			dn_minus(&t, &t);
	} else
		decNumberZero(&t);
	dn_add(at, &r, &t);
	if (dn_eq0(at) && yneg)
		dn_minus(at, at);
	return at;
}

decNumber *decNumberArcTan2(decNumber *res, const decNumber *a, const decNumber *b) {
	decNumber z;

	do_atan2(&z, a, b);
	cvt_rad2(res, &z);
	return res;	
}

void op_r2p(enum nilop op) {
	decNumber x, y, rx, ry;

	getXY(&x, &y);
	cmplxToPolar(&rx, &ry, &x, &y);
	cvt_rad2(&y, &ry);
	setlastX();
	setXY(&rx, &y);
#ifdef RP_PREFIX
	RectPolConv = 1;
#endif
}

void op_p2r(enum nilop op) {
	decNumber x, y, t, range, angle;

	getXY(&range, &angle);
	decNumberCos(&t, &angle);
	dn_multiply(&x, &t, &range);
	decNumberSin(&t, &angle);
	dn_multiply(&y, &t, &range);
	setlastX();
	setXY(&x, &y);
#ifdef RP_PREFIX
	RectPolConv = 2;
#endif
}	


/* Hyperbolic functions.
 * We start with a utility routine that calculates sinh and cosh.
 * We do the sihn as (e^x - 1) (e^x + 1) / (2 e^x) for numerical stability
 * reasons if the value of x is smallish.
 */
void dn_sinhcosh(const decNumber *x, decNumber *sinhv, decNumber *coshv) {
	decNumber t, u, v;

	if (sinhv != NULL) {
		if (dn_abs_lt(x, &const_0_5)) {
			decNumberExpm1(&u, x);
			dn_div2(&t, &u);
			dn_inc(&u);
			dn_divide(&v, &t, &u);
			dn_inc(&u);
			dn_multiply(sinhv, &u, &v);
		} else {
			dn_exp(&u, x);			// u = e^x
			decNumberRecip(&v, &u);		// v = e^-x
			dn_subtract(&t, &u, &v);	// r = e^x - e^-x
			dn_div2(sinhv, &t);
		}
	}
	if (coshv != NULL) {
		dn_exp(&u, x);			// u = e^x
		decNumberRecip(&v, &u);		// v = e^-x
		dn_average(coshv, &v, &u);	// r = (e^x + e^-x)/2
	}
}

decNumber *decNumberSinh(decNumber *res, const decNumber *x) {
	if (decNumberIsSpecial(x)) {
		if (decNumberIsNaN(x))
			return set_NaN(res);
		return decNumberCopy(res, x);
	}
	dn_sinhcosh(x, res, NULL);
	return res;
}

decNumber *decNumberCosh(decNumber *res, const decNumber *x) {
	if (decNumberIsSpecial(x)) {
		if (decNumberIsNaN(x))
			return set_NaN(res);
		return set_inf(res);
	}
	dn_sinhcosh(x, NULL, res);
	return res;
}

decNumber *decNumberTanh(decNumber *res, const decNumber *x) {
	decNumber a, b;

	if (decNumberIsNaN(x))
		return set_NaN(res);
	if (!dn_abs_lt(x, &const_100)) {
		if (decNumberIsNegative(x))
			return dn__1(res);
		return dn_1(res);
	}
	dn_add(&a, x, x);
	decNumberExpm1(&b, &a);
	dn_p2(&a, &b);
	return dn_divide(res, &b, &a);
}


#if 0
decNumber *decNumberSech(decNumber *res, const decNumber *x) {
	return dn_recip(res, x, &decNumberCosh);
}

decNumber *decNumberCosech(decNumber *res, const decNumber *x) {
	return dn_recip(res, x, &decNumberSinh);
}

decNumber *decNumberCoth(decNumber *res, const decNumber *x) {
	return dn_recip(res, x, &decNumberTanh);
}
#endif

decNumber *decNumberArcSinh(decNumber *res, const decNumber *x) {
	decNumber y, z;

	decNumberSquare(&y, x);		// y = x^2
	dn_p1(&z, &y);			// z = x^2 + 1
	dn_sqrt(&y, &z);		// y = sqrt(x^2+1)
	dn_inc(&y);			// y = sqrt(x^2+1)+1
	dn_divide(&z, x, &y);
	dn_inc(&z);
	dn_multiply(&y, x, &z);
	return decNumberLn1p(res, &y);
}


decNumber *decNumberArcCosh(decNumber *res, const decNumber *x) {
	decNumber z;

	decNumberSquare(res, x);	// r = x^2
	dn_m1(&z, res);			// z = x^2 + 1
	dn_sqrt(res, &z);		// r = sqrt(x^2+1)
	dn_add(&z, res, x);		// z = x + sqrt(x^2+1)
	return dn_ln(res, &z);
}

decNumber *decNumberArcTanh(decNumber *res, const decNumber *x) {
	decNumber y, z;

	if (decNumberIsNaN(x))
		return set_NaN(res);
	dn_abs(&y, x);
	if (dn_eq1(&y)) {
		if (decNumberIsNegative(x))
			return set_neginf(res);
		return set_inf(res);
	}
	// Not the obvious formula but more stable...
	dn_1m(&z, x);
	dn_divide(&y, x, &z);
	dn_mul2(&z, &y);
	decNumberLn1p(&y, &z);
	return dn_div2(res, &y);
}


decNumber *decNumberD2R(decNumber *res, const decNumber *x) {
	return dn_multiply(res, x, &const_PIon180);
}

decNumber *decNumberR2D(decNumber *res, const decNumber *x) {
	return dn_divide(res, x, &const_PIon180);
}


decNumber *decNumberG2R(decNumber *res, const decNumber *x) {
	return dn_multiply(res, x, &const_PIon200);
}

decNumber *decNumberR2G(decNumber *res, const decNumber *x) {
	return dn_divide(res, x, &const_PIon200);
}

decNumber *decNumberG2D(decNumber *res, const decNumber *x) {
	return dn_multiply(res, x, &const_0_9);
}

decNumber *decNumberD2G(decNumber *res, const decNumber *x) {
	return dn_divide(res, x, &const_0_9);
}

decNumber *decNumberDRG(decNumber *res, const decNumber *x) {
	return decNumberDRG_internal(res, x, XeqOpCode);
}

/* Check the arguments a little and perform the computation of
 * ln(permutation) which is common across both our callers.
 *
 * This is the real version.
 */
enum perm_opts { PERM_INVALID=0, PERM_INTG, PERM_NORMAL };
static enum perm_opts perm_helper(decNumber *r, const decNumber *x, const decNumber *y) {
	decNumber n, s;

	if (decNumberIsSpecial(x) || decNumberIsSpecial(y) || dn_lt0(x) || dn_lt0(y)) {
		if (decNumberIsInfinite(x) && !decNumberIsInfinite(y))
			set_inf(r);
		else
			set_NaN(r);
		return PERM_INVALID;
	}
	dn_p1(&n, x);				// x+1
	decNumberLnGamma(&s, &n);		// lnGamma(x+1) = Ln x!

	dn_subtract(r, &n, y);	// x-y+1
	if (dn_le0(r)) {
		set_NaN(r);
		return PERM_INVALID;
	}
	decNumberLnGamma(&n, r);		// LnGamma(x-y+1) = Ln (x-y)!
	dn_subtract(r, &s, &n);

	if (is_int(x) && is_int(y))
		return PERM_INTG;
	return PERM_NORMAL;
}


/* Calculate permutations:
 * C(x, y) = P(x, y) / y! = x! / ( (x-y)! y! )
 */
decNumber *decNumberComb(decNumber *res, const decNumber *x, const decNumber *y) {
	decNumber r, n, s;
	const enum perm_opts code = perm_helper(&r, x, y);

	if (code != PERM_INVALID) {
		dn_p1(&n, y);				// y+1
		decNumberLnGamma(&s, &n);		// LnGamma(y+1) = Ln y!
		dn_subtract(&n, &r, &s);

		dn_exp(res, &n);
		if (code == PERM_INTG)
			decNumberIntg(res, res);
	} else
		decNumberCopy(res, &r);
	return res;
}

/* Calculate permutations:
 * P(x, y) = x! / (x-y)!
 */
decNumber *decNumberPerm(decNumber *res, const decNumber *x, const decNumber *y) {
	decNumber t;
	const enum perm_opts code = perm_helper(&t, x, y);

	if (code != PERM_INVALID) {
		dn_exp(res, &t);
		if (code == PERM_INTG)
			decNumberIntg(res, res);
	} else
		decNumberCopy(res, &t);
	return res;
}

#ifdef _DEBUG_
#include <stdio.h>
char dump[DECNUMDIGITS + 10];
#define DUMP(d, s) ((int) decNumberToString(d, dump), fprintf(f, s "=%s\n", dump))

#endif

const decNumber *const gamma_consts[21] = {
	&const_gammaC01, &const_gammaC02, &const_gammaC03,
	&const_gammaC04, &const_gammaC05, &const_gammaC06,
	&const_gammaC07, &const_gammaC08, &const_gammaC09,
	&const_gammaC10, &const_gammaC11, &const_gammaC12,
	&const_gammaC13, &const_gammaC14, &const_gammaC15,
	&const_gammaC16, &const_gammaC17, &const_gammaC18,
	&const_gammaC19, &const_gammaC20, &const_gammaC21,
};

static void dn_LnGamma(decNumber *res, const decNumber *x) {
	decNumber r, s, t, u, v;
	int k;
#ifdef DUMP
	FILE *f = fopen("calc.out","a");
	DUMP(x, "z");
#endif
	decNumberZero(&s);
	dn_add(&t, x, &const_21);
	for (k = 20; k >= 0; k--) {
		dn_divide(&u, gamma_consts[k], &t);
		dn_dec(&t);
		dn_add(&s, &s, &u);
	}
	dn_add(&t, &s, &const_gammaC00);
	dn_ln(&s, &t);
#ifdef DUMP
	DUMP(&t, "sum");
	DUMP(&s, "ln");
#endif
//		r = z + g + .5;
	dn_add(&r, x, &const_gammaR);
#ifdef DUMP
	DUMP(&r, "r");
#endif

//		r = log(R[0][0]) + (z+.5) * log(r) - r;
	dn_ln(&u, &r);
	dn_add(&t, x, &const_0_5);
	dn_multiply(&v, &u, &t);
#ifdef DUMP
	DUMP(&v, "(z+.5)*log(r)");
#endif

	dn_subtract(&u, &v, &r);
	dn_add(res, &u, &s);

#ifdef DUMP
	DUMP(res, "res");
	fclose(f);
#endif
}

decNumber *decNumberFactorial(decNumber *res, const decNumber *xin) {
	decNumber x;

	dn_p1(&x, xin);
	return decNumberGamma(res, &x);
}

decNumber *decNumberGamma(decNumber *res, const decNumber *xin) {
	decNumber x, s, t, u;
	int reflec = 0;

	// Check for special cases
	if (decNumberIsSpecial(xin)) {
		if (decNumberIsInfinite(xin) && !decNumberIsNegative(xin))
			return set_inf(res);
		return set_NaN(res);
	}

	// Correct our argument and begin the inversion if it is negative
	if (dn_le0(xin)) {
		reflec = 1;
		dn_1m(&t, xin);
		if (is_int(&t)) {
			return set_NaN(res);
		}
		dn_m1(&x, &t);
	} else {
		dn_m1(&x, xin);
#ifdef GAMMA_FAST_INTEGERS
		// Provide a fast path evaluation for positive integer arguments that aren't too large
		// The threshold for overflow is 205! (i.e. 204! is within range and 205! isn't).
		// Without introducing a new constant, we've got 150 or 256 to choose from.
		if (is_int(&x) && ! dn_eq0(xin) && dn_lt(&x, &const_256)) {
			dn_1(res);
			while (! dn_eq0(&x)) {
				dn_multiply(res, res, &x);
				dn_m1(&x, &x);
			}
			return res;
		}
#endif
	}

	dn_LnGamma(&t, &x);
	dn_exp(res, &t);

	// Finally invert if we started with a negative argument
	if (reflec) {
		// figure out xin * PI mod 2PI
		decNumberMod(&s, xin, &const_2);
		dn_mulPI(&t, &s);
		sincosTaylor(&t, &s, &u);
		dn_multiply(&u, &s, res);
		dn_divide(res, &const_PI, &u);
	}
	return res;
}

// The log gamma function.
decNumber *decNumberLnGamma(decNumber *res, const decNumber *xin) {
	decNumber x, s, t, u;
	int reflec = 0;

	// Check for special cases
	if (decNumberIsSpecial(xin)) {
		if (decNumberIsInfinite(xin) && !decNumberIsNegative(xin))
			return set_inf(res);
		return set_NaN(res);
	}

	// Correct out argument and begin the inversion if it is negative
	if (dn_le0(xin)) {
		reflec = 1;
		dn_1m(&t, xin);
		if (is_int(&t)) {
			return set_NaN(res);
		}
		dn_m1(&x, &t);
	} else
		dn_m1(&x, xin);

	dn_LnGamma(res, &x);

	// Finally invert if we started with a negative argument
	if (reflec) {
		// Figure out S * PI mod 2PI
		decNumberMod(&u, &s, &const_2);
		dn_mulPI(&t, &u);
		sincosTaylor(&t, &s, &u);
		dn_divide(&u, &const_PI, &s);
		dn_ln(&t, &u);
		dn_subtract(res, &t, res);
	}
	return res;
}

// lnBeta(x, y) = lngamma(x) + lngamma(y) - lngamma(x+y)
decNumber *decNumberLnBeta(decNumber *res, const decNumber *x, const decNumber *y) {
	decNumber s, t, u;

	decNumberLnGamma(&s, x);
	busy();
	decNumberLnGamma(&t, y);
	busy();
	dn_add(&u, &s, &t);
	dn_add(&s, x, y);
	decNumberLnGamma(&t, &s);
	dn_subtract(res, &u, &t);
	return res;
}

decNumber *decNumberHMS2HR(decNumber *res, const decNumber *x) {
	decNumber m, s, t;

	// decode hhhh.mmss...
	decNumberFrac(&t, x);			// t = .mmss
	dn_mul100(&s, &t);			// s = mm.ss
	decNumberTrunc(&m, &s);			// m = mm
	decNumberFrac(&t, &s);			// t = .ss
	dn_multiply(&s, &t, &const_1on60);	// s = ss.sss / 60
	dn_mulpow10(&s, &s, 2);
	dn_add(&t, &m, &s);			// s = mm + ss.sss / 60
	dn_multiply(&m, &t, &const_1on60);
	decNumberTrunc(&s, x);			// s = hh
	dn_add(res, &m, &s);
	return res;
}

decNumber *decNumberHR2HMS(decNumber *res, const decNumber *x) {
	decNumber m, s, t;

	decNumberFrac(&t, x);			// t = .mmssss
	dn_multiply(&s, &t, &const_60);		// s = mm.ssss
	decNumberTrunc(&m, &s);			// m = mm
	decNumberFrac(&t, &s);			// t = .ssss
	dn_multiply(&s, &t, &const_0_6);	// scale down by 60/100
	dn_add(&t, &s, &m);			// t = mm.ss
	dn_mulpow10(&m, &t, -2);		// t = .mmss
	decNumberTrunc(&s, x);			// s = hh
	dn_add(&t, &m, &s);			// t = hh.mmss = result

	// Round to the appropriate number of digits for the result
	decNumberRoundDigits(&t, &t, is_dblmode() ? 34 : 16, DEC_ROUND_HALF_EVEN);

	// Now fix any rounding/carry issues
	dn_mulpow10(&s, &t, 2);			// hhmm.ssss
	decNumberFrac(&m, &s);			// .ssss
	if (dn_ge(&m, &const_0_6))
		dn_add(&s, &s, &const_0_4);
	dn_mulpow10(res, &s, -2);		// hh.mmssss
	decNumberFrac(&m, res);
	if (dn_ge(&m, &const_0_6))
		dn_add(res, res, &const_0_4);
	return res;
}

decNumber *decNumberHMSAdd(decNumber *res, const decNumber *x, const decNumber *y) {
	decNumber a, b, c;

	decNumberHMS2HR(&a, x);
	decNumberHMS2HR(&b, y);
	dn_add(&c, &a, &b);
	decNumberHR2HMS(res, &c);
	return res;
}

decNumber *decNumberHMSSub(decNumber *res, const decNumber *x, const decNumber *y) {
	decNumber a, b, c;

	decNumberHMS2HR(&a, x);
	decNumberHMS2HR(&b, y);
	dn_subtract(&c, &a, &b);
	decNumberHR2HMS(res, &c);
	return res;
}

/* Logical operations on decNumbers.
 * We treat 0 as false and non-zero as true.
 */
static int dn2bool(const decNumber *x) {
	return dn_eq0(x)?0:1;
}

static decNumber *bool2dn(decNumber *res, int l) {
	if (l)
		dn_1(res);
	else
		decNumberZero(res);
	return res;
}

decNumber *decNumberNot(decNumber *res, const decNumber *x) {
	return bool2dn(res, !dn2bool(x));
}

/*
 *  Execute the logical operations via a truth table.
 *  Each nibble encodes a single operation.
 */
decNumber *decNumberBooleanOp(decNumber *res, const decNumber *x, const decNumber *y) {
	const unsigned int TRUTH_TABLE = 0x9176E8;
	const unsigned int bit = ((argKIND(XeqOpCode) - OP_LAND) << 2) + (dn2bool(y) << 1) + dn2bool(x);
	return bool2dn(res, (TRUTH_TABLE >> bit) & 1);
}


/*
 * Round a number to a given number of digits and with a given mode
 */
decNumber *decNumberRoundDigits(decNumber *res, const decNumber *x, const int digits, const enum rounding round) {
	enum rounding default_round = Ctx.round;
	int default_digits = Ctx.digits;

	Ctx.round = round;
	Ctx.digits = digits;
	decNumberPlus(res, x, &Ctx);
	Ctx.digits = default_digits;
	Ctx.round = default_round;
	return res;
}


/*
 *  Round to display accuracy
 */
decNumber *decNumberRnd(decNumber *res, const decNumber *x) {
	int numdig = UState.dispdigs + 1;
	decNumber t, u;
	enum display_modes dmode = (enum display_modes) UState.dispmode;

	if (decNumberIsSpecial(x))
		return decNumberCopy(res, x);

	if (UState.fract) {
		decNumber2Fraction(&t, &u, x);
		return dn_divide(res, &t, &u);
	}

#if defined(INCLUDE_SIGFIG_MODE)	
 	if (dmode == MODE_STD) {
		dmode = std_round_fix(x, &numdig); // to fit new definition of std_round_fix in display.c
 	}
#else	
	if (dmode == MODE_STD) {
		dmode = std_round_fix(x);
		numdig = DISPLAY_DIGITS;
	}
#endif
	
	if (dmode == MODE_FIX)
		/* FIX is different since the number of digits changes */
		return decNumberRoundDecimals(res, x, numdig-1, DEC_ROUND_HALF_UP);

	return decNumberRoundDigits(res, x, numdig, DEC_ROUND_HALF_UP); 
}

decNumber *decNumberRoundDecimals(decNumber *r, const decNumber *x, const int n, const enum rounding round) {
	decNumber t;
#if 0
	/* The slow but always correct way */
	decNumber u, p10;

	int_to_dn(&u, n);
	decNumberPow10(&p10, &u);
	dn_multiply(&t, x, &p10);
	round2int(&u, &t, round);
	return dn_divide(r, &u, &p10);
#else
	/* The much faster way but relying on base 10 numbers with exponents */
	if (decNumberIsSpecial(x))
		return decNumberCopy(r, x);

	decNumberCopy(&t, x);
	t.exponent += n;
	round2int(r, &t, round);
	r->exponent -= n;
	return r;
#endif
}

void decNumber2Fraction(decNumber *n, decNumber *d, const decNumber *x) {
	decNumber z, dold, t, s, maxd;
	int neg;
	enum denom_modes dm;
	int i;

	if (decNumberIsNaN(x)) {
		cmplx_NaN(n, d);
		return;
	}
	if (decNumberIsInfinite(x)) {
		decNumberZero(d);
		if (decNumberIsNegative(x))
			dn__1(n);
		else
			dn_1(n);
		return;
	}

	dm = UState.denom_mode;
	get_maxdenom(&maxd);

	neg = decNumberIsNegative(x);
	if (neg)
		dn_minus(&z, x);
	else
		decNumberCopy(&z, x);
	if (dm == DENOM_ANY) {
		decNumberZero(&dold);
		dn_1(d);
		/* Do a partial fraction expansion until the denominator is too large */
		for (i=0; i<1000; i++) {
			decNumberTrunc(&t, &z);
			dn_subtract(&s, &z, &t);
			if (dn_eq0(&s))
				break;
			decNumberRecip(&z, &s);
			decNumberTrunc(&s, &z);
			dn_multiply(&t, &s, d);
			dn_add(&s, &t, &dold);	// s is new denominator estimate
			if (dn_lt(&maxd, &s))
				break;
			decNumberCopy(&dold, d);
			decNumberCopy(d, &s);
		}
	} else
		decNumberCopy(d, &maxd);
	dn_multiply(&t, x, d);
	decNumberRound(n, &t);
	if (dm == DENOM_FACTOR) {
		if (dn_eq0(n))
			dn_1(d);
		else {
			dn_gcd(&t, n, d, 0);
			dn_divide(n, n, &t);
			dn_divide(d, d, &t);
		}
	}
}

static decNumber *gser(decNumber *res, const decNumber *a, const decNumber *x, const decNumber *gln) {
	decNumber ap, del, sum, t, u;
	int i;

	if (dn_le0(x))
		return decNumberZero(res);
	decNumberCopy(&ap, a);
	decNumberRecip(&sum, a);
	decNumberCopy(&del, &sum);
	for (i=0; i<1000; i++) {
		dn_inc(&ap);
		dn_divide(&t, x, &ap);
		dn_multiply(&del, &del, &t);
		dn_add(&t, &sum, &del);
		if (dn_eq(&t, &sum))
			break;
		decNumberCopy(&sum, &t);
	}
	dn_ln(&t, x);
	dn_multiply(&u, &t, a);
	dn_subtract(&t, &u, x);
	dn_subtract(&u, &t, gln);
	dn_exp(&t, &u);
	return dn_multiply(res, &sum, &t);
}

static void gcheckSmall(decNumber *v)
{
	const decNumber * const threshold = &const_1e_10000;

	if (dn_abs_lt(v, threshold))
		decNumberCopy(v, threshold);
}

static decNumber *gcf(decNumber *res, const decNumber *a, const decNumber *x, const decNumber *gln) {
	decNumber an, b, c, d, h, t, u, v, i;
	int n;

	dn_p1(&t, x);
	dn_subtract(&b, &t, a);			// b = (x-1) a
	set_inf(&c);
	decNumberRecip(&d, &b);
	decNumberCopy(&h, &d);
	decNumberZero(&i);
	for (n=0; n<1000; n++) {
		dn_inc(&i);
		dn_subtract(&t, a, &i);		// t = a-i
		dn_multiply(&an, &i, &t);		// an = -i (i-a)
		dn_p2(&b, &b);
		dn_multiply(&t, &an, &d);
		dn_add(&v, &t, &b);
		gcheckSmall(&v);
		decNumberRecip(&d, &v);
		dn_divide(&t, &an, &c);
		dn_add(&c, &b, &t);
		gcheckSmall(&c);
		dn_multiply(&t, &d, &c);
		dn_multiply(&u, &h, &t);
		if (dn_eq(&h, &u))
			break;
		decNumberCopy(&h, &u);
	}
	dn_ln(&t, x);
	dn_multiply(&u, &t, a);
	dn_subtract(&t, &u, x);
	dn_subtract(&u, &t, gln);
	dn_exp(&t, &u);
	return dn_multiply(res, &t, &h);
}

decNumber *decNumberGammap(decNumber *res, const decNumber *x, const decNumber *a) {
	decNumber z, lga;
	const int op = XeqOpCode - (OP_DYA | OP_GAMMAg);
	const int regularised = op & 2;
	const int upper = op & 1;

	if (decNumberIsNegative(x) || dn_le0(a) ||
			decNumberIsNaN(x) || decNumberIsNaN(a) || decNumberIsInfinite(a)) {
		return set_NaN(res);
	}
	if (decNumberIsInfinite(x)) {
		if (upper) {
			if (regularised)
				return dn_1(res);
			return decNumberGamma(res, a);
		}
		return decNumberZero(res);
	}

	dn_p1(&lga, a);
	dn_compare(&z, x, &lga);
	if (regularised)
		decNumberLnGamma(&lga, a);
	else
		decNumberZero(&lga);
	if (decNumberIsNegative(&z)) {
		/* Deal with a difficult case by using the other expansion */
		if (dn_gt(a, &const_9000) && dn_gt(x, dn_multiply(&z, a, &const_0_995)))
			goto use_cf;
		gser(res, a, x, &lga);
		if (upper)
			goto invert;
	} else {
use_cf:		gcf(res, a, x, &lga);
		if (! upper)
			goto invert;
	}
	return res;

invert:	if (regularised)
		return dn_1m(res, res);
	decNumberGamma(&z, a);
	return dn_subtract(res, &z, res);
}

#ifdef INCLUDE_FACTOR
decNumber *decFactor(decNumber *r, const decNumber *x) {
	int sgn;
	unsigned long long int i;

	if (decNumberIsSpecial(x) || ! is_int(x))
		return set_NaN(r);
	i = dn_to_ull(x, &sgn);
	ullint_to_dn(r, doFactor(i));
	if (sgn)
		dn_minus(r, r);
	return r;
}
#endif

#ifdef INCLUDE_USER_IO
decNumber *decRecv(decNumber *r, const decNumber *x) {
	int to;

	if (decNumberIsSpecial(x) || decNumberIsNegative(x)) {
		to = -1;
		if (decNumberIsInfinite(x) && ! decNumberIsNegative(x))
			to = 0x7fffffff;
	} else
		to = dn_to_int(x);
	int_to_dn(r, recv_byte(to));
	return r;
}
#endif

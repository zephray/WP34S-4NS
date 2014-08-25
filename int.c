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

#include "int.h"
#include "xeq.h"
#include "serial.h"

static int check_intmode(void) {
	if (! is_intmode())
		return err(ERR_BAD_MODE);
	return 0;
}

/* Some utility routines to extract bits of long longs */

unsigned int int_base(void) {
	const unsigned int b = UState.int_base + 1;
	if (b < 2)
		return 10;
	return b;
}

enum arithmetic_modes int_mode(void) {
	return (enum arithmetic_modes) UState.int_mode;
}

unsigned int word_size(void) {
	unsigned int il = UState.int_len;
	if (il >= MAX_WORD_SIZE || il == 0)
		return MAX_WORD_SIZE;
	return il;
}

int get_carry(void) {
	return get_user_flag(CARRY_FLAG);
}

void set_carry(int c) {
#ifndef TINY_BUILD
	if (c)
		set_user_flag(CARRY_FLAG);
	else
		clr_user_flag(CARRY_FLAG);
#endif
}

int get_overflow(void) {
	return get_user_flag(OVERFLOW_FLAG);
}

void set_overflow(int o) {
#ifndef TINY_BUILD
	if (o)
		set_user_flag(OVERFLOW_FLAG);
	else
		clr_user_flag(OVERFLOW_FLAG);
#endif
}

#ifndef TINY_BUILD
/* Some utility routines for saving and restoring carry and overflow.
 * Some operations don't change these flags but their subcomponents might.
 */
static int save_flags(void) {
	return (get_overflow() << 1) | get_carry();
}

static void restore_flags(int co) {
	set_carry(co & 1);
	set_overflow(co & 2);
}
#endif

/* Utility routine for trimming a value to the current word size
 */
long long int mask_value(const long long int v) {
#ifndef TINY_BUILD
	const unsigned int ws = word_size();
	long long int mask;

	if (MAX_WORD_SIZE == 64 && ws == 64)
		return v;
	mask = (1LL << ws) - 1;
	return v & mask;
#else
	return v;
#endif
}

#ifndef TINY_BUILD
/* Ulility routine for returning a bit mask to get the topmost (sign)
 * bit from a number.
 */
static long long int topbit_mask(void) {
	const unsigned int ws = word_size();
	long long int bit = 1LL << (ws - 1);
	return bit;
}
#endif

/* Utility routine to convert a binary integer into separate sign and
 * value components.  The sign returned is 1 for negative and 0 for positive.
 */
unsigned long long int extract_value(const long long int val, int *const sign) {
	const enum arithmetic_modes mode = int_mode();
	long long int v = mask_value(val);
	long long int tbm;

	if (mode == MODE_UNSIGNED) {
		*sign = 0;
		return v;
	}

	tbm = topbit_mask();

	if (v & tbm) {
		*sign = 1;
		if (mode == MODE_2COMP)
			v = -v;
		else if (mode == MODE_1COMP)
			v = ~v;
		else // if (mode == MODE_SGNMANT)
			v ^= tbm;
	} else
		*sign = 0;
    return mask_value(v);
}

/* Helper routine to construct a value from the magnitude and sign
 */
long long int build_value(const unsigned long long int x, const int sign) {
#ifndef TINY_BUILD
	const enum arithmetic_modes mode = int_mode();
	long long int v = mask_value(x);

	if (sign == 0 || mode == MODE_UNSIGNED)
		return v;

	if (mode == MODE_2COMP)
		return mask_value(-(signed long long int)v);
	if (mode == MODE_1COMP)
		return mask_value(~v);
	return v | topbit_mask();
#else
	return x;
#endif
}


/* Utility routine to check if a value has overflowed or not */
int check_overflow(long long int x) {
	return mask_value(x) != x ||
		(int_mode() != MODE_UNSIGNED && (x & topbit_mask()) != 0);
}


#ifndef TINY_BUILD
/* Helper routine for addition and subtraction that detemines the proper
 * setting for the overflow bit.  This routine should only be called when
 * the signs of the operands are the same for addition and different
 * for subtraction.  Overflow isn't possible if the signs are opposite.
 * The arguments of the operator should be passed in after conversion
 * to positive unsigned quantities nominally in two's complement.
 */
static int calc_overflow(unsigned long long int xv,
		unsigned long long int yv, enum arithmetic_modes mode, int neg) {
	unsigned long long int tbm = topbit_mask();
	unsigned long long int u;
	int i;

	switch (mode) {
	case MODE_UNSIGNED:
		// C doesn't expose the processor's status bits to us so we
		// break the addition down so we don't lose the overflow.
		u = (yv & (tbm-1)) + (xv & (tbm-1));
		i = ((u & tbm)?1:0) + ((xv & tbm)?1:0) + ((yv & tbm)?1:0);
		if (i > 1)
			break;
		return 0;

	case MODE_2COMP:
		u = xv + yv;
		if (neg && u == tbm)
			return 0;
		if (tbm & u)
			break;
		if ((xv == tbm && yv !=0) || (yv == tbm && xv != 0))
			break;
		return 0;

	case MODE_SGNMANT:
	case MODE_1COMP:
		if (tbm & (xv + yv))
			break;
		return 0;
	}
	set_overflow(1);
	return 1;
}
#endif


long long int intAdd(long long int y, long long int x) {
#ifndef TINY_BUILD
	int sx, sy;
	unsigned long long int xv = extract_value(x, &sx);
	unsigned long long int yv = extract_value(y, &sy);
	const enum arithmetic_modes mode = int_mode();
	long long int v;
	int overflow;

	set_overflow(0);
	if (sx == sy)
		overflow = calc_overflow(xv, yv, mode, sx);
	else
		overflow = 0;

	if (mode == MODE_SGNMANT) {
		const long long int tbm = topbit_mask();
		const long long int x2 = (x & tbm)?-(x ^ tbm):x;
		const long long int y2 = (y & tbm)?-(y ^ tbm):y;

		set_carry(overflow);

		v = y2 + x2;
		if (v & tbm)
			v = -v | tbm;
	} else {
		int carry;
		const unsigned long long int u = mask_value(y + x);

		if (u < (unsigned long long int)mask_value(y)) {
			set_carry(1);
			carry = 1;
		} else {
			set_carry(0);
			carry = 0;
		}

		v = y + x;
		if (carry && mode == MODE_1COMP)
			v++;
	}
	return mask_value(v);
#else
	return y+x;
#endif
}

long long int intSubtract(long long int y, long long int x) {
#ifndef TINY_BUILD
	int sx, sy;
	unsigned long long int xv = extract_value(x, &sx);
	unsigned long long int yv = extract_value(y, &sy);
	const enum arithmetic_modes mode = int_mode();
	long long int v, tbm;

	set_overflow(0);
	if (sx != sy)
		calc_overflow(xv, yv, mode, sy);

	if (mode == MODE_SGNMANT) {
		long long int x2, y2;
		set_carry((sx == 0 && sy == 0 && xv > yv) ||
				(sx != 0 && sy != 0 && xv < yv));

		tbm = topbit_mask();
		x2 = (x & tbm)?-(x ^ tbm):x;
		y2 = (y & tbm)?-(y ^ tbm):y;

		v = y2 - x2;
		if (v & tbm)
			v = -v | tbm;
	} else {
		int borrow;

		if ((unsigned long long int)y < (unsigned long long int)x) {
			set_carry(1);
			if (mode == MODE_UNSIGNED)
				set_overflow(1);
			borrow = 1;
		} else {
			set_carry(0);
			borrow = 0;
		}

		v = y - x;
		if (borrow && mode == MODE_1COMP)
			v--;
	}
	return mask_value(v);
#else
	return y-x;
#endif
}

static unsigned long long int multiply_with_overflow(unsigned long long int x, unsigned long long int y, int *overflow) {
	const unsigned long long int t = mask_value(x * y);

	if (! *overflow && y != 0) {
		const enum arithmetic_modes mode = int_mode();
		const unsigned long long int tbm = (mode == MODE_UNSIGNED) ? 0 : topbit_mask();

		if ((t & tbm) != 0 || t / y != x)
			*overflow = 1;
	}
	return t;
}

long long int intMultiply(long long int y, long long int x) {
#ifndef TINY_BUILD
	unsigned long long int u;
	int sx, sy;
	unsigned long long int xv = extract_value(x, &sx);
	unsigned long long int yv = extract_value(y, &sy);
	int overflow = 0;

	u = multiply_with_overflow(xv, yv, &overflow);
	set_overflow(overflow);

	if (int_mode() == MODE_UNSIGNED)
		return u;
	return build_value(u & ~topbit_mask(), sx ^ sy);
#else
	return x*y;
#endif
}

#ifndef TINY_BUILD
static void err_div0(unsigned long long int num, int sn, int sd) {
	if (num == 0)
		err(ERR_DOMAIN);
	else if (sn == sd)
		err(ERR_INFINITY);
	else
		err(ERR_MINFINITY);
}
#endif

long long int intDivide(long long int y, long long int x) {
#ifndef TINY_BUILD
	const enum arithmetic_modes mode = int_mode();
	int sx, sy;
	unsigned long long int xv = extract_value(x, &sx);
	unsigned long long int yv = extract_value(y, &sy);
	unsigned long long int r;
	long long int tbm;

	if (xv == 0) {
		err_div0(yv, sy, sx);
		return 0;
	}
	set_overflow(0);
	r = mask_value(yv / xv);
	// Set carry if there is a remainder
	set_carry(r * xv != yv);

	if (mode != MODE_UNSIGNED) {
		tbm = topbit_mask();
		if (r & tbm)
			set_carry(1);
		// Special case for 0x8000...00 / -1 in 2's complement
		if (mode == MODE_2COMP && sx && xv == 1 && y == tbm)
			set_overflow(1);
	}
	return build_value(r, sx ^ sy);
#else
	return y/x;
#endif
}

long long int intMod(long long int y, long long int x) {
#ifndef TINY_BUILD
	int sx, sy;
	unsigned long long int xv = extract_value(x, &sx);
	unsigned long long int yv = extract_value(y, &sy);
	unsigned long long int r;

	if (xv == 0) {
		err_div0(yv, sy, sx);
		return 0;
	}
	r = yv % xv;
#ifdef INCLUDE_MOD41
	if (XeqOpCode == (OP_DYA | OP_MOD41) && sx != sy) {
        if (r != 0)
		    r = xv - r;
		sy = sx;
	}
#endif
	return build_value(r, sy);
#else
	return y%x;
#endif
}


long long int intMin(long long int y, long long int x) {
#ifndef TINY_BUILD
	int sx, sy;
	const unsigned long long int xv = extract_value(x, &sx);
	const unsigned long long int yv = extract_value(y, &sy);

	if (sx != sy) {			// different signs
		if (sx)
			return x;
	} else if (sx) {		// both negative
		if (xv > yv)
			return x;
	} else {			// both positive
		if (xv < yv)
			return x;
	}
	return y;
#else
	return 0;
#endif
}

long long int intMax(long long int y, long long int x) {
#ifndef TINY_BUILD
	int sx, sy;
	unsigned long long int xv = extract_value(x, &sx);
	unsigned long long int yv = extract_value(y, &sy);

	if (sx != sy) {			// different signs
		if (sx)
			return y;
	} else if (sx) {		// both negative
		if (xv > yv)
			return y;
	} else {			// both positive
		if (xv < yv)
			return y;
	}
	return x;
#else
	return 0;
#endif
}


#ifdef INCLUDE_MULADD
long long int intMAdd(long long int z, long long int y, long long int x) {
#ifndef TINY_BUILD
	long long int t = intMultiply(x, y);
	const int of = get_overflow();

	t = intAdd(t, z);
	if (of)
		set_overflow(1);
	return t;
#else
	return 0;
#endif
}
#endif


static unsigned long long int int_gcd(unsigned long long int a, unsigned long long int b) {
	while (b != 0) {
		const unsigned long long int t = b;
		b = a % b;
		a = t;
	}
	return a;
}

long long int intGCD(long long int y, long long int x) {
	int s;
	unsigned long long int xv = extract_value(x, &s);
	unsigned long long int yv = extract_value(y, &s);
	unsigned long long int v;

	if (xv == 0)
		v = yv;
	else if (yv == 0)
		v = xv;
	else
		v = int_gcd(xv, yv);
	return build_value(v, 0);
}

long long int intLCM(long long int y, long long int x) {
	int s;
	unsigned long long int xv = extract_value(x, &s);
	unsigned long long int yv = extract_value(y, &s);
	unsigned long long int gcd;

	if (xv == 0 || yv == 0)
		return 0;
	gcd = int_gcd(xv, yv);
	return intMultiply(mask_value(xv / gcd), build_value(yv, 0));
}

long long int intSqr(long long int x) {
	return intMultiply(x, x);
}

long long int intCube(long long int x) {
#ifndef TINY_BUILD
	long long int y = intMultiply(x, x);
	int overflow = get_overflow();

	y = intMultiply(x, y);
	if (overflow)
		set_overflow(1);
	return y;
#else
	return 0;
#endif
}

long long int intChs(long long int x) {
#ifndef TINY_BUILD
	const enum arithmetic_modes mode = int_mode();
	int sx;
	unsigned long long int xv = extract_value(x, &sx);

	if (mode == MODE_UNSIGNED || (mode == MODE_2COMP && x == topbit_mask())) {
		set_overflow(1);
		return mask_value(-(signed long long int)xv);
	}
	set_overflow(0);
	return build_value(xv, !sx);
#else
	return x;
#endif
}

long long int intAbs(long long int x) {
#ifndef TINY_BUILD
	int sx;
	unsigned long long int xv = extract_value(x, &sx);

	set_overflow(0);
	if (int_mode() == MODE_2COMP && x == topbit_mask()) {
		set_overflow(1);
		return x;
	}
	return build_value(xv, 0);
#else
	return x;
#endif
}

#ifndef TINY_BUILD
static void breakup(unsigned long long int x, unsigned short xv[4]) {
	xv[0] = x & 0xffff;
	xv[1] = (x >> 16) & 0xffff;
	xv[2] = (x >> 32) & 0xffff;
	xv[3] = (x >> 48) & 0xffff;
}

static unsigned long long int packup(unsigned short int x[4]) {
	return (((unsigned long long int)x[3]) << 48) |
			(((unsigned long long int)x[2]) << 32) |
			(((unsigned long int)x[1]) << 16) |
			x[0];
}
#endif

void intDblMul(enum nilop op) {
#ifndef TINY_BUILD
	const enum arithmetic_modes mode = int_mode();
	unsigned long long int xv, yv;
	int s;	
	unsigned short int xa[4], ya[4];
	unsigned int t[8];
	unsigned short int r[8];
	int i, j;

	{
		long long int xr, yr;
		int sx, sy;

		xr = getX_int();
		yr = get_reg_n_int(regY_idx);

		xv = extract_value(xr, &sx);
		yv = extract_value(yr, &sy);

		s = sx != sy;
	}

	/* Do the multiplication by breaking the values into unsigned shorts
	 * multiplying them all out and accumulating into unsigned ints.
	 * Then perform a second pass over the ints to propogate carry.
	 * Finally, repack into unsigned long long ints.
	 *
	 * This isn't terribly efficient especially for shorter word
	 * sizes but it works.  Special cases for WS <= 16 and/or WS <= 32
	 * might be worthwhile since the CPU supports these multiplications
	 * natively.
	 */
	breakup(xv, xa);
	breakup(yv, ya);

	for (i=0; i<8; i++)
		t[i] = 0;

	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			t[i+j] += xa[i] * ya[j];

	for (i=0; i<8; i++) {
		if (t[i] >= 65536)
			t[i+1] += t[i] >> 16;
		r[i] = t[i];
	}

	yv = packup(r);
	xv = packup(r+4);

	i = word_size();
	if (i != 64)
		xv = (xv << (64-i)) | (yv >> i);

	setlastX();

	if (s != 0) {
		if (mode == MODE_2COMP) {
			yv = mask_value(1 + ~yv);
			xv = ~xv;
			if (yv == 0)
				xv++;
		} else if (mode == MODE_1COMP) {
			yv = ~yv;
			xv = ~xv;
		} else
			xv |= topbit_mask();
	}

	set_reg_n_int(regY_idx, mask_value(yv));
	setX_int(mask_value(xv));
	set_overflow(0);
#endif
}


#ifndef TINY_BUILD
static int nlz(unsigned short int x) {
   int n;

   if (x == 0)
	   return 16;
   n = 0;
   if (x <= 0x00ff) {n = n + 8; x = x << 8;}
   if (x <= 0x0fff) {n = n + 4; x = x << 4;}
   if (x <= 0x3fff) {n = n + 2; x = x << 2;}
   if (x <= 0x7fff) {n = n + 1;}
   return n;
}

/* q[0], r[0], u[0], and v[0] contain the LEAST significant halfwords.
(The sequence is in little-endian order).

This first version is a fairly precise implementation of Knuth's
Algorithm D, for a binary computer with base b = 2**16.  The caller
supplies
   1. Space q for the quotient, m - n + 1 halfwords (at least one).
   2. Space r for the remainder (optional), n halfwords.
   3. The dividend u, m halfwords, m >= 1.
   4. The divisor v, n halfwords, n >= 2.
The most significant digit of the divisor, v[n-1], must be nonzero.  The
dividend u may have leading zeros; this just makes the algorithm take
longer and makes the quotient contain more leading zeros.  A value of
NULL may be given for the address of the remainder to signify that the
caller does not want the remainder.
   The program does not alter the input parameters u and v.
   The quotient and remainder returned may have leading zeros.
   For now, we must have m >= n.  Knuth's Algorithm D also requires
that the dividend be at least as long as the divisor.  (In his terms,
m >= 0 (unstated).  Therefore m+n >= n.) */

static void divmnu(unsigned short q[], unsigned short r[],
		const unsigned short u[], const unsigned short v[],
		const int m, const int n) {
	const unsigned int b = 65536;			// Number base (16 bits).
	unsigned qhat;					// Estimated quotient digit.
	unsigned rhat;					// A remainder.
	unsigned p;					// Product of two digits.
	int s, i, j, t, k;
	unsigned short vn[8];				// Normalised denominator
	unsigned short un[18];				// Normalised numerator

	if (n == 1) {					// Take care of
		k = 0;					// the case of a
		for (j = m - 1; j >= 0; j--) {		// single-digit
			q[j] = (k*b + u[j])/v[0];	// divisor here.
			k = (k*b + u[j]) - q[j]*v[0];
		}
		r[0] = k;
		return;
	}

	// Normalize by shifting v left just enough so that
	// its high-order bit is on, and shift u left the
	// same amount.  We may have to append a high-order
	// digit on the dividend; we do that unconditionally.

	s = nlz(v[n-1]);       				 // 0 <= s <= 16.
	for (i = n - 1; i > 0; i--)
		vn[i] = (v[i] << s) | (v[i-1] >> (16-s));
	vn[0] = v[0] << s;

	un[m] = u[m-1] >> (16-s);
	for (i = m - 1; i > 0; i--)
		un[i] = (u[i] << s) | (u[i-1] >> (16-s));
	un[0] = u[0] << s;

	for (j = m - n; j >= 0; j--) {       		// Main loop.
	// Compute estimate qhat of q[j].
	qhat = (un[j+n]*b + un[j+n-1])/vn[n-1];
	rhat = (un[j+n]*b + un[j+n-1]) - qhat*vn[n-1];
	again:
	if (qhat >= b || qhat*vn[n-2] > b*rhat + un[j+n-2]) {
		qhat = qhat - 1;
		rhat = rhat + vn[n-1];
		if (rhat < b) goto again;
	}

	// Multiply and subtract.
	k = 0;
	for (i = 0; i < n; i++) {
		p = qhat*vn[i];
		t = un[i+j] - k - (p & 0xFFFF);
		un[i+j] = t;
		k = (p >> 16) - (t >> 16);
	}
	t = un[j+n] - k;
	un[j+n] = t;

	q[j] = qhat;					// Store quotient digit.
	if (t < 0) {					// If we subtracted too
		q[j] = q[j] - 1;       			// much, add back.
		k = 0;
		for (i = 0; i < n; i++) {
			t = un[i+j] + vn[i] + k;
			un[i+j] = t;
			k = t >> 16;
		}
		un[j+n] = un[j+n] + k;
		}
	} // End j.
	// Unnormalize remainder
	for (i = 0; i < n; i++)
		r[i] = (un[i] >> s) | (un[i+1] << (16-s));
}

static unsigned long long int divmod(const long long int z, const long long int y,
		const long long int x, int *sx, int *sy, unsigned long long *rem) {
	const enum arithmetic_modes mode = int_mode();
	const unsigned int ws = word_size();
	const long long int tbm = topbit_mask();
	unsigned long long int d, h, l;
	unsigned short denom[4];
	unsigned short numer[8];
	unsigned short quot[5];
	unsigned short rmdr[4];
	int num_denom;
	int num_numer;

	l = (unsigned long long int)z;		// Numerator low
	h = (unsigned long long int)y;		// Numerator high
	if (mode != MODE_UNSIGNED && (h & tbm) != 0) {
		if (mode == MODE_2COMP) {
			l = mask_value(1 + ~l);
			h = ~h;
			if (l == 0)
				h++;
			h = mask_value(h);
		} else if (mode == MODE_1COMP) {
			l = mask_value(~l);
			h = mask_value(~h);
		} else {
			h ^= tbm;
		}
		*sy = 1;
	} else
		*sy = 0;
	d = extract_value(x, sx);		// Demonimator
	if (d == 0) {
		err_div0(h|l, *sx, *sy);
		return 0;
	}

	if (ws != 64) {
		l |= h << ws;
		h >>= (64 - ws);
	}

	if (h == 0 && l == 0) {				// zero over
		*rem = 0;
		return 0;
	}

	xset(quot, 0, sizeof(quot));
	xset(rmdr, 0, sizeof(rmdr));

	breakup(d, denom);
	breakup(l, numer);
	breakup(h, numer+4);

	for (num_denom = 4; num_denom > 1 && denom[num_denom-1] == 0; num_denom--);
	for (num_numer = 8; num_numer > num_denom && numer[num_numer-1] == 0; num_numer--);

	divmnu(quot, rmdr, numer, denom, num_numer, num_denom);

	*rem = packup(rmdr);
	return packup(quot);
}
#endif

long long int intDblDiv(long long int z, long long int y, long long int x) {
#ifndef TINY_BUILD
	unsigned long long int q, r;
	int sx, sy;

	q = divmod(z, y, x, &sx, &sy, &r);
	set_overflow(0);
	set_carry(r != 0);
	return build_value(q, sx != sy);
#else
	return 0;
#endif
}

long long int intDblRmdr(long long int z, long long int y, long long int x) {
#ifndef TINY_BUILD
	unsigned long long int r;
	int sx, sy;

	divmod(z, y, x, &sx, &sy, &r);
	return build_value(r, sy);
#else
	return 0;
#endif
}


long long int intNot(long long int x) {
	return mask_value(~x);
}

long long int intBooleanOp(long long int y, long long int x) {
	long long result;
	const int op = XeqOpCode - (OP_DYA | OP_LAND);
	const int not = op >= 3 ? 3 : 0;

	switch (op - not) {
	case 0:  result = y & x; break;
	case 1:  result = y | x; break;
	default: result = y ^ x; break;
	}
	if (not)
		result = ~result;
	return mask_value(result);
}

/* Fraction and integer parts are very easy for integers.
 */
long long int intFP(long long int x) {
	return 0;
}


long long int intSign(long long int x) {
	int sgn;
	unsigned long long int v = extract_value(x, &sgn);

	if (v == 0)
		sgn = 0;
	else
		v = 1;
	return build_value(v, sgn);
}


/* Single bit shifts are special internal version.
 * The multi-bit shifts vector through these.
 */

#ifndef TINY_BUILD
static long long int intLSL(long long int x) {
	set_carry(0 != (topbit_mask() & x));
	return mask_value((x << 1) & ~1);
}

static long long int intLSR(long long int x) {
	set_carry(0 != (x & 1));
	return mask_value((x >> 1) & ~topbit_mask());
}

static long long int intASR(long long int x) {
	const enum arithmetic_modes mode = int_mode();
	const long long int tbm = topbit_mask();
	long long int y;

	set_carry(x & 1);
	if (mode == MODE_SGNMANT)
		return ((x & ~tbm) >> 1) | tbm;

	y = x >> 1;
	if (mode != MODE_UNSIGNED && (x & tbm) != 0)
		y |= tbm;
	return y;
}

static long long int intRL(long long int x) {
	const int cry = (topbit_mask() & x)?1:0;

	set_carry(cry);
	return mask_value(intLSL(x) | cry);
}

static long long int intRR(long long int x) {
	const int cry = x & 1;

	set_carry(cry);
	x = intLSR(x);
	if (cry)
		x |= topbit_mask();
	return mask_value(x);
}

static long long int intRLC(long long int x) {
	const int cin = get_carry();
	set_carry((topbit_mask() & x)?1:0);
	return mask_value(intLSL(x) | cin);
}

static long long int intRRC(long long int x) {
	const int cin = get_carry();

	set_carry(x&1);
	x = intLSR(x);
	if (cin)
		x |= topbit_mask();
	return mask_value(x);
}
#endif

/* Like the above but taking the count argument from the opcode.
 * Also possibly register indirect but that is dealt with elsewhere.
 */
void introt(unsigned int arg, enum rarg op) {
#ifndef TINY_BUILD
	long long int (*f)(long long int);
	unsigned int mod;
	unsigned int ws;
	long long int x;
	unsigned int i;
	
	if (check_intmode())
		return;

	ws = word_size();
	x = getX_int();

	if (arg != 0) {
		switch (op) {
		case RARG_RL:	f = &intRL;	mod = ws;	break;
		case RARG_RR:	f = &intRR;	mod = ws;	break;
		case RARG_RLC:	f = &intRLC;	mod = ws + 1;	break;
		case RARG_RRC:	f = &intRRC;	mod = ws + 1;	break;
		case RARG_SL:	f = &intLSL;	mod = 0;	break;
		case RARG_SR:	f = &intLSR;	mod = 0;	break;
		case RARG_ASR:	f = &intASR;	mod = 0;	break;
		default:
			return;
		}
		if (arg > ws) {
			if (mod)
				arg = arg % mod;
			else
				arg = ws;
		}
		for (i=0; i<arg; i++)
			x = (*f)(x);
	}
	setlastX();
	setX_int(mask_value(x));
#endif
}


#ifndef TINY_BUILD
/* Some code to count bits.  We start with a routine to count bits in a single
 * 32 bit word and call this twice.
 */
static unsigned int count32bits(unsigned long int v) {
	v = v - ((v >> 1) & 0x55555555);
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
	return (((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
}

static unsigned int count64bits(long long int x) {
	return count32bits(x & 0xffffffff) + count32bits((x >> 32) & 0xffffffff);
}
#endif

long long int intNumBits(long long int x) {
#ifndef TINY_BUILD
	return mask_value(count64bits(x));
#else
	return 0;
#endif
}


/* Integer floor(sqrt())
 */
long long int intSqrt(long long int x) {
#if !defined(TINY_BUILD)
	int sx;
	unsigned long long int v = extract_value(x, &sx);
	unsigned long long int n0, n1;

	if (sx) {
		err(ERR_DOMAIN);
		return 0;
	}
	if (v == 0)
		n1 = 0;
	else {
		n0 = v / 2 + 1;
		n1 = v / n0 + n0 / 2;
		while (n1 < n0) {
			n0 = n1;
			n1 = (n0 + v / n0) / 2;
		}
		n0 = n1 * n1;
		if (n0 > v)
			n1--;
		set_carry((n0 != v)?1:0);
	}
	return build_value(n1, sx);
#else
	return 0;
#endif
}

long long int int_power_helper(unsigned long long int vy, unsigned long long int vx, int overflow) {
	unsigned long long int r = 1;
	unsigned int i;
	unsigned int ws;
	int overflow_next = 0;

	ws = word_size();
	for (i=0; i<ws && vx != 0; i++) {
		if (vx & 1) {
			if (overflow_next)
				overflow = 1;
			r = multiply_with_overflow(r, vy, &overflow);
		}
		vx >>= 1;
		vy = multiply_with_overflow(vy, vy, &overflow_next);
	}
	set_overflow(overflow);
	return r;
}

/* Integer power y^x
 */
long long int intPower(long long int y, long long int x) {
#ifndef TINY_BUILD
	int sx, sy, sr;
	unsigned long long int vx = extract_value(x, &sx);
	unsigned long long int vy = extract_value(y, &sy);

	if (vx == 0 && vy == 0) {
		err(ERR_DOMAIN);
		return 0;
	}
	set_carry(0);
	set_overflow(0);

	if (vx == 0) {
		if (vy == 0) {
			set_overflow(1);
			return 0;
		}
		return 1;
	} else if (vy == 0)
		return 0;

	if (sx) {
		set_carry(1);
		return 0;
	}

	sr = (sy && (vx & 1))?1:0;	// Determine the sign of the result

	return build_value(int_power_helper(vy, vx, 0), sr);
#else
	return 0;
#endif
}


/* Integer floor(log2())
 */
long long int intLog2(long long int x) {
#ifndef TINY_BUILD
	int sx;
	unsigned long long int v = extract_value(x, &sx);
	unsigned int r = 0;

	if (v == 0 || sx) {
		err(ERR_DOMAIN);
		return 0;
	}
	set_carry((v & (v-1))?1:0);
	if (v != 0)
		while (v >>= 1)
			r++;
	return build_value(r, sx);
#else
	return 0;
#endif
}


/* 2^x
 */
long long int int2pow(long long int x) {
#ifndef TINY_BUILD
	int sx;
	unsigned long long int v = extract_value(x, &sx);
	unsigned int ws;

	set_overflow(0);
	set_carry(sx && v == 1);
	if (sx && v != 0)
		return 0;

	ws = word_size();
	if (int_mode() != MODE_UNSIGNED)
		ws--;
	if (v >= ws) {
		set_carry(v == ws);
		set_overflow(1);
		return 0;
	}

	return 1LL << (unsigned int)(v & 0xff);
#else
	return 0;
#endif
}


/* Integer floor(log10())
 */
long long int intLog10(long long int x) {
#ifndef TINY_BUILD
	int sx;
	unsigned long long int v = extract_value(x, &sx);
	int r = 0;
	int c = 0;

	if (v == 0 || sx) {
		err(ERR_DOMAIN);
		return 0;
	}
	while (v >= 10) {
		r++;
		if (v % 10)
			c = 1;
		v /= 10;
	}
	set_carry(c || v != 1);
	return build_value(r, sx);
#else
	return 0;
#endif
}


/* 10^x
 */
long long int int10pow(long long int x) {
	int sx;
	unsigned long long int vx = extract_value(x, &sx);
	const unsigned int ws = word_size();
	int overflow = 0;
	
	set_carry(0);
	if (vx == 0) {
		set_overflow(0);
		return 1;
	}
	if (sx) {
		set_carry(1);
		return 0;
	}

	if (ws <= 3 || (int_mode() != MODE_UNSIGNED && ws == 4))
		overflow = 1;
	return build_value(int_power_helper(10, x, overflow), 0);
}


/* -1^x
 */
long long int int_1pow(long long int x) {
#ifndef TINY_BUILD
	int sx;
	unsigned long long int xv = extract_value(x, &sx);
	int odd = xv & 1;

	set_overflow((int_mode() == MODE_UNSIGNED && odd) ? 1 : 0);
	return build_value((unsigned long long int)1, odd);
#else
	return 0;
#endif
}


/* Mirror - reverse the bits in the word
 */
long long int intMirror(long long int x) {
#ifndef TINY_BUILD
	long long int r = 0;
	unsigned int n = word_size();
	unsigned int i;

	if (x == 0)
		return 0;

	for (i=0; i<n; i++)
		if (x & (1LL << i))
			r |= 1LL << (n-i-1);
	return r;
#else
	return 0;
#endif
}


/* Justify to the end of the register
 */
static void justify(long long int (*shift)(long long int), const long long int mask) {
	unsigned int c = 0;
	long long int v;

	v = getX_int();
	setlastX();
	lift();
	if (v != 0) {
		const int flags = save_flags();
		while ((v & mask) == 0) {
			v = (*shift)(v);
			c++;
		}
		restore_flags(flags);
		set_reg_n_int(regY_idx, v);
	}
	setX_int((long long int)c);
}

void int_justify(enum nilop op) {
	const unsigned long long int mask = (op == OP_LJ) ? topbit_mask() : 1LL;
	justify((op == OP_LJ) ? &intLSL : &intLSR, mask);
}


/* Create n bit masks at either end of the word.
 * If the n is negative, the mask is created at the other end of the
 * word.
 */
void intmsks(unsigned int arg, enum rarg op) {
#ifndef TINY_BUILD
	long long int mask;
	long long int x;
	unsigned int i;
	long long int (*f)(long long int);
	const int carry = get_carry();

	lift();

	if (op == RARG_MASKL) {
		mask = topbit_mask();
		f = &intLSR;
	} else {
		mask = 1LL;
		f = &intLSL;
	}
	if (arg >= word_size()) {
		x = mask_value(-1);
	} else {
		x = 0;
		for (i=0; i<arg; i++) {
			x |= mask;
			mask = (*f)(mask);
		}
	}
	setX_int(x);
	set_carry(carry);
#endif
}


/* Set, clear, flip and test bits */
void intbits(unsigned int arg, enum rarg op) {
#ifndef TINY_BUILD
	long long int m, x;

	if (check_intmode())
		return;

	m =  (arg >= word_size())?0:(1LL << arg);
	x = getX_int();

	switch (op) {
	case RARG_SB:	x |= m;		setlastX();		break;
	case RARG_CB:	x &= ~m;	setlastX();		break;
	case RARG_FB:	x ^= m;		setlastX();		break;
	case RARG_BS:	fin_tst((x&m)?1:0);			break;
	case RARG_BC:	fin_tst((m != 0 && (x&m) != 0)?0:1);	break;
	default:
		return;
	}

	setX_int(x);
#endif
}

long long int intFib(long long int x) {
#ifndef TINY_BUILD
	int sx, s;
	unsigned long long int v = extract_value(x, &sx);
	const enum arithmetic_modes mode = int_mode();
	unsigned long long int a0, a1;
	unsigned int n, i;
	long long int tbm;

	/* Limit things so we don't loop for too long.
	 * The actual largest non-overflowing values for 64 bit integers
	 * are Fib(92) for signed quantities and Fib(93) for unsigned.
	 * We allow a bit more and maintain the low order digits.
	 */
	if (v >= 100) {
		set_overflow(1);
		return 0;
	}
	set_overflow(0);
	n = v & 0xff;
	if (n <= 1)
		return build_value(n, 0);

	/* Negative integers produce the same values as positive
	 * except the sign for negative evens is negative.
	 */
	s = (sx && (n & 1) == 0)?1:0;

	/* Mask to check for overflow */
	tbm = topbit_mask();
	if (mode == MODE_UNSIGNED)
		tbm <<= 1;

	/* Down to the computation.
	 */
	a0 = 0;
	a1 = 1;
	for (i=1; i<n; i++) {
		const unsigned long long int anew = a0 + a1;
		if ((anew & tbm) || anew < a1)
			set_overflow(1);
		a0 = a1;
		a1 = anew;
	}
	return build_value(a1, s);
#else
	return 0;
#endif
}


/* Calculate (a . b) mod c taking care to avoid overflow */
static unsigned long long mulmod(const unsigned long long int a, unsigned long long int b, const unsigned long long int c) {
	unsigned long long int x=0, y=a%c;
	while (b > 0) {
		if ((b & 1))
			x = (x+y)%c;
		y = (y+y)%c;
		b /= 2;
	}
	return x % c;
}

/* Calculate (a ^ b) mod c */
static unsigned long long int expmod(const unsigned long long int a, unsigned long long int b, const unsigned long long int c) {
	unsigned long long int x=1, y=a;
	while (b > 0) {
		if ((b & 1))
			x = mulmod(x, y, c);
		y = mulmod(y, y, c);
		b /= 2;
	}
	return (x % c);
}

/* Test if a number is prime or not using a Miller-Rabin test */
#ifndef TINY_BUILD
static const unsigned char primes[] = {
	2, 3, 5, 7,	11, 13, 17, 19,
	23, 29, 31, 37,	41, 43, 47, 53,
};
#define N_PRIMES	(sizeof(primes) / sizeof(unsigned char))
#define QUICK_CHECK	(59*59-1)
#endif

int isPrime(unsigned long long int p) {
#ifndef TINY_BUILD
	int i;
	unsigned long long int s;
#define PRIME_ITERATION	15

	/* Quick check for p <= 2 and evens */
	if (p < 2)	return 0;
	if (p == 2)	return 1;
	if ((p&1) == 0)	return 0;

	/* We fail for numbers >= 2^63 */
	if ((p & 0x8000000000000000ull) != 0) {
		err(ERR_DOMAIN);
		return 1;
	}

	/* Quick check for divisibility by small primes */
	for (i=1; i<N_PRIMES; i++)
		if (p == primes[i])
			return 1;
		else if ((p % primes[i]) == 0)
			return 0;
	if (p < QUICK_CHECK)
		return 1;

	s = p - 1;
	while ((s&1) == 0)
		s /= 2;

	for(i=0; i<PRIME_ITERATION; i++) {
		unsigned long long int temp = s;
		unsigned long long int mod = expmod(primes[i], temp, p);
		while (temp != p-1 && mod != 1 && mod != p-1) {
			mod = mulmod(mod, mod, p);
			temp += temp;
		}
		if(mod!=p-1 && temp%2==0)
			return 0;
	}
#endif
	return 1;
}

#ifdef INCLUDE_INT_MODULO_OPS
long long int intmodop(long long int z, long long int y, long long int x) {
	int sx, sy, sz;
	unsigned long long int vx = extract_value(x, &sx);
	unsigned long long int vy = extract_value(y, &sy);
	unsigned long long int vz = extract_value(z, &sz);
	unsigned long long int r;

	if (sx || sy || sz || vx <= 1)
		err(ERR_DOMAIN);
	if (XeqOpCode == (OP_TRI | OP_MULMOD))
		r = mulmod(vz, vy, vx);
	else
		r = expmod(vz, vy, vx);
	return build_value(r, 0);
}

#endif


#ifdef INCLUDE_FACTOR

#ifndef TINY_BUILD

// only need 8 terms for factors > 256
#define MAX_TERMS	8

static int dscanOdd(unsigned int d, unsigned int limit, int nd, unsigned int ad[MAX_TERMS])
{
	/* given starting odd `d', skip two divisors at a time and thus
	* scan only the odd numbers.
	*/
	int i, j;
	while (ad[0])
	{
		d += 2;
		if (d > limit) return 0; // limit reached
		for (i = nd-2; i >= 0; --i)
		{
			for (j = i; j < nd-1; ++j)
			{
				int v = ad[j] - ad[j+1] - ad[j+1];
				if (v < 0)
				{
					v += d;
					--ad[j+1];
					if (v < 0)
					{
						v += d;
						--ad[j+1];
					}
				}
				ad[j] = v;
			}
			if (!ad[j]) --nd;
		}
	}
	return d;
}
#endif

unsigned long long int doFactor(unsigned long long int n)
{
#ifndef TINY_BUILD
	/* find the least prime factor of `n'.
	* numbers up to 10^14 can be factored. worst case about 30 seconds
	* on realbuild.
	*
	* returns least prime factor or `n' if prime.
	* returns 0 if failed to find factor.
	*
	* we will only fail if we have a 14 digit number with a factor > dmax (1e7).
	* since we have a 12 digit display, this ought to be good, but actually more digits are
	* held internally. for example 10000019*1000079 displays as scientific, but actually all
	* the digits are held. this example will return 0.
	*/

	unsigned int d;
	unsigned int dmax = 10000000; // biggest factor, 10^7
	unsigned int rt;
	unsigned int limit;

	unsigned int ad[MAX_TERMS];
	int nd;
	int i, j;
	unsigned char* cp;

	// eliminate small cases < 257
	if (n <= 2) return n;
	if ((n & 1) == 0) return 2;
	for (i=1; i<N_PRIMES; i++) {
		if (n % primes[i] == 0)
			return primes[i];
	}
	if (n <= QUICK_CHECK)		// the number is prime
		return n;

	rt = (unsigned int)intSqrt(n);
	limit = rt;
	if (limit > dmax)
		limit = dmax; // max time about 30 seconds

	// starting factor for search
	d = 257;

	// since we've eliminated all factors < 257, convert
	// the initial number to bytes to get base 256
	// XX ASSUME little endian here.
	cp = (unsigned char*)&n;
	nd = 0;
	for (i = 0; i < sizeof(n); ++i)
		if ((ad[i] = *cp++) != 0) ++nd;

	// and slide to 257
	for (i = nd-2; i >= 0; --i)
	{
		for (j = i; j < nd-1; ++j)
		{
			if ((ad[j] -= ad[j+1]) < 0)
			{
				ad[j] += d;
				--ad[j+1];
			}
		}
		if (!ad[j]) --nd;
	}

	if (ad[0])
	{
		// find factor or return 0 if limit reached
		d = dscanOdd(d, limit, nd, ad);
		if (!d)
		{
			// no factor found, if limit reached, we've failed
			// otherwise `n' is prime
			if (limit == dmax)
			n = 0; // fail
		}
	}

	if (d) n = d;
	return n;
#else
	return 0;
#endif
}
#undef MAX_TERMS


long long int intFactor(long long int x) {
	int sx;
	unsigned long long int vx = extract_value(x, &sx);
	unsigned long long int r = doFactor(vx);
	return build_value(r, sx);
}
#endif // INCLUDE_FACTOR

#ifdef INCLUDE_USER_IO
long long int intRecv(long long int x) {
	int sx;
	unsigned long long int xv = extract_value(x, &sx);
	int to = xv & 0x7fffffff;
	int c;

	if (sx)
		to = -1;
	c = recv_byte(to);

	sx = c < 0;
	if (sx)
		c = -c;
	set_overflow(sx);
	return build_value(c, sx);
}
#endif

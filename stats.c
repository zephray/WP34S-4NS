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

// New version with variable register block

#include "xeq.h"
#include "decn.h"
#include "stats.h"
#include "consts.h"
#include "int.h"

// #define DUMP1	// Debug output

#define DISCRETE_TOLERANCE	&const_0_1

/*
 *  Define register block
 */
STAT_DATA *StatRegs;

#define sigmaN		(StatRegs->sN)
#define sigmaX		(StatRegs->sX)
#define sigmaY		(StatRegs->sY)
#define sigmaX2		(StatRegs->sX2)
#define sigmaY2		(StatRegs->sY2)
#define sigmaXY		(StatRegs->sXY)
#define sigmaX2Y	(StatRegs->sX2Y)
#define sigmalnX	(StatRegs->slnX)
#define sigmalnXlnX	(StatRegs->slnXlnX)
#define sigmalnY	(StatRegs->slnY)
#define sigmalnYlnY	(StatRegs->slnYlnY)
#define sigmalnXlnY	(StatRegs->slnXlnY)
#define sigmaXlnY	(StatRegs->sXlnY)
#define sigmaYlnX	(StatRegs->sYlnX)

/*
 *  Actual size of this block (may be zero)
 */
SMALL_INT SizeStatRegs;

/*
 *  Handle block (de)allocation
 */
int sigmaCheck(void) {
	if (SizeStatRegs == 0) {
		err(ERR_MORE_POINTS);
		return 1;
	}
	StatRegs = (STAT_DATA *) ((unsigned short *)(Regs + TOPREALREG - NumRegs) - SizeStatRegs);
	return 0;
}

static int sigmaAllocate(void)
{
	if (State.have_stats == 0) {
		SizeStatRegs = sizeof(STAT_DATA) >> 1;	// in 16 bit words!
		if (move_retstk(-SizeStatRegs)) {
			SizeStatRegs = 0;
			err(ERR_RAM_FULL);
			return 1;
		}
		State.have_stats = 1;
		sigmaCheck();
		xset(StatRegs, 0, sizeof(STAT_DATA));
	}
	return sigmaCheck();
}

void sigmaDeallocate(void) {
	move_retstk(SizeStatRegs);
	SizeStatRegs = 0;
	State.have_stats = 0;
}

/*
 *  Helper for serial and storage commands
 */
int sigmaCopy(void *source)
{
	if (sigmaAllocate())
		return 1;
	xcopy(StatRegs, source, sizeof(STAT_DATA));
	return 0;
}

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
#endif


static void correlation(decNumber *, const enum sigma_modes);

static int check_data(int n) {
	if (sigmaCheck() || sigmaN < n) {
		err(ERR_MORE_POINTS);
		return 1;
	}
	return 0;
}

void stats_mode(enum nilop op) {
	UState.sigma_mode = (op - OP_LINF) + SIGMA_LINEAR;
}

void sigma_clear(enum nilop op) {
	sigmaDeallocate();
}


/* Accumulate sigma data.
 */
static void sigop(decimal64 *r, const decNumber *a, decNumber *(*op)(decNumber *, const decNumber *, const decNumber *)) {
	decNumber t, u;

	decimal64ToNumber(r, &t);
	(*op)(&u, &t, a);
	packed_from_number(r, &u);
}

static void sigop128(decimal128 *r, const decNumber *a, decNumber *(*op)(decNumber *, const decNumber *, const decNumber *)) {
	decNumber t, u;

	decimal128ToNumber(r, &t);
	(*op)(&u, &t, a);
	packed128_from_number(r, &u);
}


/* Multiply a pair of values and accumulate into the sigma data.
 */
static void mulop(decimal64 *r, const decNumber *a, const decNumber *b, decNumber *(*op)(decNumber *, const decNumber *, const decNumber *)) {
	decNumber t;

	sigop(r, dn_multiply(&t, a, b), op);
}

static void mulop128(decimal128 *r, const decNumber *a, const decNumber *b, decNumber *(*op)(decNumber *, const decNumber *, const decNumber *)) {
	decNumber t;

	sigop128(r, dn_multiply(&t, a, b), op);
}


/* Define a helper function to handle sigma+ and sigma-
 */
static void sigma_helper(decNumber *(*op)(decNumber *, const decNumber *, const decNumber *), const decNumber *x, const decNumber *y) {
	decNumber lx, ly;

	sigop(&sigmaX, x, op);
	sigop(&sigmaY, y, op);
	mulop128(&sigmaX2, x, x, op);
	mulop128(&sigmaY2, y, y, op);
	mulop128(&sigmaXY, x, y, op);

	decNumberSquare(&lx, x);
	mulop128(&sigmaX2Y, &lx, y, op);

//	if (UState.sigma_mode == SIGMA_LINEAR)
//		return;

	dn_ln(&lx, x);
	dn_ln(&ly, y);

	sigop(&sigmalnX, &lx, op);
	sigop(&sigmalnY, &ly, op);
	mulop(&sigmalnXlnX, &lx, &lx, op);
	mulop(&sigmalnYlnY, &ly, &ly, op);
	mulop(&sigmalnXlnY, &lx, &ly, op);
	mulop(&sigmaXlnY, x, &ly, op);
	mulop(&sigmaYlnX, y, &lx, op);
}

static void sigma_helper_xy(decNumber *(*op)(decNumber *, const decNumber *, const decNumber *)) {
	decNumber x, y;

	getXY(&x, &y);
	sigma_helper(op, &x, &y);
}

void sigma_plus() {
	if (sigmaAllocate())
		return;
	++sigmaN;
	sigma_helper_xy(&dn_add);
}

void sigma_minus() {
	if (sigmaAllocate())
		return;
	sigma_helper_xy(&dn_subtract);
	--sigmaN;
}

/*
 * Used by Stopwatch to compute round time averages
 */
int sigma_plus_x( const decNumber *x) {
	decNumber y;

	if (sigmaAllocate())
		return -1;
	
	ullint_to_dn(&y, ++sigmaN);
	sigma_helper(&dn_add, x, &y);
	return sigmaN;
}

/* Loop through the various modes and work out
 * which has the highest absolute correlation.
 */
static enum sigma_modes determine_best(void) {
	enum sigma_modes m = SIGMA_LINEAR;
	int i;
	decNumber b, c, d;

	if (sigmaN > 2) {
		correlation(&c, SIGMA_LINEAR);
		dn_abs(&b, &c);
		for (i=SIGMA_LINEAR+1; i<SIGMA_BEST; i++) {
			correlation(&d, (enum sigma_modes) i);

			if (! decNumberIsNaN(&d)) {
				dn_abs(&c, &d);
				if (dn_gt(&c, &b)) {
					decNumberCopy(&b, &c);
					m = (enum sigma_modes) i;
				}
			}
		}
	}
	return m;
}


/* Return the appropriate variables for the specified fit.
 * If the fit is best, call a routine to figure out which has the highest
 * absolute r.  This entails a recursive call back here.
 */
static enum sigma_modes get_sigmas(decNumber *N, decNumber *sx, decNumber *sy, 
					decNumber *sxx, decNumber *syy,
					decNumber *sxy, enum sigma_modes mode) {
	int lnx, lny;
	decimal64 *xy = NULL;

	if (mode == SIGMA_BEST)
		mode = determine_best();

	switch (mode) {
	default:
	case SIGMA_LINEAR:
		DispMsg = "Linear";
	case SIGMA_QUIET_LINEAR:
		lnx = lny = 0;
		break;

	case SIGMA_LOG:
		DispMsg = "Log";
		xy = &sigmaYlnX;
		lnx = 1;
		lny = 0;
		break;

	case SIGMA_EXP:
		DispMsg = "Exp";
		xy = &sigmaXlnY;
		lnx = 0;
		lny = 1;
		break;

	case SIGMA_POWER:
		DispMsg = "Power";
	case SIGMA_QUIET_POWER:
		xy = &sigmalnXlnY;
		lnx = lny = 1;
		break;
	}

	if (N != NULL)
		int_to_dn(N, sigmaN);
	if (sx != NULL)
		decimal64ToNumber(lnx ? &sigmalnX : &sigmaX, sx);
	if (sy != NULL)
		decimal64ToNumber(lny ? &sigmalnY : &sigmaY, sy);
	if (sxx != NULL) {
		if (lnx)
			decimal64ToNumber(&sigmalnXlnX, sxx);
		else
			decimal128ToNumber(&sigmaX2, sxx);
	}
	if (syy != NULL) {
		if (lny)
			decimal64ToNumber(&sigmalnYlnY, syy);
		else
			decimal128ToNumber(&sigmaY2, syy);
	}
	if (sxy != NULL) {
		if (lnx || lny)
			decimal64ToNumber(xy, sxy);
		else
			decimal128ToNumber(&sigmaXY, sxy);
	}
	return mode;
}


/*
 *  Return a summation register to the user.
 *  Opcodes have been reaaranged to move sigmaN to the end of the list.
 *  decimal64 values are grouped together, if decimal128 is used, regrouping is required
 */
void sigma_val(enum nilop op) {
	REGISTER *const x = StackBase;
	const int dbl = is_dblmode();
	if (SizeStatRegs == 0) {
		zero_X();
		return;
	}
	sigmaCheck();
	if (op == OP_sigmaN) {
		if (sigmaN > 0)
			setX_int_sgn( sigmaN, 0);
		else
			setX_int_sgn( -sigmaN, 1);
	}
	else if (op < OP_sigmaX) {
		decimal128 *d = (&sigmaX2Y) + (op - OP_sigmaX2Y);
		if (dbl)
			x->d = *d;
		else
			packed_from_packed128(&(x->s), d);
	}
	else {
		x->s = (&sigmaX)[op - OP_sigmaX];
		if (dbl)
			packed128_from_packed(&(x->d), &(x->s));
	}
}

void sigma_sum(enum nilop op) {
	REGISTER *const x = StackBase;
	REGISTER *const y = get_reg_n(regY_idx);

	if (SizeStatRegs == 0) {
		x->s = y->s = get_const(OP_ZERO, 0)->s;
	}
	else {
		sigmaCheck();	// recompute pointer to StatRegs
		x->s = sigmaX;
		y->s = sigmaY;
	}
	if (is_dblmode()) {
		packed128_from_packed(&(x->d), &(x->s));
		packed128_from_packed(&(y->d), &(y->s));
	}
}


static void mean_common(int index, const decNumber *x, const decNumber *n, int exp) {
	decNumber t, u, *p = &t;

	dn_divide(&t, x, n);
	if (exp)
		dn_exp(p=&u, &t);
	setRegister(index, p);
}

void stats_mean(enum nilop op) {
	decNumber N;
	decNumber sx, sy;

	if (check_data(1))
		return;
	get_sigmas(&N, &sx, &sy, NULL, NULL, NULL, SIGMA_QUIET_LINEAR);

	mean_common(regX_idx, &sx, &N, 0);
	mean_common(regY_idx, &sy, &N, 0);
}


// weighted mean sigmaXY / sigmaY
void stats_wmean(enum nilop op) {
	decNumber xy, y;

	if (check_data(1))
		return;
	get_sigmas(NULL, NULL, &y, NULL, NULL, &xy, SIGMA_QUIET_LINEAR);

	mean_common(regX_idx, &xy, &y, 0);
}

// geometric mean e^(sigmaLnX / N)
void stats_gmean(enum nilop op) {
	decNumber N;
	decNumber sx, sy;

	if (check_data(1))
		return;
	get_sigmas(&N, &sx, &sy, NULL, NULL, NULL, SIGMA_QUIET_POWER);

	mean_common(regX_idx, &sx, &N, 1);
	mean_common(regY_idx, &sy, &N, 1);
}

// Standard deviations and standard errors
static void do_s(int index,
		const decNumber *sxx, const decNumber *sx,
		const decNumber *N, const decNumber *denom, 
		int rootn, int exp) {
	decNumber t, u, v, *p = &t;

	decNumberSquare(&t, sx);
	dn_divide(&u, &t, N);
	dn_subtract(&t, sxx, &u);
	dn_divide(&u, &t, denom);
	if (dn_le0(&u))
		decNumberZero(&t);
	else
		dn_sqrt(&t, &u);

	if (rootn) {
		dn_sqrt(&u, N);
		dn_divide(p = &v, &t, &u);
	}
	if (exp) {
		dn_exp(&u, p);
		p = &u;
	}
	setRegister(index, p);
}

// sx = sqrt(sigmaX^2 - (sigmaX ^ 2 ) / (n-1))
void stats_deviations(enum nilop op) {
	decNumber N, nm1, *n = &N;
	decNumber sx, sxx, sy, syy;
	int sample = 1, rootn = 0, exp = 0;

	if (check_data(2))
		return;

	if (op == OP_statSigma || op == OP_statGSigma)
		sample = 0;
	if (op == OP_statSErr || op == OP_statGSErr)
		rootn = 1;
	if (op == OP_statGS || op == OP_statGSigma || op == OP_statGSErr)
		exp = 1;

	get_sigmas(&N, &sx, &sy, &sxx, &syy, NULL, exp?SIGMA_QUIET_POWER:SIGMA_QUIET_LINEAR);
	if (sample)
		dn_m1(n = &nm1, &N);
	do_s(regX_idx, &sxx, &sx, &N, n, rootn, exp);
	do_s(regY_idx, &syy, &sy, &N, n, rootn, exp);
}



// Weighted standard deviation
void stats_wdeviations(enum nilop op) {
	decNumber sxxy, sy, sxy, syy;
	decNumber t, u, v, w, *p;
	int sample = 1, rootn = 0;

	if (op == OP_statWSigma)
		sample = 0;
	if (op == OP_statWSErr)
		rootn = 1;

	if (sigmaCheck())
		return;
	get_sigmas(NULL, NULL, &sy, NULL, &syy, &sxy, SIGMA_QUIET_LINEAR);
	if (dn_lt(&sy, &const_2)) {
		err(ERR_MORE_POINTS);
		return;
	}
	decimal128ToNumber(&sigmaX2Y, &sxxy);
	dn_multiply(&t, &sy, &sxxy);
	decNumberSquare(&u, &sxy);
	dn_subtract(&v, &t, &u);
	decNumberSquare(p = &t, &sy);
	if (sample)
		dn_subtract(p = &u, &t, &syy);
	dn_divide(&w, &v, p);
	dn_sqrt(p = &u, &w);
	if (rootn) {
		dn_sqrt(&t, &sy);
		dn_divide(p = &v, &u, &t);
	}
	setX(p);
}


decNumber *stats_sigper(decNumber *res, const decNumber *x) {
	decNumber sx, t;

	if (sigmaCheck())
		return res;
	get_sigmas(NULL, &sx, NULL, NULL, NULL, NULL, SIGMA_QUIET_LINEAR);
	dn_divide(&t, x, &sx);
	return dn_mul100(res, &t);
}

/* Calculate the correlation based on the stats data using the
 * specified model.
 */
static void correlation(decNumber *t, const enum sigma_modes m) {
	decNumber N, u, v, w;
	decNumber sx, sy, sxx, syy, sxy;

	get_sigmas(&N, &sx, &sy, &sxx, &syy, &sxy, m);

	dn_multiply(t, &N, &sxx);
	decNumberSquare(&u, &sx);
	dn_subtract(&v, t, &u);
	dn_multiply(t, &N, &syy);
	decNumberSquare(&u, &sy);
	dn_subtract(&w, t, &u);
	dn_multiply(t, &v, &w);
	dn_sqrt(&w, t);
	dn_multiply(t, &N, &sxy);
	dn_multiply(&u, &sx, &sy);
	dn_subtract(&v, t, &u);
	dn_divide(t, &v, &w);

	if (dn_gt(t, &const_1))
		dn_1(t);
	else if (dn_lt(t, &const__1))
		dn__1(t);
}


void stats_correlation(enum nilop op) {
	decNumber t;

	if (check_data(2))
		return;
	correlation(&t, (enum sigma_modes) UState.sigma_mode);
	setX(&t);
}


void stats_COV(enum nilop op) {
	const int sample = (op == OP_statCOV) ? 0 : 1;
	decNumber N, t, u, v;
	decNumber sx, sy, sxy;

	if (check_data(2))
		return;
	get_sigmas(&N, &sx, &sy, NULL, NULL, &sxy, (enum sigma_modes) UState.sigma_mode);
	dn_multiply(&t, &sx, &sy);
	dn_divide(&u, &t, &N);
	dn_subtract(&t, &sxy, &u);
	if (sample) {
		dn_m1(&v, &N);
		dn_divide(&u, &t, &v);
	} else
		dn_divide(&u, &t, &N);
	setX(&u);
}


// y = B + A . x
static enum sigma_modes do_LR(decNumber *B, decNumber *A) {
	decNumber N, u, v, denom;
	decNumber sx, sy, sxx, sxy;
	enum sigma_modes m;

	m = get_sigmas(&N, &sx, &sy, &sxx, NULL, &sxy, (enum sigma_modes) UState.sigma_mode);

	dn_multiply(B, &N, &sxx);
	decNumberSquare(&u, &sx);
	dn_subtract(&denom, B, &u);

	dn_multiply(B, &N, &sxy);
	dn_multiply(&u, &sx, &sy);
	dn_subtract(&v, B, &u);
	dn_divide(A, &v, &denom);

	dn_multiply(B, &sxx, &sy);
	dn_multiply(&u, &sx, &sxy);
	dn_subtract(&v, B, &u);
	dn_divide(B, &v, &denom);

	return m;
}


void stats_LR(enum nilop op) {
	decNumber a, b;
	enum sigma_modes m;

	if (check_data(2))
		return;
	m = do_LR(&b, &a);
	if (m == SIGMA_EXP || m == SIGMA_POWER || m == SIGMA_QUIET_POWER)
		dn_exp(&b, &b);
	setY(&a);
	setX(&b);
}


decNumber *stats_xhat(decNumber *res, const decNumber *y) {
	decNumber a, b, t, u;
	enum sigma_modes m;

	if (check_data(2))
		return set_NaN(res);
	m = do_LR(&b, &a);
	switch (m) {
	default:
		dn_subtract(&t, y, &b);
		return dn_divide(res, &t, &a);

	case SIGMA_EXP:
		dn_ln(&t, y);
		dn_subtract(&u, &t, &b);
		return dn_divide(res, &u, &a);

	case SIGMA_LOG:
		dn_subtract(&t, y, &b);
		dn_divide(&b, &t, &a);
		return dn_exp(res, &b);

	case SIGMA_POWER:
	case SIGMA_QUIET_POWER:
		dn_ln(&t, y);
		dn_subtract(&u, &t, &b);
		dn_divide(&t, &u, &a);
		return dn_exp(res, &t);
	}
}


decNumber *stats_yhat(decNumber *res, const decNumber *x) {
	decNumber a, b, t, u;
	enum sigma_modes m;

	if (check_data(2))
		return set_NaN(res);
	m = do_LR(&b, &a);
	switch (m) {
	default:
		dn_multiply(&t, x, &a);
		return dn_add(res, &t, &b);

	case SIGMA_EXP:
		dn_multiply(&t, x, &a);
		dn_add(&a, &b, &t);
		return dn_exp(res, &a);

	case SIGMA_LOG:
		dn_ln(&u, x);
		dn_multiply(&t, &u, &a);
		return dn_add(res, &t, &b);

	case SIGMA_POWER:
	case SIGMA_QUIET_POWER:
		dn_ln(&t, x);
		dn_multiply(&u, &t, &a);
		dn_add(&t, &u, &b);
		return dn_exp(res, &t);
	}
}


/* rng/taus.c from the GNU Scientific Library.
 * The period of this generator is about 2^88.
 */
static unsigned long int taus_get(void) {
#define MASK 0xffffffffUL
#define TAUSWORTHE(s,a,b,c,d) (((s & c) << d) & MASK) ^ ((((s << a) & MASK) ^ s) >> b)

  RandS1 = TAUSWORTHE (RandS1, 13, 19, 4294967294UL, 12);
  RandS2 = TAUSWORTHE (RandS2,  2, 25, 4294967288UL, 4);
  RandS3 = TAUSWORTHE (RandS3,  3, 11, 4294967280UL, 17);

  return RandS1 ^ RandS2 ^ RandS3;
}

static void taus_seed(unsigned long int s) {
	int i;

	if (s == 0)
		s = 1;
#define LCG(n) ((69069 * n) & 0xffffffffUL)
	RandS1 = LCG (s);
	if (RandS1 < 2) RandS1 += 2UL;

	RandS2 = LCG (RandS1);
	if (RandS2 < 8) RandS2 += 8UL;

	RandS3 = LCG (RandS2);
	if (RandS3 < 16) RandS3 += 16UL;
#undef LCG

	for (i=0; i<6; i++)
		taus_get();
}

void stats_random(enum nilop op) {
	// Start by generating the next in sequence
	unsigned long int s;
	decNumber y, z;

	if (RandS1 == 0 && RandS2 == 0 && RandS3 == 0)
		taus_seed(0);
	s = taus_get();

	// Now build ourselves a number
	if (is_intmode())
		setX_int_sgn((((unsigned long long int)taus_get()) << 32) | s, 0);
	else {
		ullint_to_dn(&z, s);
		dn_multiply(&y, &z, &const_randfac);
		setX(&y);
	}
}


void stats_sto_random(enum nilop op) {
	unsigned long int s;
	int z;
	decNumber x;

	if (is_intmode()) {
		 s = getX_int() & 0xffffffff;
	} else {
		getX(&x);
		s = (unsigned long int) dn_to_ull(&x, &z);
	}
	taus_seed(s);
}

static void check_low(decNumber *d) {
	if (dn_abs_lt(d, &const_1e_32))
		decNumberCopy(d, &const_1e_32);
}

static void ib_step(decNumber *d, decNumber *c, const decNumber *aa) {
	decNumber t, u;

	dn_multiply(&t, aa, d);
	dn_p1(&u, &t);		// d = 1+aa*d
	check_low(&u);
	decNumberRecip(d, &u);
	dn_divide(&t, aa, c);
	dn_p1(c, &t);		// c = 1+aa/c
	check_low(c);
}


static void betacf(decNumber *r, const decNumber *a, const decNumber *b, const decNumber *x) {
	decNumber aa, c, d, apb, am1, ap1, m, m2, oldr;
	int i;
	decNumber t, u, v, w;

	dn_p1(&ap1, a);				// ap1 = 1+a
	dn_m1(&am1, a);				// am1 = a-1
	dn_add(&apb, a, b);			// apb = a+b
	dn_1(&c);				// c = 1
	dn_divide(&t, x, &ap1);
	dn_multiply(&u, &t, &apb);
	dn_1m(&t, &u);				// t = 1-apb*x/ap1
	check_low(&t);
	decNumberRecip(&d, &t);			// d = 1/t
	decNumberCopy(r, &d);				// res = d
	decNumberZero(&m);
	for (i=0; i<500; i++) {
		decNumberCopy(&oldr, r);
		dn_inc(&m);			// m = i+1
		dn_mul2(&m2, &m);
		dn_subtract(&t, b, &m);
		dn_multiply(&u, &t, &m);
		dn_multiply(&t, &u, x);	// t = m*(b-m)*x
		dn_add(&u, &am1, &m2);
		dn_add(&v, a, &m2);
		dn_multiply(&w, &u, &v);	// w = (am1+m2)*(a+m2)
		dn_divide(&aa, &t, &w);	// aa = t/w
		ib_step(&d, &c, &aa);
		dn_multiply(&t, r, &d);
		dn_multiply(r, &t, &c);	// r = r*d*c
		dn_add(&t, a, &m);
		dn_add(&u, &apb, &m);
		dn_multiply(&w, &t, &u);
		dn_multiply(&t, &w, x);
		dn_minus(&w, &t);		// w = -(a+m)*(apb+m)*x
		dn_add(&t, a, &m2);
		dn_add(&u, &ap1, &m2);
		dn_multiply(&v, &t, &u);	// v = (a+m2)*(ap1+m2)
		dn_divide(&aa, &w, &v);	// aa = w/v
		ib_step(&d, &c, &aa);
		dn_multiply(&v, &d, &c);
		dn_multiply(r, r, &v);	// r *= d*c
		if (dn_eq(&oldr, r))
			break;
		busy();
	}
}

/* Regularised incomplete beta function Ix(a, b)
 */
decNumber *betai(decNumber *r, const decNumber *b, const decNumber *a, const decNumber *x) {
	decNumber t, u, v, w, y;
	int limit = 0;

	dn_compare(&t, &const_1, x);
	if (decNumberIsNegative(x) || decNumberIsNegative(&t)) {
		return set_NaN(r);
	}
	if (dn_eq0(x) || dn_eq0(&t))
		limit = 1;
	else {
		decNumberLnBeta(&u, a, b);
		dn_ln(&v, x);			// v = ln(x)
		dn_multiply(&t, a, &v);
		dn_subtract(&v, &t, &u);	// v = lng(...)+a.ln(x)
		dn_1m(&y, x);			// y = 1-x
		dn_ln(&u, &y);			// u = ln(1-x)
		dn_multiply(&t, &u, b);
		dn_add(&u, &t, &v);		// u = lng(...)+a.ln(x)+b.ln(1-x)
		dn_exp(&w, &u);
	}
	dn_add(&v, a, b);
	dn_p2(&u, &v);				// u = a+b+2
	dn_p1(&t, a);				// t = a+1
	dn_divide(&v, &t, &u);			// u = (a+1)/(a+b+2)
	if (dn_lt(x, &v)) {
		if (limit)
			return decNumberZero(r);
		betacf(&t, a, b, x);
		dn_divide(&u, &t, a);
		return dn_multiply(r, &w, &u);
	} else {
		if (limit)
			return dn_1(r);
		betacf(&t, b, a, &y);
		dn_divide(&u, &t, b);
		dn_multiply(&t, &w, &u);
		return dn_1m(r, &t);
	}
}


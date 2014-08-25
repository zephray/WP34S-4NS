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

#include "matrix.h"
#include "decn.h"
#include "consts.h"
#include "decimal128.h"

#define MAX_DIMENSION	100
#define MAX_SQUARE	10

static int matrix_idx(int row, int col, int ncols) {
	return col + row * ncols;
}
		
static void matrix_get(decNumber *r, const decimal64 *base, int row, int col, int ncols) {
	decimal64ToNumber(base + matrix_idx(row, col, ncols), r);
}

/* Check if a matrix fits into the available registers or not.
 * Raise an error if not.
 */
static int matrix_range_check(int base, int rows, int cols) {
	int limit = NumRegs;
	int s;

	if (is_intmode() || is_dblmode()) {
		err(ERR_BAD_MODE);
		return 0;
	}
	if (base >= LOCAL_REG_BASE && LocalRegs < 0) {
		base -= LOCAL_REG_BASE;
		limit = local_regs();
	}
	s = rows * cols;
	if (base < 0 || base + s > limit || s > MAX_DIMENSION) {
		err(ERR_RANGE);
		return 0;
	}
	return 1;
}

/* Build a matrix descriptor from the base, rows and columns.
 */
static int matrix_descriptor(decNumber *r, int base, int rows, int cols) {
	decNumber z;

	if (! matrix_range_check(base, rows, cols))
		return 0;
	int_to_dn(&z, (base * 100 + rows) * 100 + cols);
	dn_mulpow10(r, &z, -4);
	return 1;
}

/* Take a matrix descriptor and return the base register number.
 * Optionally return the number of rows and columns in the matrix.
 * Optionally return the sign of the initial descriptor as well.
 */
static int matrix_decompose(const decNumber *x, int *rows, int *cols, int *up) {
	decNumber ax, y;
	unsigned int n, base;
	int r, c, u;

	if (decNumberIsNegative(x)) {
		dn_abs(&ax, x);
		x = &ax;
		u = 0;
	} else
		u = 1;
	if (up)		*up = u;

	dn_mulpow10(&y, x, 4);
	n = dn_to_int(&y);
	base = n / 10000;
	c = n % 100;
	r = (n / 100) % 100;
	if (c == 0)
		c = r;
	if (! matrix_range_check(base, r, c))
		return -1;
	if (c == 0) {
		err(ERR_BAD_PARAM);
		return -1;
	}
	if (rows)	*rows = r;
	if (cols)	*cols = c;
	return base;
}

/* Decompose a matrix descriptor and return a pointer to its first
 * element.  Optionally return the number of rows and columns.
 */
static decimal64 *matrix_decomp(const decNumber *x, int *rows, int *cols) {
	const int base = matrix_decompose(x, rows, cols, NULL);

	if (base < 0)
		return NULL;
	return &(get_reg_n(base)->s);
}

/* Check if a matrix is square or not.
 */
void matrix_is_square(enum nilop op) {
	int r, c;
	decNumber x;

	getX(&x);
	if (matrix_decompose(&x, &r, &c, NULL) < 0)
		return;
	fin_tst(r != c);
}

#ifdef SILLY_MATRIX_SUPPORT
/* Create either a zero matrix or an identity matrix.
 */
void matrix_create(enum nilop op) {
	decNumber x;
	int r, c, i, j;
	decimal64 *base;
	const decimal64 *diag, *off;

	getX(&x);
	base = matrix_decomp(&x, &r, &c);
	if (base != NULL) {
		off = get_const(OP_ZERO, 0)->s;

		if (op == OP_MAT_IDENT) {
			if (r != c) {
				err(ERR_MATRIX_DIM);
				return;
			}
			diag = get_const(OP_ONE, 0)->s;
		} else
			diag = off;

		for (i=0; i<r; i++)
			for (j=0; j<c; j++)
				*base++ = *((i==j)?diag:off);
	}
}
#endif

/* Matrix copy
 */
decNumber *matrix_copy(decNumber *r, const decNumber *y, const decNumber *x) {
	decimal64 *src;
	int rows, cols, d;

	src = matrix_decomp(y, &rows, &cols);
	if (src == NULL)
		return NULL;

	d = dn_to_int(x);
	if (matrix_descriptor(r, d, rows, cols) == 0)
		return NULL;
	xcopy(get_reg_n(d), src, rows * cols * sizeof(decimal64));
	return r;
}


static decNumber *matrix_do_loop(decNumber *r, int low, int high, int step, int up) {
	decNumber z;
	int i;

	if (up) {
		i = (low * 1000 + high) * 100 + step;
	} else {
		i = (high * 1000 + low) * 100 + step;
	}
	int_to_dn(&z, i);
	dn_mulpow10(r, &z, -5);
	return r;
}

decNumber *matrix_all(decNumber *r, const decNumber *x) {
	int rows, cols, base, up;

	base = matrix_decompose(x, &rows, &cols, &up);
	if (base < 0)
		return NULL;
	return matrix_do_loop(r, base, base+rows*cols-1, 1, up);
}

decNumber *matrix_diag(decNumber *r, const decNumber *x) {
	int rows, cols, base, up, n;

	base = matrix_decompose(x, &rows, &cols, &up);
	if (base < 0)
		return NULL;
	n = ((rows < cols) ? rows : cols) - 1;
	cols++;
	return matrix_do_loop(r, base, base+n*cols, cols, up);
}

decNumber *matrix_row(decNumber *r, const decNumber *y, const decNumber *x) {
	int rows, cols, base, up, n;

	base = matrix_decompose(x, &rows, &cols, &up);
	if (base < 0)
		return NULL;
	n = dn_to_int(y) - 1;
	if (n < 0 || n >= rows) {
		err(ERR_RANGE);
		return NULL;
	}
	base += n*cols;
	return matrix_do_loop(r, base, base + cols - 1, 1, up);
}

decNumber *matrix_col(decNumber *r, const decNumber *y, const decNumber *x) {
	int rows, cols, base, up, n;

	base = matrix_decompose(x, &rows, &cols, &up);
	if (base < 0)
		return NULL;
	n = dn_to_int(y) - 1;
	if (n < 0 || n >= cols) {
		err(ERR_RANGE);
		return NULL;
	}
	base += n;
	return matrix_do_loop(r, base, base + cols * (rows-1), cols, up);
}

decNumber *matrix_rowq(decNumber *r, const decNumber *x) {
	int rows;

	if (matrix_decompose(x, &rows, NULL, NULL) < 0)
		return NULL;
	int_to_dn(r, rows);
	return r;
}

decNumber *matrix_colq(decNumber *r, const decNumber *x) {
	int cols;

	if (matrix_decompose(x, NULL, &cols, NULL) < 0)
		return NULL;
	int_to_dn(r, cols);
	return r;
}

decNumber *matrix_getreg(decNumber *r, const decNumber *cdn, const decNumber *rdn, const decNumber *m) {
	int h, w, ri, ci;
	int n = matrix_decompose(m, &h, &w, NULL);

	if (n < 0)
		return NULL;
	ri = dn_to_int(rdn) - 1;
	ci = dn_to_int(cdn) - 1;
	if (ri < 0 || ci < 0 || ri >= h || ci >= w) {
		err(ERR_RANGE);
		return NULL;
	}
	n += matrix_idx(ri, ci, w);
	int_to_dn(r, n);
	return r;
}

decNumber *matrix_getrc(decNumber *res, const decNumber *m) {
	decNumber ydn;
	int rows, cols, c, r, pos;
	int n = matrix_decompose(m, &rows, &cols, NULL);

	if (n < 0)
		return NULL;
	getY(&ydn);
	pos = dn_to_int(&ydn);
	pos -= n;
	if (pos < 0 || pos >= rows*cols) {
		err(ERR_RANGE);
		return NULL;
	}
	c = pos % cols + 1;
	r = pos / cols + 1;
	int_to_dn(res, r);
	int_to_dn(&ydn, c);
	setY(&ydn);
	return res;
}

// a = a + b * k -- generalised matrix add and subtract
decNumber *matrix_genadd(decNumber *r, const decNumber *k, const decNumber *b, const decNumber *a) {
	int arows, acols, brows, bcols;
	decNumber s, t, u;
	int i;

	decimal64 *abase = matrix_decomp(a, &arows, &acols);
	decimal64 *bbase = matrix_decomp(b, &brows, &bcols);
	if (abase == NULL || bbase == NULL)
		return NULL;
	if (arows != brows || acols != bcols) {
		err(ERR_MATRIX_DIM);
		return NULL;
	}
	for (i=0; i<arows*acols; i++) {
		decimal64ToNumber(bbase + i, &s);
		dn_multiply(&t, &s, k);
		decimal64ToNumber(abase + i, &s);
		dn_add(&u, &t, &s);
		packed_from_number(abase + i, &u);
	}
	return decNumberCopy(r, a);
}


// Matrix multiply c = a * b, c can be a or b or overlap either
decNumber *matrix_multiply(decNumber *r, const decNumber *a, const decNumber *b, const decNumber *c) {
	int arows, acols, brows, bcols;
	decNumber sum, s, t, u;
	int creg;
	int i, j, k;
	decimal64 result[MAX_DIMENSION];
	decimal64 *rp = result;
	decimal64 *abase = matrix_decomp(a, &arows, &acols);
	decimal64 *bbase = matrix_decomp(b, &brows, &bcols);

	if (abase == NULL || bbase == NULL)
		return NULL;
	if (acols != brows) {
		err(ERR_MATRIX_DIM);
		return NULL;
	}
	creg = dn_to_int(c);
	if (matrix_descriptor(r, creg, arows, bcols) == 0)
		return NULL;

        busy();
	for (i=0; i<arows; i++)
		for (j=0; j<bcols; j++) {
			decNumberZero(&sum);
			for (k=0; k<acols; k++) {
				matrix_get(&s, abase, i, k, acols);
				matrix_get(&t, bbase, k, j, bcols);
				dn_multiply(&u, &s, &t);
				dn_add(&sum, &sum, &u);
			}
			packed_from_number(rp++, &sum);
		}
	xcopy(get_reg_n(creg), result, sizeof(decimal64) * arows * bcols);
	return r;
}

/* In place matrix transpose using minimal extra storage */
decNumber *matrix_transpose(decNumber *r, const decNumber *m) {
	int w, h, start, next, i;
	int n = matrix_decompose(m, &h, &w, NULL);
	decimal64 *base, tmp;

	if (n < 0)
		return NULL;
	base = &(get_reg_n(n)->s);
	if (base == NULL)
		return NULL;

	for (start=0; start < w*h; start++) {
		next = start;
		i=0;
		do {
			i++;
			next = (next % h) * w + next / h;
		} while (next > start);
		if (next < start || i == 1)
			continue;

		tmp = base[next = start];
		do {
			i = (next % h) * w + next / h;
			base[next] = (i == start) ? tmp : base[i];
			next = i;
		} while (next > start);
	}

	matrix_descriptor(r, n, w, h);
	return r;
}

#ifdef MATRIX_ROWOPS
void matrix_rowops(enum nilop op) {
	decNumber m, ydn, zdn, t;
	decimal64 *base, *r1, *r2;
	int rows, cols;
	int i;

	getXYZT(&m, &ydn, &zdn, &t);
	base = matrix_decomp(&m, &rows, &cols);
	if (base == NULL)
		return;

	i = dn_to_int(&ydn) - 1;
	if (i < 0 || i >= rows) {
badrow:		err(ERR_RANGE);
		return;
	}
	r1 = base + i * cols;

	if (op == OP_MAT_ROW_MUL) {
		for (i=0; i<cols; i++) {
			decimal64ToNumber(r1, &t);
			dn_multiply(&m, &zdn, &t);
			packed_from_number(r1++, &m);
		}
	} else {
		i = dn_to_int(&zdn) - 1;
		if (i < 0 || i >= rows)
			goto badrow;
		r2 = base + i * cols;

		if (op == OP_MAT_ROW_SWAP) {
			for (i=0; i<cols; i++)
				swap_reg((REGISTER *) r1++, (REGISTER *) r2++);
		} else {
			for (i=0; i<cols; i++) {
				decimal64ToNumber(r1, &ydn);
				decimal64ToNumber(r2++, &zdn);
				dn_multiply(&m, &zdn, &t);
				dn_add(&zdn, &ydn, &m);
				packed_from_number(r1++, &zdn);
			}
		}
	}
}
#endif


/* Two little utility routines to convert decimal128s to decNumbers and back and to
 * extract elements form a decimal128 matrix.
 */
static void matrix_get128(decNumber *r, const decimal128 *base, int row, int col, int ncols) {
	decimal128ToNumber(base + matrix_idx(row, col, ncols), r);
}

static void matrix_put128(const decNumber *x, decimal128 *base, int row, int col, int ncols) {
	packed128_from_number(base + matrix_idx(row, col, ncols), x);
}


/* Perform a LU decomposition of the specified matrix in-situ.
 * Return the pivot rows in pivots if not null and return the parity
 * of the number of pivots or zero if the matrix is singular
 */
static int LU_decomposition(decimal128 *A, unsigned char *pivots, const int n) {
	int i, j, k;
	int pvt, spvt = 1;
	decimal128 *p1, *p2;
	decNumber max, t, u;

        busy();
	for (k=0; k<n; k++) {
		/* Find the pivot row */
		pvt = k;
		matrix_get128(&u, A, k, k, n);
		dn_abs(&max, &u);
		for (j=k+1; j<n; j++) {
			matrix_get128(&t, A, j, k, n);
			dn_abs(&u, &t);
			if (dn_gt(&u, &max)) {
				decNumberCopy(&max, &u);
				pvt = j;
			}
		}
		if (pivots != NULL)
			*pivots++ = pvt;

		/* pivot if required */
		if (pvt != k) {
			spvt = -spvt;
			p1 = A + (n * k);
			p2 = A + (n * pvt);
			for (j=0; j<n; j++) {
				decimal128 t = *p1;
				*p1 = *p2;
				*p2 = t;
				p1++;
				p2++;
				//swap_reg(p1++, p2++);
			}
		}

		/* Check for singular */
		matrix_get128(&t, A, k, k, n);
		if (dn_eq0(&t))
			return 0;

		/* Find the lower triangular elements for column k */
		for (i=k+1; i<n; i++) {
			matrix_get128(&t, A, k, k, n);
			matrix_get128(&u, A, i, k, n);
			dn_divide(&max, &u, &t);
			matrix_put128(&max, A, i, k, n);
		}
		/* Update the upper triangular elements */
		for (i=k+1; i<n; i++)
			for (j=k+1; j<n; j++) {
				matrix_get128(&t, A, i, k, n);
				matrix_get128(&u, A, k, j, n);
				dn_multiply(&max, &t, &u);
				matrix_get128(&t, A, i, j, n);
				dn_subtract(&u, &t, &max);
				matrix_put128(&u, A, i, j, n);
			}
	}
	return spvt;
}

/* Solve the linear equation Ax = b.
 * We do this by utilising the LU decomposition passed in in A and solving
 * the linear equation Ly = b for y, where L is the lower diagonal triangular
 * matrix with unity along the diagonal.  Then we solve the linear system
 * Ux = y, where U is the upper triangular matrix.
 */
static void matrix_pivoting_solve(decimal128 *LU, const decimal64 *b[], unsigned char pivot[], decNumber *x, int n) {
	int i, k;
	decNumber r, t;

	/* Solve the first linear equation Ly = b */
	for (k=0; k<n; k++) {
		if (k != pivot[k]) {
			const decimal64 *swap = b[k];
			b[k] = b[pivot[k]];
			b[pivot[k]] = swap;
		}
		decimal64ToNumber(b[k], x + k);
		for (i=0; i<k; i++) {
			matrix_get128(&r, LU, k, i, n);
			dn_multiply(&t, &r, x+i);
			dn_subtract(x+k, x+k, &t);
		}
	}

	/* Solve the second linear equation Ux = y */
	for (k=n-1; k>=0; k--) {
		//if(k != pivot[k]) swap(b[k], b[pivot[k]]);		// undo pivoting from before
		for (i=k+1; i<n; i++) {
			matrix_get128(&r, LU, k, i, n);
			dn_multiply(&t, &r, x+i);
			dn_subtract(x+k, x+k, &t);
		}
		matrix_get128(&r, LU, k, k, n);
#if 0
		/* Check for singular matrix */
		if (dn_eq0(&r))
			return;
#endif
		dn_divide(x+k, x+k, &r);
	}
}

/* Decompose the passed in matrix identifier and extract the matrix from the
 * associated registers into the passed higher precision matrix.  Optionally,
 * return the first register in the matrix and always return the dimensionality.
 * On error, return 0.
 */
static int matrix_lu_check(const decNumber *m, decimal128 *mat, decimal64 **mbase) {
	int rows, cols;
	decimal64 *base;
	decNumber t;
	int i;

	base = matrix_decomp(m, &rows, &cols);
	if (base == NULL)
		return 0;
	if (rows != cols) {
		err(ERR_MATRIX_DIM);
		return 0;
	}
	if (mat != NULL) {
		for (i=0; i<rows*rows; i++) {
			decimal64ToNumber(base+i, &t);
			packed128_from_number(mat+i, &t);
		}
	}
	if (mbase != NULL)
		*mbase = base;
	return rows;
}

/* Calculate the determinant of a matrix by performing the LU decomposition
 * and multiplying the diagonal elements of the upper triangular portion.
 * Also adjust for the parity of the number of pivots.
 */
decNumber *matrix_determinant(decNumber *r, const decNumber *m) {
	int n, i;
	decimal128 mat[MAX_SQUARE*MAX_SQUARE];
	decNumber t;

	n = matrix_lu_check(m, mat, NULL);
	if (n == 0)
		return NULL;

	i = LU_decomposition(mat, NULL, n);

	int_to_dn(r, i);
	for (i=0; i<n; i++) {
		matrix_get128(&t, mat, i, i, n);
		dn_multiply(r, r, &t);
	}
	return r;
}

/* Invert a matrix in situ.
 * Do this by calculating the LU decomposition and solving lots of systems
 * of linear equations.
 */
void matrix_inverse(enum nilop op) {
	decimal128 mat[MAX_SQUARE*MAX_SQUARE];
	decNumber x[MAX_SQUARE];
	unsigned char pivots[MAX_SQUARE];
	int i, j, n;
	decimal64 *base;
	const decimal64 *b[MAX_SQUARE];

	getX(x);
	n = matrix_lu_check(x, mat, &base);
	if (n == 0)
		return;

	i = LU_decomposition(mat, pivots, n);
	if (i == 0) {
		err(ERR_SINGULAR);
		return;
	}

	for (i=0; i<n; i++) {
		for (j=0; j<n; j++)
			b[j] = (i==j) ? (decimal64 *) get_const(OP_ONE, 0) : (decimal64 *) get_const(OP_ZERO, 0);
		matrix_pivoting_solve(mat, b, pivots, x, n);
		for (j=0; j<n; j++)
			packed_from_number(base + matrix_idx(j, i, n), x+j);
	}
}

/* Solve a system of linear equations Ac = b
 */
decNumber *matrix_linear_eqn(decNumber *r, const decNumber *a, const decNumber *b, const decNumber *c) {
	int n, i, brows, bcols, creg;
	decimal128 mat[MAX_SQUARE*MAX_SQUARE];
	decimal64 *bbase, *cbase;
	decNumber cv[MAX_SQUARE];
	unsigned char pivots[MAX_SQUARE];
	const decimal64 *bv[MAX_SQUARE];

	n = matrix_lu_check(a, mat, NULL);
	if (n == 0)
		return NULL;

	bbase = matrix_decomp(b, &brows, &bcols);
	if (bbase == NULL)
		return NULL;
	if (brows != n || bcols != 1) {
		err(ERR_MATRIX_DIM);
		return NULL;
	}

	creg = dn_to_int(c);
	if (matrix_descriptor(r, creg, n, 1) == 0)
		return NULL;
	cbase = &(get_reg_n(creg)->s);

	/* Everything is happy so far -- decompose */
	i = LU_decomposition(mat, pivots, n);
	if (i == 0) {
		err(ERR_SINGULAR);
		return NULL;
	}

	/* And solve */
	for (i=0; i<n; i++)
		bv[i] = bbase + i;
	matrix_pivoting_solve(mat, bv, pivots, cv, n);
	for (i=0; i<n; i++)
		packed_from_number(cbase+i, cv+i);
	return r;
}

#ifdef MATRIX_LU_DECOMP
/* Perform an in-situ LU decomposition of a user's matrix.
 * Return the pivot descriptor.
 */
decNumber *matrix_lu_decomp(decNumber *r, const decNumber *m) {
	unsigned char pivots[MAX_SQUARE];
	int i, sign, n;
	decNumber t, u;
	decimal128 mat[MAX_SQUARE*MAX_SQUARE];
	decimal64 *base;

	n = matrix_lu_check(m, mat, &base);
	if (n == 0)
		return NULL;

	sign = LU_decomposition(mat, pivots, n);
	if (sign == 0) {
		err(ERR_SINGULAR);
		return NULL;
	}

	/* Build the pivot number */
	decNumberZero(r);
	for (i=0; i<n; i++) {
		int_to_dn(&t, pivots[i]);
		dn_mulpow10(&u, r, 1);
		dn_add(r, &u, &t);
	}

	/* Copy the result back over the matrix */
	for (i=0; i<n*n; i++) {
		packed_from_packed128(base+i, mat+i);
	}
	return r;
}
#endif

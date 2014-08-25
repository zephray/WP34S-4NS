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

#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "xeq.h"

extern decNumber *matrix_all(decNumber *r, const decNumber *x);
extern decNumber *matrix_diag(decNumber *r, const decNumber *x);
extern decNumber *matrix_rowq(decNumber *r, const decNumber *x);
extern decNumber *matrix_colq(decNumber *r, const decNumber *x);
extern decNumber *matrix_row(decNumber *r, const decNumber *y, const decNumber *x);
extern decNumber *matrix_col(decNumber *r, const decNumber *y, const decNumber *x);
extern decNumber *matrix_genadd(decNumber *r, const decNumber *k, const decNumber *b, const decNumber *a);
extern decNumber *matrix_multiply(decNumber *r, const decNumber *a, const decNumber *b, const decNumber *c);
extern decNumber *matrix_transpose(decNumber *r, const decNumber *m);
extern decNumber *matrix_getreg(decNumber *r, const decNumber *k, const decNumber *b, const decNumber *a);
extern decNumber *matrix_getrc(decNumber *r, const decNumber *x);
extern void matrix_rowops(enum nilop op);
extern void matrix_is_square(enum nilop op);
extern void matrix_create(enum nilop op);
extern decNumber *matrix_copy(decNumber *r, const decNumber *y, const decNumber *x);

extern decNumber *matrix_determinant(decNumber *r, const decNumber *x);
extern decNumber *matrix_lu_decomp(decNumber *r, const decNumber *x);
extern void matrix_inverse(enum nilop op);
extern decNumber *matrix_linear_eqn(decNumber *r, const decNumber *a, const decNumber *b, const decNumber *c);

#endif

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

#ifndef __STATS_H__
#define __STATS_H__

/*
 *  Define register block
 */
typedef struct _stat_data {
	// The next four are higher precision
	decimal128 sX2Y;
	decimal128 sX2;		
	decimal128 sY2;		
	decimal128 sXY;

	decimal64 sX;		
	decimal64 sY;		
	decimal64 slnX;		
	decimal64 slnXlnX;	
	decimal64 slnY;		
	decimal64 slnYlnY;	
	decimal64 slnXlnY;	
	decimal64 sXlnY;	
	decimal64 sYlnX;

	signed int sN;		
} STAT_DATA;

extern STAT_DATA *StatRegs;

extern int  sigmaCheck(void);
extern void sigmaDeallocate(void);
extern int  sigmaCopy(void *source);
extern void sigma_clear(enum nilop);
extern int sigma_plus_x(const decNumber*);
extern void sigma_plus(void);
extern void sigma_minus(void);

extern void stats_mean(enum nilop);
extern void stats_wmean(enum nilop);
extern void stats_gmean(enum nilop);
extern void stats_deviations(enum nilop);
extern void stats_wdeviations(enum nilop);
extern decNumber *stats_xhat(decNumber *, const decNumber *);
extern decNumber *stats_yhat(decNumber *, const decNumber *);
extern void stats_correlation(enum nilop);
extern void stats_COV(enum nilop);
extern void stats_LR(enum nilop);
extern void stats_SErr(enum nilop);
extern decNumber *stats_sigper(decNumber *, const decNumber *);

extern void sigma_val(enum nilop);

extern void sigma_sum(enum nilop);

extern void stats_mode(enum nilop op);

extern void stats_random(enum nilop);
extern void stats_sto_random(enum nilop);

extern decNumber *betai(decNumber *, const decNumber *, const decNumber *, const decNumber *);
extern decNumber *pdf_Q(decNumber *q, const decNumber *x);
extern void cdf_Q_helper(enum nilop op);

#endif

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

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "decn.h"

/* STO/RCL modifiers */
#define SR_OP_NONE	0
#define SR_OP_SWAP	1
#define SR_OP_ADD	2
#define SR_OP_SUB	3
#define SR_OP_MUL	4
#define SR_OP_DIV	5
#define SR_OP_POW	6


#define COMPLEX_PREFIX	'\024'

extern int setuptty(int reset);
extern void display(void);
extern void frozen_display(void);
extern void error_message(const unsigned int e);
extern void message(const char *str1, const char *str2);

extern void format_reg(int index, char *buf);
extern void dot(int n, int on);
extern void set_IO_annunciator(void);
extern void set_RPN_annunciator(int on);

#ifndef REALBUILD
extern int getdig(int ch);
extern char forceDispPlot;
#endif
#ifdef INCLUDE_STOPWATCH
extern void stopwatch_message(const char *str1, const char *str2, int force_small, char* exponent);
#endif // INCLUDE_STOPWATCH

extern unsigned int charlengths(unsigned int);
extern void findlengths(unsigned short int posns[257], int smallp);
extern void unpackchar(unsigned int c, unsigned char d[6], int smallp, const unsigned short int posns[257]);
extern int pixel_length( const char *, int smallp );

extern void set_x_dn(decNumber *z, char *res, int *display_digits);

#endif

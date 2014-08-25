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

#ifndef __ALPHA_H__
#define __ALPHA_H__

#include "xeq.h"

#define CHARS_IN_REG	6

extern void clralpha(enum nilop);
extern void cmdalpha(unsigned int, enum rarg);
extern void alpha_ip(unsigned int, enum rarg);
extern void alpha_length(enum nilop);
extern void alpha_shift_l(unsigned int, enum rarg);
extern void alpha_shift_r(unsigned int, enum rarg);
extern void alpha_rot_r(unsigned int, enum rarg);

extern void alpha_view(enum nilop);
extern void alpha_view_reg(unsigned int arg, enum rarg op);
extern void alpha_view_common(int reg);

extern void alpha_tox(enum nilop);
extern void alpha_fromx(enum nilop);
extern void alpha_sto(unsigned int, enum rarg);
extern void alpha_rcl(unsigned int, enum rarg);
extern char *alpha_rcl_s(int index, char buf[12]);

extern void multialpha(opcode, enum multiops);

extern void alpha_reg(unsigned int, enum rarg);
extern void alpha_onoff(enum nilop);


extern void add_string(const char *);

#endif

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

#ifndef __XROM_H__
#define __XROM_H__

#include "xeq.h"
#ifndef COMPILE_CATALOGUES
#include "xrom_labels.h"
#endif

#ifdef REALBUILD
#define XROM_DATA __attribute__((section(".xrom")))
#else
#define XROM_DATA /**/
#endif

extern const s_opcode xrom[];
extern const unsigned short int xrom_size;

#endif

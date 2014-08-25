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

#ifndef __LCD_H__
#define __LCD_H__
#include "xeq.h"

#if defined(QTGUI) || defined(IOS)
#include <stdint.h>
#endif

extern int setuptty(int reset);
extern void set_dot(int n);
extern void clr_dot(int n);
extern int is_dot(int n);
extern void set_status_grob(unsigned long long int grob[6]);
extern void show_disp(void);
extern void wait_for_display(void);
extern void finish_display(void);
extern void show_flags(void);
extern void reset_disp(void);
extern void show_progtrace(char *buf);
extern void show_stack(void);

#define MANT_SIGN	129
#define EXP_SIGN	130
#define BIG_EQ		131
#define LIT_EQ		132
#define DOWN_ARR	133
#define INPUT		134
#define BATTERY		135
#define BEG		136
#define STO_annun	137
#define RCL_annun	138
#define RAD		139
#define DEG		140
#define RPN		141
#define MATRIX_BASE	142

#define SEGS_PER_DIGIT		9
#define SEGS_PER_EXP_DIGIT	7

#define SEGS_EXP_BASE		(DISPLAY_DIGITS*SEGS_PER_DIGIT)

#define BITMAP_WIDTH		43

#if defined(CONSOLE) && !defined(NOCURSES)
#ifdef USECURSES
#include <curses.h>
#define GETCHAR	getch
#define GETCHAR_ERR	ERR
#define PRINTF	printw
#define MOVE(x, y)	move(y, x)
#else
#define GETCHAR getchar
#define GETCHAR_ERR	EOF
#define PRINTF	printf
#define MOVE(x, y)

#ifdef USETERMIO
#include <termio.h>
#else
#include <termios.h>
#endif
#endif
#endif

#ifdef NSPIRE
extern unsigned char showpage;
#endif

#if defined(WINGUI)
// The symbols are declared in VirtualLCD.cpp
extern void WindowsBlink(int a);
extern void WindowsSwapBuffers();
#ifdef _WINDLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif
extern unsigned int LcdData[20];
#undef AT91C_SLCDC_MEM
#define AT91C_SLCDC_MEM LcdData
#define Lcd_Enable()
#endif

#if defined(QTGUI) || defined(IOS)
extern uint64_t LcdData[10];
#undef AT91C_SLCDC_MEM
#define AT91C_SLCDC_MEM ((uint32_t*) LcdData)
#define Lcd_Enable()
#endif

#endif

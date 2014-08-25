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

#include "features.h"
#include "xeq.h" 
#include "storage.h"
#include "display.h"
#include "lcd.h"
#include "int.h"
#include "consts.h"
#include "alpha.h"
#include "stats.h"
#include "decn.h"
#include "revision.h"
//#include "printer.h"
#include "serial.h"

static enum separator_modes { SEP_NONE, SEP_COMMA, SEP_DOT } SeparatorMode;
static enum decimal_modes { DECIMAL_DOT, DECIMAL_COMMA } DecimalMode;

static void set_status_sized(const char *, int);
static void set_status(const char *);
static void set_status_right(const char *);
static void set_status_graphic(const unsigned char *);

const char *DispMsg;	   // What to display in message area
short int DispPlot;
#ifndef REALBUILD
char LastDisplayedText[NUMALPHA + 1];	   // For clipboard export
char LastDisplayedNumber[NUMBER_LENGTH + 1];
char LastDisplayedExponent[EXPONENT_LENGTH + 1];
char forceDispPlot;
#endif

FLAG ShowRPN;		   // controls visibility of RPN annunciator
FLAG JustDisplayed;	   // Avoid duplicate calls to display()
SMALL_INT IntMaxWindow;    // Number of windows for integer display
FLAG IoAnnunciator;	   // Status of the little "=" sign

/* Message strings
 * Strings starting S7_ are for the lower 7 segment line.  Strings starting S_
 * are for the upper dot matrix line.
 */
static const char S_SURE[] = "Sure?";

static const char S7_ERROR[] = "Error";		/* Default lower line error display */
static const char S7_NaN[] = "not nuMmEric";	/* Displaying NaN in lower line */
#ifndef REALBUILD
static const char S7_NaN_Text[] = " N o t   n u m e r i c ";
#endif
static const char S7_INF[] = "Infinity";	/* Displaying infinity in lower line */
#ifndef REALBUILD
static const char S7_INF_Text[] = " I n f i n i t y ";
static const char S7_NEG_INF_Text[] = "-I n f i n i t y ";
#endif

static const char S7_STEP[] = "StEP ";		/* Step marker in program mode (lower line) */
#ifndef REALBUILD
static const char S7_STEP_ShortText[] = "STEP";
#endif

static const char S7_fract_EQ[] = " = ";	/* Exponent in fraction mode indicates low, equal or high */
static const char S7_fract_LT[] = " Lt";
static const char S7_fract_GT[] = " Gt";

static const char libname[][5] = {
	"rAMm", "Lib ", "Bup ",
#ifndef REALBUILD
	"roMm"
#endif
};

#ifndef REALBUILD
static const char libname_text[][10] = {
	" R a m ", " L i b ", " B u p ",	" R o m "
};
static const char libname_shorttext[][5] = {
	"Ram", "Lib", "Bup", "Rom"
};
#endif


/* Set the separator and decimal mode globals
 */
static void set_separator_decimal_modes(void) {
	// Separators used by various modes
	if (UState.fraccomma) {
		SeparatorMode = SEP_DOT;
		DecimalMode = DECIMAL_COMMA;
	}
	else {
		SeparatorMode = SEP_COMMA;
		DecimalMode = DECIMAL_DOT;
	}
	if ((UState.intm && UState.nointseparator) || (!UState.intm && UState.nothousands))
		SeparatorMode = SEP_NONE;
}


/* Table of error messages.
 * These consist of a double string.  The first is displayed in the
 * top line, the second in the bottom.  If the second is empty, "Error"
 * is displayed instead.  To get a blank lower line, include a space.
 */
void error_message(const unsigned int e) 
{
#define MSG1(top) top "\0" 
#define MSG2(top,bottom) top "\0" bottom

	// NB: this MUST be in the same order as the error #defines in errors.h
	static const char *const error_table[] = 
	{
		// manually get the order correct!
		MSG2("Running", "ProGraMm"),
		MSG1("Domain"),
		MSG2("Bad time", "or dAtE"),
		MSG2("Undefined", "Op-COdE"),
		MSG1("+\237"),
		MSG1("-\237"),
		MSG2("No such", "LAbEL"),
		MSG2("Illegal", "OPErAtion"),
		MSG1("Out of range"),
		MSG1("Bad digit"),
		MSG1("Too long"),
		MSG2("RAM is", "FuLL"),
		MSG2("Stack", "CLASH"),
		MSG1("Bad mode"),
		MSG2("Word\006\006\006size", "too SMmALL"),
		MSG2("Too few", "dAtA PointS"),
		MSG2("Invalid", "ParaMmEtEr"),
		MSG1("I/O"),
		MSG2("Invalid", "dAtA"),
		MSG2("Write", "ProtEctEd"),
		MSG2("No root", "Found"),
		MSG2("Matrix", "MmISMmAtCH"),
		MSG1("Singular"),
		MSG2("Flash is", "FuLL"),
		MSG2("No crystal", "InStaLLEd"),
		MSG2("\004 \035", "X"),		// Integral ~
	};
#undef MSG1
#undef MSG2
#ifndef REALBUILD
	static const char *const error_table_text[] =
	{
		" P r o g r a m ",
		"",
		" o r   d a t e ",
		" O p - c o d e ",
		"",
		"",
		" L a b e l ",
		" O p e r a t i o n ",
		"",
		"",
		"",
		" F u l l ",
		" C l a s h ",
		"",
		" T o o   s m a l l ",
		" D a t a   p o i n t s ",
		" P a r a m e t e r ",
		"",
		" D a t a ",
		" P r o t e c t e d ",
		" F o u n d ",
		" M i s m a t c h ",
		"",
		" F u l l ",
		" I n s t a l l e d ",
		"",
	};
#endif

	if (e != ERR_NONE || Running) {
		const char *p = error_table[e];
		const char *q = find_char(p, '\0') + 1;
		if (*q == '\0')
			q = S7_ERROR;
		if (*q == 'X') {
			DispMsg = p;
			frozen_display();
		}
		else {
			message(p, q);
			State2.disp_freeze = (e != ERR_NONE);
#ifndef REALBUILD
			scopy(LastDisplayedNumber, error_table_text[e]);
#endif
		}
#ifdef INFRARED
		if (Tracing) {
			if (*q == 'X')
				print_reg(regX_idx, p, 0);
			else {
				print_tab(0);
				print_line(p, 0);
				print(' ');
				while (*q != '\0') {
					int c = *q;
					if (c >= 'A')
						c |= 0x60; // ASCII lower case
					print(c);
					if (c == 'm' /* || c == 'w' */)
						++q;
					++q;
				}
				print_advance( 0 );
			}
		}
#endif
	}
}


/* Define a limited character set for the 7-segment portion of the
 * display.
 */
#define D_TOP 64
#define D_TL 32
#define D_TR 8
#define D_MIDDLE 16
#define D_BL 4
#define D_BR 1
#define D_BOTTOM 2

#include "charset7.h"

#ifndef REALBUILD
#define SET_MANT_SIGN set_mant_sign_dot()
#define CLR_MANT_SIGN clr_mant_sign_dot()
#define SET_EXP_SIGN set_exp_sign_dot()
#define CLR_EXP_SIGN clr_exp_sign_dot()

static void set_mant_sign_dot()
{
	LastDisplayedNumber[0]='-';
	set_dot(MANT_SIGN);
}

static void clr_mant_sign_dot()
{
	LastDisplayedNumber[0]=' ';
	clr_dot(MANT_SIGN);
}

static void set_exp_sign_dot()
{
	LastDisplayedExponent[0]='-';
	set_dot(EXP_SIGN);
}

static void clr_exp_sign_dot()
{
	LastDisplayedExponent[0]=' ';
	clr_dot(EXP_SIGN);
}

#else
#define SET_MANT_SIGN set_dot(MANT_SIGN)
#define CLR_MANT_SIGN clr_dot(MANT_SIGN)

#define SET_EXP_SIGN set_dot(EXP_SIGN)
#define CLR_EXP_SIGN clr_dot(EXP_SIGN)

#endif

#ifndef REALBUILD
int getdig(int ch)
#else
static int getdig(int ch)
#endif
{
	// perform index lookup
	return digtbl[ch&0xff];
}

void dot(int n, int on) {
	if (on)	set_dot(n);
	else	clr_dot(n);
}


/* Set the decimal point *after* the indicated digit
 * The marker can be either a comma or a dot depending on the value
 * of decimal.
 */
static char *set_decimal(const int posn, const enum decimal_modes decimal, char *res) {
	if (res) {
		*res++ = (decimal == DECIMAL_DOT)?'.':',';
	} else {
		set_dot(posn+7);
		if (decimal != DECIMAL_DOT)
			set_dot(posn+8);
#ifndef REALBUILD
	LastDisplayedNumber[(posn/9)*2+2]= decimal == DECIMAL_DOT?'.':',';
#endif
	}
	return res;
}

/* Set the digit group separator *before* the specified digit.
 * This can be nothing, a comma or a dot depending on the state of the
 * sep argument.
 */
static char *set_separator(int posn, const enum separator_modes sep, char *res) {
	if (sep == SEP_NONE)
		return res;
	if (res) {
		if (sep == SEP_COMMA) *res++ = ',';
		else *res++ = '.';
	} else {
		posn -= SEGS_PER_DIGIT;
		set_dot(posn+7);
		if (sep == SEP_COMMA)
			set_dot(posn+8);
#ifndef REALBUILD
		LastDisplayedNumber[(posn/9)*2+2] = sep == SEP_COMMA?',':'.';
#endif
	}
	return res;
}

/* Set a digit in positions [base, base+6] */
static void set_dig(int base, int ch)
{
	int i;
	int c = getdig(ch);
#ifndef REALBUILD
	if(base<SEGS_EXP_BASE)
		LastDisplayedNumber[(base/9)*2+1] = ch==0?' ':ch;
	else
		LastDisplayedExponent[(base-SEGS_EXP_BASE)/7+1] = ch;
#endif
	for (i=6; i>=0; i--)
	{
//		dot(base, c & (1 << i));
		if (c & (1 << i))
			set_dot(base);
		else
			clr_dot(base);
		base++;
	}
}

static char *set_dig_s(int base, int ch, char *res) {
	if (res) *res++ = ch;
	else	set_dig(base, ch);
	return res;
}


static void set_digits_string(const char *msg, int j) {
	for (; *msg != '\0'; msg++) {
		if (*msg == '.' || *msg == ',')
			set_decimal(j - SEGS_PER_DIGIT, *msg == '.' ? DECIMAL_DOT : DECIMAL_COMMA, CNULL);
		else {
			set_dig_s(j, *msg, CNULL);
			j += SEGS_PER_DIGIT;
		}
	}
}

static void set_exp_digits_string(const char *msg, char *res) {
	int i;
	const int n = res == NULL ? 3 : 4;

	for (i=0; i<n && msg[i] != '\0'; i++)
		res = set_dig_s(SEGS_EXP_BASE + i * SEGS_PER_EXP_DIGIT, msg[i], res);
}

/* Force the exponent display */
static void set_exp(int exp, int zerop, char *res) {
	char buf[6];

#if defined(INCLUDE_YREG_CODE)
	if (res) { // this code chooses different exponent separators depending on the number of digits to squash more in!
		if ( exp < -999 ) {
//			No exponent separator for large -ve exponents;
		}
		else if ( exp > 999 ) {
			*res++ = ':'; // separator for large +ve exponents;
		}
		else {
			*res++ = 'e'; // normal separator;
		}
	}
#else
	if (res) *res++ = 'e';
#endif
	if (exp < 0) {
		if (res) *res++ = '-';
		else SET_EXP_SIGN;
		exp = -exp;
	}
	if (res == NULL && exp > 999)
		scopy(buf, "HIG");
	else {
		xset(buf, '\0', sizeof(buf));
		if (zerop)
			num_arg_0(buf, exp, 3);
		else
			num_arg(buf, exp);
	}
	set_exp_digits_string(buf, res);
}

static void carry_overflow(void) {
	const int base = SEGS_EXP_BASE;
	int c;
	unsigned int b;

	// Figure out the base
	switch (State2.smode) {
	case SDISP_BIN:	b = 2;		break;
	case SDISP_OCT:	b = 8;		break;
	case SDISP_DEC:	b = 10;		break;
	case SDISP_HEX:	b = 16;		break;
	default:	b = UState.int_base+1;	break;
	}

	// Display the base as the first exponent digit
	if (b > 10 && b < 16)
		SET_EXP_SIGN;
	c = "B34567o9D12345h"[b-2];
	set_dig(base, c);

	// Carry and overflow are the next two exponent digits if they are set
	if (get_carry())
		set_dig(base + SEGS_PER_EXP_DIGIT, 'c');
	if (get_overflow())
		set_dig(base + 2*SEGS_PER_EXP_DIGIT, 'o');
}

static int set_x_fract(const decNumber *rgx, char *res);
static void set_x_hms(const decNumber *rgx, char *res);
#if !(defined INCLUDE_YREG_CODE && defined INCLUDE_YREG_HMS)
// replace_char() isn't used or implemented unless HMS Y register display is enabled
static void replace_char(char *a, char b, char c) { }
#endif

/* Display the annunicator text line.
 * Care needs to be taken to keep things aligned.
 * Spaces are 5 pixels wide, \006 is a single pixel space.
 */
static void annunciators(void) {
	// We initialize q here to avoid uninitialized error messages by very strict compilers
	char buf[42], *p = buf, *q="";
	int n;
	static const char shift_chars[4] = " \021\022\023";
	const char shift_char = shift_chars[cur_shift()];
	// Constant variables and code branches depending on a constant variable
	// that's set to 0 will be optimized away. This way it's easier to make a
	// feature run-time configurable if needed.
#ifdef INCLUDE_YREG_CODE
	const int yreg_enabled = UState.show_y;
#  ifdef INCLUDE_YREG_HMS
	const int yreg_hms = 1;
#  else
	const int yreg_hms = 0;
#  endif
#  ifdef INCLUDE_YREG_FRACT
	const int yreg_fract = 1;
#  else
	const int yreg_fract = 0;
#  endif
#else
	const int yreg_enabled = 0;
	const int yreg_hms = 0;
	const int yreg_fract = 0;
#endif
#ifdef RP_PREFIX
	const int rp_prefix = 1;
#else
	const int rp_prefix = 0;
	const int RectPolConv = -1; // This variable doesn't exist without RP_PREFIX
#endif
// Indicates whether font escape code is compiled in.
// This variable will always be set at compile time.
#ifdef INCLUDE_FONT_ESCAPE
	const int has_FONT_ESCAPE = 1;
#else
	const int has_FONT_ESCAPE = 0;
#endif

	xset(buf, '\0', sizeof(buf));

	if (is_intmode()) {
#ifdef SHOW_STACK_SIZE
		if (shift_char == ' ') {
			*p++ = '\007';
			*p++ = '\346';
			*p++ = (UState.stack_depth ? ':' : '.');
		}
		else
#endif
		{
			*p++ = shift_char;
			*p++ = '\006';
		}

		switch(int_mode()) {
		default:
		case MODE_2COMP:	q = "2c\006";		break;
		case MODE_UNSIGNED:	q = "un\006";		break;
		case MODE_1COMP:	q = "\0061c\006\006";	break;
		case MODE_SGNMANT:	q = "sm";		break;
		}
		q = scopy(p, q);
		*q++ = '\006';
		p = num_arg_0(q, word_size(), 2);

		if (IntMaxWindow > 0) {
			n = 4 + 2 * (5 - IntMaxWindow);
			if (*q == '1')
				n += 2;
			if (q[1] == '1')
				n += 2;
			while (n-- > 0)
				*p++ = '\006';

			for (n = IntMaxWindow; n >= 0; n--)
				*p++ = State2.window == n ? '|' : '\'';
		}
	}
	else if (!yreg_enabled
#ifdef SHIFT_AND_CMPLX_SUPPRESS_YREG
		 || shift_char != ' ' || State2.cmplx
#endif
		 ) {
// The stack size indicator is displayed on the right if date mode indication is enabled
// because the 'D' in small font doesn't look good next to the date mode indicator.
#if defined SHOW_STACK_SIZE && defined NO_DATEMODE_INDICATION
		if (shift_char == ' ') {
			*p++ = '\007';
			*p++ = '\342';
			*p++ = (UState.stack_depth ? ':' : '.');
			*p++ = '\007';
			*p++ = '\344';
			*p++ = (is_dblmode() ? 'D' : ' ');
		}
		else
#endif
		if (shift_char != ' ' || !is_dblmode()) {
			*p++ = shift_char;
			*p++ = '\006';
		}
		else {
			*p++ = 'D';
		}

		if (State2.cmplx) {
			*p++ = ' ';
			*p = '\024';
			goto skip;
		}
		if (State2.arrow) {
			*p++ = ' ';
			*p = '\015';
			goto skip;
		}

		if (shift_char == ' ' && (State2.wascomplex || (rp_prefix && RectPolConv != 0))) {
			if (State2.wascomplex) {
				q = (has_FONT_ESCAPE ? "\007\207i" : "i\006");
			}
			else if (rp_prefix) {
				if (RectPolConv == 1) {
					q = "\007\306<";
				}
				else {
					q = "\007\306y";
				}
			}
			p = scopy(buf, q);

			goto display_yreg;
		}

		switch (UState.date_mode) {
#ifndef NO_DATEMODE_INDICATION
#if defined(DEFAULT_DATEMODE) && (DEFAULT_DATEMODE != 0)
		case DATE_DMY:	q = "d.my\006\006";	break;
#endif
#if ! defined(DEFAULT_DATEMODE) || (DEFAULT_DATEMODE != 1)
		case DATE_YMD:	q = "y.md\006\006";	break;
#endif
#if ! defined(DEFAULT_DATEMODE) || (DEFAULT_DATEMODE != 2)
		case DATE_MDY:	q = "m.dy\006\006";	break;
#endif
#endif
		default:	q = (has_FONT_ESCAPE ? "\007\225\006" : "    \006");	break; // 21 pixels
		}
		p = scopy(p, q);
#if !defined SHOW_STACK_SIZE || defined NO_DATEMODE_INDICATION
		if (get_trig_mode() == TRIG_GRAD) {
			scopy(p, (has_FONT_ESCAPE ? "\006\006\007\210\007" : "\006\006\007" ));
		}
#else
		p = scopy(p, (get_trig_mode() == TRIG_GRAD ? "\006\006\007\210\007" : "  "));
		*p++ = '\007';
		*p++ = '\342';
		*p =  (UState.stack_depth ? ':' : '.');
#endif
	}
	else { // yreg_enabled
#ifndef SHIFT_AND_CMPLX_SUPPRESS_YREG
		if (State2.cmplx) {
			*p++ = '\007';
			*p++ = '\344';
			*p++ = shift_char;
			q = "\024";
		}
		else if (shift_char != ' ') {
			*p++ = '\007';
			*p++ = '\307';
			*p++ = shift_char;
			goto no_copy;
		}
		else
#endif
		if (State2.wascomplex) {
			q = "\007\207i";
		}
		else if (rp_prefix && RectPolConv == 1) {
			q = "\007\307<";
		}
		else if (rp_prefix && RectPolConv == 2) {
			q = "\007\307y";
		}
#ifdef SHOW_GRADIAN_PREFIX
		else if (get_trig_mode() == TRIG_GRAD) {
			q = "\007\207\007";
		}
#endif
		else {
#ifndef SHOW_STACK_SIZE
			q = (is_dblmode() ? "\007\307D" : "\007\207 ");
#else
			if (is_dblmode()) {
				*p++ = '\007';
				*p++ = '\342';
				*p++ = (UState.stack_depth ? ':' : '.');
				q = "\007\345D";
			}
			else {
				q = (UState.stack_depth ? "\007\347:" : "\007\347.");
			}
#endif
		}
		p = scopy(p, q);
#ifndef SHIFT_AND_CMPLX_SUPPRESS_YREG
	no_copy:
#endif

		if (State2.arrow) {
			scopy(p, "\007\204\006\015");
		} else if (State2.runmode) {
			decNumber y;
display_yreg:
			/* This is a bit convoluted.  ShowRegister is the real portion being shown.  Normally
			 * ShowRegister+1 would contain the complex component, however if the register being
			 * examined is on the stack and there is a command line present, the stack will be lifted
			 * after we execute so we need to show ShowRegister instead.
			 */
			getRegister(&y, (ShowRegister >= regX_idx && ShowRegister < regX_idx + stack_size() && get_cmdline()
					 && !(yreg_enabled && !State2.state_lift) // unless stack lift is disabled...
					) ? ShowRegister : ShowRegister+1);
			if ((yreg_hms || yreg_fract) && !decNumberIsSpecial(&y)) {
				if (yreg_hms && State2.hms) {
					const int saved_nothousands = UState.nothousands;

					xset(buf, '\0', sizeof(buf));
					UState.nothousands = 1;
					set_x_hms(&y, buf); // no prefix or alignment for HMS display
					UState.nothousands = saved_nothousands;
					// First replace the '@' character with the degree symbol
					// Then, if the string doesn't fit in the dot matrix display, replace spaces with narrow spaces,
					// then remove the second symbol (") and the overflow or underflow signs,
					// then remove the fractional part of the seconds.
					p = "@\005 \006\"\0.\0";
					while (*p) {
						replace_char(buf, p[0], p[1]);
						if (pixel_length(buf, 1) <= BITMAP_WIDTH + 1) {
							goto skip;
						}
						p += 2;
					}
					goto skip;
				}
				if (yreg_fract && UState.fract
#ifndef SHIFT_AND_CMPLX_SUPPRESS_YREG
				    && !State2.cmplx
#endif
#ifdef ANGLES_NOT_SHOWN_AS_FRACTIONS
				    && !(rp_prefix && RectPolConv == 1)
#endif
				    && set_x_fract(&y, p)) {
					char ltgteq;

					q = find_char(buf, '\0') - 2;
					// Replace Lt/Gt/= with </>/= in small font
					ltgteq = *q;
					switch (ltgteq) {
					case 'G':	ltgteq = '>'; break;
					case 'L':	ltgteq = '<'; break;
					}
					scopy(q, "\007\344?");
					q[2] = ltgteq;

					if (pixel_length(buf, 1) <= BITMAP_WIDTH + 1) {
						goto skip;
					}
					q[-1] = '\0'; // Remove </>/= if string doesn't fit in the dot matrix display
					if (pixel_length(buf, 1) <= BITMAP_WIDTH + 1) {
						goto skip;
					}
					xset(p, '\0', sizeof(buf) - (p - buf));
				}
			}
			for (n=DISPLAY_DIGITS; n>1; ) {
				int extra_pixels;

				set_x_dn(&y, p, &n);
				extra_pixels = pixel_length(buf, 1) - (BITMAP_WIDTH + 1);
				if (extra_pixels <= 0)
					break;

				xset(p, '\0', n+10);

				n -= (extra_pixels + 3) / 4; // The maximum width of digits in the small font is 4 pixels.
			}
		}
	}

skip:	set_status(buf);
}

static void disp_x(const char *p) {
	int i;
	int gotdot = -1;
#if defined(PRETTY_FRACTION_ENTRY)
	int twodot = 0; // ND change
	const char *q; // ND change
#endif

	if (*p == '-') {
		SET_MANT_SIGN;
		p++;
	}

	if (is_intmode()) {
		for (i=0; *p != '\0'; p++) {
			set_dig(i, *p);
			i += SEGS_PER_DIGIT;
		}
		carry_overflow();
	} else {
		set_separator_decimal_modes();

#if defined(PRETTY_FRACTION_ENTRY)
		q = p; // ND change; scan ahead to count dots;
		for (i=0; *q != '\0' && *q != 'E'; q++) {
			if (*q == '.') twodot++;
		}
#endif

		for (i=0; *p != '\0' && *p != 'E'; p++) {
		if (*p == '.') {
				if (gotdot == -1) 
					gotdot = i;
#if defined(PRETTY_FRACTION_ENTRY)
					if ( ( *(p+1) == '.' ) || ( i != gotdot ) ) {
						set_dig(i, '/'); // put in a fraction separator
						i += SEGS_PER_DIGIT;
						if ( *(p+1) == '.' ) {
							p++;
						}
					}
					else {
						if ( twodot > 1 ) {
							i += SEGS_PER_DIGIT;
						}
						else {
							set_decimal(i - SEGS_PER_DIGIT, DecimalMode, CNULL);
//							i += SEGS_PER_DIGIT;
						}
					}
						
#else
				if (i > 0)
					set_decimal(i - SEGS_PER_DIGIT, DecimalMode, CNULL);
				else {
					set_dig(i, '0');
					set_decimal(i, DecimalMode, CNULL);
					i += SEGS_PER_DIGIT;	
			}
#endif
		} else {
				set_dig(i, *p);
				i += SEGS_PER_DIGIT;
			}
		}

		/* Implement a floating comma */
		if (gotdot < 0)
			gotdot = i;
		for (;;) {
			gotdot -= 3 * SEGS_PER_DIGIT;
			if (gotdot <= 0)			// MvC: was '<', caused crash
				break;
			set_separator(gotdot, SeparatorMode, CNULL);
		}

		if (*p == 'E') {
			p++;
			if (*p != '\0') {
				if (*p == '-') {
					SET_EXP_SIGN;
					p++;
				}
			}
			set_exp(s_to_i(p), 1, CNULL);
		} 
	}
}

static const char DIGITS[] = "0123456789ABCDEF";

static void set_int_x(const long long int value, char *res) {
	const int ws = word_size();
	unsigned int b;
	long long int vs = value;
	unsigned long long int v;
	char buf[MAX_WORD_SIZE + 1];
	int i, j, k;
	int sign = 0;
	int dig = SEGS_PER_DIGIT * 11;

	switch (State2.smode) {
	case SDISP_BIN:	b = 2;		break;
	case SDISP_OCT:	b = 8;		break;
	case SDISP_DEC:	b = 10;		break;
	case SDISP_HEX:	b = 16;		break;
	default:	b = int_base();	break;
	}

	if (!res) {
		IntMaxWindow = 0;
		carry_overflow();
	}

	if ((0x7f75 & (1 << (b-1))) != 0) {
		v = extract_value(value, &sign);
		if (int_mode() == MODE_2COMP && sign == 1 && v == 0)
			v = value;
		if (v == 0) {
			if (sign)
				set_dig_s(dig-SEGS_PER_DIGIT, '-', res);
			set_dig_s(dig, '0', res);
			return;
		} else
			for (i=0; v != 0; i++) {
				const int r = v % b;
				v /= b;
				buf[i] = DIGITS[r];
			}
	} else {
		// Truncate down to the current word size and then sign extend it back
		if (ws < 64) {
			const long long int mask = (1LL << ws) - 1;
			vs &= mask;
			if (b == 10 && (vs & (1LL << (ws-1))))
				vs |= ~mask;
		}

		if (!UState.leadzero && vs == 0) {
			set_dig_s(dig, '0', res);
			return;
		} else if (!UState.leadzero) {
			v = (unsigned long long int)vs;
			for (i=0; v != 0; i++) {
				const int r = v % b;
				v /= b;
				buf[i] = DIGITS[r];
			}
		} else {
			int n;
			const unsigned int b1 = b >> 1;
			const unsigned int fac = ((b1 & 0xa) != 0) | (((b1 & 0xc) != 0) << 1);
			v = (unsigned long long int)vs;

			n = (ws + fac) / (fac+1);
			for (i=0; i<n; i++) {
				const int r = v % b;
				v /= b;
				buf[i] = DIGITS[r];
			}
		}
	}

	/* At this point i is the number of digits in the output */
	if (res) {
		if (sign) *res++ = '-';
		while (--i >= 0)
			*res++ = buf[i];
	} else {
#if 0
		set_separator_decimal_modes();

		// Allows configuration of digit grouping per base
		static const char grouping[] = 
			{       0x84, 0xb3, 0xb4, 0xb3, 0xb3, 0xb3, 0xb3, 
		      //	   2     3     4     5     6     7     8
		          0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb2 };
		      //     9    10    11    12    13    14    15    16
		const int shift = SeparatorMode == SEP_NONE ? 12 
			        : grouping[b - 2] >> 4;
		const int group = SeparatorMode == SEP_NONE ? 16
				: (grouping[b - 2] & 0xf);
#else
		// Less flexible but shorter
		const int shift = b == 2 ? 8 : 12;
		const int group = (b == 2 || b == 4) ? 4
				: b == 16 ? 2 : 3;
		set_separator_decimal_modes();
#endif
		IntMaxWindow = (i - 1) / shift;
		if ((SMALL_INT) State2.window > IntMaxWindow)
			State2.window = 0;
		buf[i] = '\0';

		j = State2.window * shift;	// digits at a time
		for (k = 0; k < 12; k++)
			if (buf[j + k] == '\0')
				break;
		for (i=0; --k >= 0; i++) {
			int ch = buf[j++];
			if (i >= shift)
				ch -= 030;
			set_dig(dig, ch);
			if ((j % group) == 0 && k != 0)
				set_separator(dig, SeparatorMode, CNULL);
			dig -= SEGS_PER_DIGIT;
		}
		if (sign) {
			if (dig >= 0)
				set_dig(dig, '-');
			else	SET_MANT_SIGN;
		}
	}
}

/* Handle special cases.
 * return non-zero if the number is special.
 */
static int check_special_dn(const decNumber *x, char *res) {
	if (decNumberIsSpecial(x)) {
		if (decNumberIsNaN(x)) {
			if (res) {
				scopy(res, "NaN");
			} else {
				set_digits_string(S7_NaN, 0);
#ifndef REALBUILD
				scopy(LastDisplayedNumber, S7_NaN_Text);
				forceDispPlot=0;
#endif
			}
			return 1;
		} else {
			if (decNumberIsNegative(x)) {
				if (res) *res++ = '-';
				else	set_dig(SEGS_PER_DIGIT, '-');
			}
			if (res)
				*res++ = '\237';
			else {
				set_digits_string(S7_INF, SEGS_PER_DIGIT * 2);
#ifndef REALBUILD
				if (decNumberIsNegative(x)) {
					scopy(LastDisplayedNumber, S7_NEG_INF_Text);
				}
				else {
					scopy(LastDisplayedNumber, S7_INF_Text);
				}
				forceDispPlot=0;
#endif
			}
			return 1;
		}
	}
	return 0;
}


/* Extract the two lowest integral digits from the number
 */
static void hms_step(decNumber *res, decNumber *x, unsigned int *v) {
	decNumber n;

	decNumberMod(&n, x, &const_100);
	*v = dn_to_int(&n);
	dn_mulpow10(&n, x, -2);
	decNumberTrunc(res, &n);
}

static char *hms_render(unsigned int v, char *str, int *jin, int n, int spaces) {
	char b[32];
	int i, j;

	for (i=0; i<n; i++) {
		if (v == 0)
			b[i] = spaces?' ':'0';
		else {
			j = v % 10;
			v /= 10;
			b[i] = j + '0';
		}
	}
	if (b[0] == ' ')
		b[0] = '0';

	/* Copy across and appropriately leading space things
	 */
	j = *jin;
	while (--i >= 0) {
		str = set_dig_s(j, b[i], str);
		j += SEGS_PER_DIGIT;
	}
	*jin = j;
	return str;
}


/* Display the number in H.MS mode.
 * HMS is hhh[degrees]mm'ss.ss" fixed formated modulo reduced to range
 */
static void set_x_hms(const decNumber *rgx, char *res) {
	decNumber x, y, a, t, u;
	int j=0;
	const int exp_last = SEGS_EXP_BASE + 2*SEGS_PER_EXP_DIGIT;
	unsigned int hr, min, sec, fs;

	if (check_special_dn(rgx, res)) {
		if (decNumberIsInfinite(rgx))
			res = set_dig_s(exp_last, 'o', res);
		return;
	}

	set_separator_decimal_modes();
	decNumberMod(&x, rgx, &const_9000);
	dn_abs(&a, rgx);
	if (decNumberIsNegative(&x)) {
		if (res != NULL)
			*res++ += '-';
		else
			SET_MANT_SIGN;
		dn_minus(&x, &x);
	}

	decNumberHR2HMS(&y, &x);
	dn_mulpow10(&t, &y, 6);
	decNumberRound(&u, &t);

	hms_step(&t, &u, &fs);
	hms_step(&u, &t, &sec);
	hms_step(&t, &u, &min);
	hr = dn_to_int(&t);
	if (sec >= 60) { sec -= 60; min++;	}
	if (min >= 60) { min -= 60; hr++;	}

	// degrees
	res = hms_render(hr, res, &j, 4, 1);
	res = set_dig_s(j, '@', res);
	j += SEGS_PER_DIGIT;

	// minutes
	res = hms_render(min, res, &j, 2, 1);
	res = set_dig_s(j, '\'', res);
	j += SEGS_PER_DIGIT;

	// seconds
	res = hms_render(sec, res, &j, 2, 1);
	res = set_decimal(j - SEGS_PER_DIGIT, DecimalMode, res);

	// Fractional seconds
	res = hms_render(fs, res, &j, 2, 0);

	// We're now pointing at the exponent's first digit...
	res = set_dig_s(j, '"', res);
	// j += SEGS_PER_EXP_DIGIT;

	// Check for values too big or small
	if (dn_ge(&a, &const_9000)) {
		res = set_dig_s(exp_last, 'o', res);
	} else if (! dn_eq0(&a)) {
		if (dn_le(&a, &const_hms_threshold)) {
			res = set_dig_s(exp_last, 'u', res);
		}
	}
}


static int set_x_fract(const decNumber *rgx, char *res) {
	decNumber x, w, n, d, t;
	char buf[32], *p = buf;
	int j;

	if (check_special_dn(rgx, res))
		return 1;
	dn_abs(&x, rgx);
	if (dn_ge(&x, &const_100000))
		return 0;
	if (dn_lt(&x, &const_0_0001))
		return 0;
	if (decNumberIsNegative(rgx)) {
		if (res != NULL)
			*res++ += '-';
		else
			SET_MANT_SIGN;
	}
	decNumberFrac(&w, &x);
	decNumber2Fraction(&n, &d, &w);	/* Get the number as a numerator & denominator */

	dn_divide(&t, &n, &d);
	dn_compare(&t, &t, &w);
	decNumberTrunc(&w, &x);		/* Extract the whole part */

	if (dn_eq(&n, &d)) {
		dn_inc(&w);
		decNumberZero(&n);
	}

	if (!UState.improperfrac) {
		if (!dn_eq0(&w)) {
			p = num_arg(p, dn_to_int(&w));
			*p++ = ' ';
		}
	} else {
		dn_multiply(&x, &w, &d);
		dn_add(&n, &n, &x);
	}
	p = num_arg(p, dn_to_int(&n));
	*p++ = '/';
	p = num_arg(p, dn_to_int(&d));
	*p = '\0';
	if ((p - 12) > buf) {
		p -= 12;
		*p = '<';
	} else	p = buf;
	for (j=0; *p != '\0'; p++) {
		res = set_dig_s(j, *p, res);
		j += SEGS_PER_DIGIT;
	}

	if (dn_eq0(&t))
		p = (char *)S7_fract_EQ;
	else if (decNumberIsNegative(&t))
		p = (char *)S7_fract_LT;
	else
		p = (char *)S7_fract_GT;
	for (j = SEGS_EXP_BASE; *p != '\0'; p++) {
		res = set_dig_s(j, *p, res);
		j += SEGS_PER_EXP_DIGIT;
	}
	return 1;
}

#if defined(INCLUDE_SIGFIG_MODE)
enum display_modes std_round_fix(const decNumber *z, int *dd) {
	decNumber c;
	int true_exp, x=0;
	int min_pos_exp, max_neg_exp;

	if ( get_user_flag(regI_idx) ) {
		min_pos_exp = 9;
		max_neg_exp = -4;}
	else {
		min_pos_exp = 12;
		max_neg_exp = 0 - UState.dispdigs;
	}
 
	dn_abs(&c, z); // c is abs(z)
	true_exp = c.exponent + c.digits - 1;

	if (get_user_flag(regK_idx) && UState.dispdigs < 10 ) {//trailing zeros display: flag K ignored if dispdigs > 9
		x = *dd;
	}

	if ((true_exp < x) && (true_exp > max_neg_exp)) { // decimals needed; *dd adjusted to provide correct number
		*dd += -true_exp;
 		return MODE_FIX; 
	}

	if ((true_exp <= max_neg_exp) || (true_exp >= min_pos_exp)) {
		return UState.fixeng?MODE_ENG:MODE_SCI; // force ENG/SCI mode for big / small numbers
	}
	else {
		return MODE_STD;
	}
 }
#else
enum display_modes std_round_fix(const decNumber *z) {
	decNumber b, c;

	dn_1(&b);
	b.exponent -= UState.dispdigs;
	dn_abs(&c, z);
	if (dn_gt(&c, &b) && dn_lt(&c, &const_1))
		return MODE_FIX;
	return MODE_STD;
}
#endif

/* SHOW display mode
 * in double precision show left or right part
 * 4 + 12 + 3 or 6 + 10 + 4 version
 */
static void show_x(char *x, int exp) {
	const int dbl = is_dblmode();
	const int upper = dbl ? 6 : 4;
	char *const p = find_char(x, '\0');
	int i, j;
	int left = 1;

	xset(p, '0', x + 34 - p);
	if (State2.window) {
		x += 16;	// right half
		left = 0;
	    }
	else {
		if (dbl) {
			if (exp < 0) {
				x[16] = '-';
				exp = -exp;
			}
			else
				x[16] = ' ';
			j = exp / 1000;
			x[17] = '0' + j;
			exp -= 1000 * j;
		}
		set_exp(exp, 1, CNULL);
	}

	for (i = 0, j = 0; i < 12; ++i, j += SEGS_PER_DIGIT)
		set_dig_s(j, x[upper + i], CNULL);

	// Move the digits over one place and insert the radix
	x[upper] = '\0';
	if (left) {
	    xcopy(x + 2, x + 1, upper);  // Copy end of string char too
	    x[1] = DecimalMode == DECIMAL_DOT ? '.' : ',';
	    }

	set_status(x);
}


/* Display the X register in the numeric portion of the display.
 * We have to account for the various display modes and numbers of
 * digits.
 */
static void set_x(const REGISTER *rgx, char *res, int dbl) {
	decNumber z;
	int digits = DISPLAY_DIGITS;

	if (dbl)
		decimal128ToNumber(&(rgx->d), &z);
	else
		decimal64ToNumber(&(rgx->s), &z);
	set_x_dn(&z, res, &digits);
}

void set_x_dn(decNumber *z, char *res, int *display_digits) {
	char x[50], *obp = x;
	int odig = 0;
	int show_exp = 0;
	int j;
	char mantissa[64];
	int exp = 0;
	char *p = mantissa;
	char *r;
	const char *q;
	int count, i;
	int extra_digits = 0;
	int dd = UState.dispdigs;
	int mode = UState.dispmode;
	int c;
	int negative = 0;
	int trimzeros = 0;

	// Do not allow non ALL modes to produce more digits than we're being asked to display.
#if defined(INCLUDE_SIGFIG_MODE)
	if ( ( !get_user_flag(regI_idx)) && (mode == MODE_STD) ) { //normal ALL mode: putting dd=display digits fills the display
		dd = *display_digits;
	}
	if (dd > *display_digits) // no more digits than we can display: no exclusion for ALL as dealt with above
		dd = *display_digits;
#else
	if (mode != MODE_STD && dd > *display_digits)
		dd = *display_digits;
#endif
		
	set_separator_decimal_modes();
#if defined(INCLUDE_YREG_CODE)
	if ( !res ) { // no hms or fraction displays for the dot matrix display
		if (!State2.smode && ! State2.cmplx) {
			if (State2.hms) {
				set_x_hms(z, res);
 				return;
			}
			else if (UState.fract) {
				if (set_x_fract(z, res))
					return;
			}
		}
	}		
#else
	if (!State2.smode && ! State2.cmplx && ! State2.wascomplex) {
		if (State2.hms) {
			set_x_hms(z, res);
			State2.hms = 0;
			return;
		} else if (UState.fract) {
			if (set_x_fract(z, res))
				return;
		}
	}
#endif

	if (check_special_dn(z, res))
		return;

	if (State2.smode == SDISP_SHOW) {
		dn_abs(z, z);
		decNumberNormalize(z, z, &Ctx);
		exp = z->exponent + z->digits - 1;
		z->exponent = 0;
	}

	xset(x, '\0', sizeof(x));

	if (dn_eq0(z)) {
		if (decNumberIsNegative(z) && get_user_flag(NAN_FLAG)) {
			x[0] = '-';
			x[1] = '0';
		} else
			x[0] = '0';
	} else
		decNumberToString(z, x);

	if (State2.smode == SDISP_SHOW) {
		show_x(x, exp);
		return;
	}

#if defined(INCLUDE_SIGFIG_MODE)
	if (mode == MODE_STD) {
		mode = std_round_fix(z, &dd); // modified function called
		if (mode != MODE_STD) { // allow zeros to be trimmed
			if ( get_user_flag(regK_idx) && UState.dispdigs < 10 ) { // flag K set gives zeros, but only for < 10 digits. So dispdigs=11 turns off any effect.
				trimzeros = 0;
			}
			else {
				trimzeros = 1;
			}
		}
 	}
#else
	if (mode == MODE_STD) {
		mode = std_round_fix(z);
		if (mode == MODE_FIX)
			trimzeros = 1;
		dd = *display_digits - 1;
	}
#endif

	xset(mantissa, '0', sizeof(mantissa)-1);
	mantissa[sizeof(mantissa)-1] = '\0';

	q = find_char(x, 'E');
	if (q == NULL) exp = 0;
	else exp = s_to_i(q+1);

	// Skip leading spaces and zeros.  Also grab the sign if it is there
	for (q=x; *q == ' '; q++);
	if (!res) {
		CLR_EXP_SIGN;
		CLR_MANT_SIGN;
	}
	if (*q == '-') {
		negative = 1;
		q++;
	} else if (*q == '+')
		q++;
	for (; *q == '0'; q++);
	if (*q == '.') {
		do
			exp--;
		while (*++q == '0');
		while (*q >= '0' && *q <= '9')
			*p++ = *q++;
	} else {
		if (*q >= '0' && *q <= '9')
			*p++ = *q++;
		while (*q >= '0' && *q <= '9') {
			*p++ = *q++;
			exp++;
		}
		if (*q == '.') {
			q++;
			while (*q >= '0' && *q <= '9')
				*p++ = *q++;
		}
	}

	if (mode == MODE_FIX) {
		if (exp > (*display_digits - 1) || exp < -dd)
			mode = UState.fixeng?MODE_ENG:MODE_SCI;
		else {
			extra_digits = exp;
			/* We might have push the fixed decimals off the
			 * screen so adjust if so.
			 */
			if (extra_digits + dd > (*display_digits - 1))
				dd = (*display_digits - 1) - extra_digits;
		}
	}

	// Round the mantissa to the number of digits desired
	p = mantissa + dd + extra_digits + 1;
	if (*p >= '5') {	// Round up
		*p = '0';
		for (r = mantissa; *r == '9'; r++);
		if (r == p) {   // Special case 9.9999999
			for (r = mantissa; *r == '9'; *r++ = '0');
			mantissa[0] = '1';
			exp++;
			if (mode == MODE_FIX && exp > (*display_digits - 1)) {
				mode = UState.fixeng?MODE_ENG:MODE_SCI;
				extra_digits = 0;
			}
		} else {
			while (*--p == '9')
				*p = '0';
			(*p)++;
		}
	}

	// Zap what is left
	for (p = mantissa + dd + extra_digits + 1; *p != '\0'; *p++ = '0');

	p = mantissa;
	switch (mode) {
	default:
	case MODE_STD:   
		for (count = *display_digits; mantissa[count] == '0'; count--);
		if (count != *display_digits)
			count++;
		// Too big or too small to fit on display
		if (exp >= *display_digits || exp < (count - *display_digits)) {
			switch ((exp % 3) * UState.fixeng) {
			case -1:
			case 2:
				*obp++ = *p++;
				odig++;
				dd--;
				exp--;
			case -2:
			case 1:
				*obp++ = *p++;
				odig++;
				dd--;
				exp--;
			case 0:
				;
			};
			*obp++ = *p++;
			odig++;
			*obp++ = '.';
			for (i=1; i<count; i++) {
				*obp++ = *p++;
				odig++;
			}
			show_exp = 1;
		} else if (exp >= 0) {  // Some digits to left of decimal point
			for(i=0; i<=exp; i++) {
				if (i > 0 && (exp - i) % 3 == 2)
					*obp++ = ',';
				*obp++ = *p++;
				odig++;
			}
			*obp++ = '.';
			if (count > (exp + 1)) {
				for (i=exp+1; i<count; i++) {
					*obp++ = *p++;
					odig++;
				}
			}
		} else {		// All digits to right of decimal point
			*obp++ = '0';
			odig++;
			*obp++ = '.';
			for (i=exp+1; i<0; i++) {
				*obp++ = '0';
				odig++;
			}
			for (i=0; i<count; i++) {
				*obp++ = *p++;
				odig++;
			}
		}
		break;

	case MODE_FIX:
		j = 0;
		if (exp >= 0) {		// Some digits to left of decimal point
			for (i=0; i<=exp; i++) {
				if (i > 0 && (exp - i) % 3 == 2)
					*obp++ = ',';
				*obp++ = *p++;
				odig++;
			}
			*obp++ = '.';
			for (i=0; i<dd && j < SEGS_EXP_BASE; i++) {
				*obp++ = *p++;
				odig++;
			}
		} else {		// All digits to right of decimal point
			*obp++ = '0';
			odig++;
			*obp++ = '.';
			for (i=exp+1; i<0; i++) {
				*obp++ = '0';
				odig++;
				dd--;
			}
			while (dd-- > 0) {
				*obp++ = *p++;
				odig++;
			}
		}
#if !defined(INCLUDE_SIGFIG_MODE)
		if (trimzeros)
			while (obp > x && obp[-1] == '0') {
				obp--;
				odig--;
			}
#endif			
		break;

	case MODE_ENG:
		switch (exp % 3) {
		case -1:
		case 2:
			*obp++ = *p++;
			odig++;
			dd--;
			exp--;
		case -2:
		case 1:
			*obp++ = *p++;
			odig++;
			dd--;
			exp--;
		case 0:
			;
		};
	// Falling through

	case MODE_SCI:
		*obp++ = *p++;
		odig++;
		*obp++ = '.';
		dd--;
		while (dd-- >= 0) {
			*obp++ = *p++;
			odig++;
		}
		show_exp = 1;
	}
#if defined(INCLUDE_SIGFIG_MODE)
	if (trimzeros) // ND change: trimzeros generally available
		while (obp > x && obp[-1] == '0') {
			obp--;
			odig--;
		}
#endif	
	/* Finally, send the output to the display */
	*obp = '\0';
	if (odig > *display_digits)
		odig = *display_digits;
	j = (*display_digits - odig) * SEGS_PER_DIGIT;
	if (negative) {
		if (res) *res++ = '-';
		else {
			if (j == 0)
				SET_MANT_SIGN;
			else
				set_dig(j - SEGS_PER_DIGIT, '-');
		}
	}
	for (i=0; (c = x[i]) != '\0' && j < SEGS_EXP_BASE; i++) {
		if (c == '.') {
			res = set_decimal(j - SEGS_PER_DIGIT, DecimalMode, res);
		} else if (c == ',') {
			res = set_separator(j, SeparatorMode, res);
		} else {
			res = set_dig_s(j, c, res);
			j += SEGS_PER_DIGIT;
		}
	}
#if defined(INCLUDE_RIGHT_EXP)
	if (show_exp) { // ND change: leading zeros in exponent in seven-segment display
		if ( !res ) {
				set_exp(exp, 1, res);
		}
		else {
			set_exp(exp, 0, res);
		}
	}
#else
	if (show_exp)
		set_exp(exp, 0, res);
#endif
	if (obp[-1] == '.' && res == NULL)
		set_decimal((*display_digits - 1) * SEGS_PER_DIGIT, DecimalMode, res);
	*display_digits = odig;
}

#if defined(QTGUI) || defined(IOS)
void format_display(char *buf) {
	if (State2.runmode && !State2.labellist && !State2.registerlist && !State2.status)
	{
		const char *p = get_cmdline();
		if (p == NULL) {
			format_reg(regX_idx, buf);
		} else {
			scopy(buf, p);
		}
	}
	else {
		buf[0]=0;
	}
}
#endif

void format_reg(int index, char *buf) {
	const REGISTER *const r = get_reg_n(index);

	if (is_intmode())
		set_int_x(get_reg_n_int(index), buf);
#ifndef HP16C_MODE_CHANGE
	else if (buf == NULL && State2.smode > SDISP_SHOW) {
		decNumber x;
		int s;
		unsigned long long int v;

		getRegister(&x, index);
		v = dn_to_ull(&x, &s);
		set_int_x(build_value(v, s), CNULL);
	}
#endif
	else
		set_x(r, buf, UState.mode_double);
}

/* Display the status screen */
static void show_status(void) {
	int i, n;
	int j = SEGS_EXP_BASE;
	const int status = State2.status - 3;
	char buf[16], *p = buf;
	unsigned int pc;

	if (status == -2) {
		set_status("Free:");
		p = num_arg(buf, free_mem());
		p = scopy(p, " , FL. ");
		p = num_arg(p, free_flash());
		*p = '\0';
		set_digits_string(buf, 0);
	}
	else if (status == -1) {
		/* Top line */
		p = scopy(buf, "Regs:");
		if (SizeStatRegs)
			p = scopy(p, " \221\006\006+");
		*p = '\0';
		set_status(buf);

		/* Bottom line */
		p = num_arg(buf, global_regs());
		if (LocalRegs < 0) {
			p = scopy(p, " , Loc. ");
			p = num_arg(p, local_regs());
		}
		*p = '\0';
		set_digits_string(buf, 0);
	} else {
		int base;
		int end;
		int group = 10;
		int start = 0;
		
		if (status <= 9) {
			base = 10 * status;
			end = base >= 70 ? 99 : base + 29;
			p = scopy(buf, "FL ");
			p = num_arg_0(p, base, 2);
			*p++ = '-';
			p = num_arg_0(p, end, 2);
			*p = '\0';
			set_status(buf);
		}
		else if (status == 10) {
			base = regX_idx;
			end = regK_idx;
			start = 3;
			group = 4;
			set_status("XYZT\006A:D\006LIJK");
		}
		else { // status == 11
			base = LOCAL_FLAG_BASE;
			end = LOCAL_FLAG_BASE + 15;
			set_status("FL.00-.15");
		}
		j = start * SEGS_PER_DIGIT;
		set_decimal(j, DECIMAL_DOT, CNULL);
		j += SEGS_PER_DIGIT;
		for (i = start; i < group + start; i++) {
			int k = i + base - start;
			int l = get_user_flag(k);
			k += group;
			if (end >= k) {
				l |= (get_user_flag(k) << 1);
				k += group;
				if (end >= k)
					l |= (get_user_flag(k) << 2);
			}
			set_dig(j, l);
			set_decimal(j, DECIMAL_DOT, CNULL);
			j += SEGS_PER_DIGIT;
			if (i == 4) {
				set_dig(j, 8);
				set_decimal(j, DECIMAL_DOT, CNULL);
				j += SEGS_PER_DIGIT;
			}
		}
	}

	j = SEGS_EXP_BASE;
	pc = state_pc();
	if (isXROM(pc))
		pc = 1;
	for (n=i=0; i<4; i++) {
		if (find_label_from(pc, 100+i, FIND_OP_ENDS)) {
			if (++n == 4) {
				set_dig(SEGS_EXP_BASE + SEGS_PER_EXP_DIGIT, 'L');
				set_dig(SEGS_EXP_BASE + 2*SEGS_PER_EXP_DIGIT, 'L');
			} else {
				set_dig(j, 'A'+i);
				j += SEGS_PER_EXP_DIGIT;
			}
		}
	}
}


/* Display the list of alpha labels */
static void show_label(void) {
	char buf[16];
	unsigned short int pc = State2.digval;
	unsigned int op = getprog(pc);
	int n = nLIB(pc);
	unsigned short int lblpc;

	set_status(prt((opcode)op, buf));
	set_digits_string(libname[n], 0);
#ifndef REALBUILD
	scopy(LastDisplayedNumber, libname_text[n]);
#endif

	if (op & OP_DBL) {
		lblpc = findmultilbl(op, 0);
		if (lblpc != pc) {
			set_digits_string("CALLS", SEGS_PER_DIGIT * 7);
			n = nLIB(lblpc);
			if (n == REGION_RAM)
				set_exp(lblpc, 1, CNULL);
			else {
				set_exp_digits_string(libname[n], CNULL);
#ifndef REALBUILD
				scopy(LastDisplayedNumber, libname_text[n]);
#endif
			}
		}
	}
}

/* Display a list of register contents */
static void show_registers(void) {
	char buf[16], *bp;
	int n = State2.digval;
	
#ifdef INCLUDE_FLASH_RECALL
	const int reg = State2.digval2 ? FLASH_REG_BASE + n : 
			State2.local   ? LOCAL_REG_BASE + n : 
			n;
#else
	const int reg = State2.local   ? LOCAL_REG_BASE + n : 
			n;
#endif

	if (State2.disp_as_alpha) {
		set_status(alpha_rcl_s(reg, buf));
	}
	else {
		xset(buf, '\0', 16);
#ifdef INCLUDE_FLASH_RECALL
		bp = scopy_spc(buf, State2.digval2 ? "Bkup" : "Reg ");
#else
		bp = scopy_spc(buf, "Reg ");
#endif
		if (State2.local) {
			*bp++ = '.';
			if (n >= 100) {
				*bp++ = '1';
				n -= 100;
			}
		}
		if (n < 100)
			bp = num_arg_0(bp, n, 2);
		else
			*bp++ = REGNAMES[n - regX_idx];
		set_status(buf);
	}
	format_reg(reg, CNULL);
}


static void set_annunciators(void)
{
	const enum trig_modes tm = get_trig_mode();

	/* Turn INPUT on for alpha mode.  Turn down arrow on if we're
	 * typing lower case in alpha mode.  Turn the big equals if we're
	 * browsing constants.
	 */
#ifdef MODIFY_BEG_SSIZE8
	dot(BEG, UState.stack_depth && ! Running);
#else
	dot(BEG, state_pc() <= 1 && ! Running);
#endif
	dot(INPUT, State2.catalogue || State2.alphas || State2.confirm);
	dot(DOWN_ARR, (State2.alphas || State2.multi) && State2.alphashift);
	dot(BIG_EQ, get_user_flag(A_FLAG));
	set_IO_annunciator();

	/* Set the trig mode indicator 360 or RAD.  Grad is handled elsewhere.
	 */
	dot(DEG, !is_intmode() && tm == TRIG_DEG);
	dot(RAD, !is_intmode() && tm == TRIG_RAD);
}


/*
 *  Toggle the little "=" sign
 */
void set_IO_annunciator(void) {
	int on = SerialOn
#ifdef REALBUILD
	  || DebugFlag
#endif
#ifdef INFRARED
	  || PrinterColumn != 0
#endif
	;

	if (on != IoAnnunciator) {
		dot(LIT_EQ, on);
		IoAnnunciator = on;
		finish_display();
	}
}

/*
 *  Update the display
 */
void display(void) {
	int i, j;
	char buf[40], *bp = buf;
	const char *p;
	int annuc = 0;
	const enum catalogues cata = (enum catalogues) State2.catalogue;
	int skip = 0;
	int x_disp = 0;
	const int shift = cur_shift();



	if (State2.disp_freeze) {
		State2.disp_freeze = 0;
		State2.disp_temp = 1;
#ifdef CONSOLE
		JustDisplayed = 1;
#endif
		ShowRPN = 0;
		return;
	}

	// Clear display
	reset_disp();

	xset(buf, '\0', sizeof(buf));
	if (State2.cmplx  && !cata) {
		*bp++ = COMPLEX_PREFIX;
		set_status(buf);
	}
	if (State2.version) {
		char vers[VERS_SVN_OFFSET + 5] = VERS_DISPLAY;
		set_digits_string("pAULI, WwALtE", 0);
		set_dig_s(SEGS_EXP_BASE, 'r', CNULL);
#ifndef REALBUILD
		scopy(LastDisplayedNumber, " P A U L I,  W A L T E R ");
		scopy(LastDisplayedExponent, " ");
#endif
		xcopy( vers + VERS_SVN_OFFSET, SvnRevision, 4 );
		set_status(vers);
		skip = 1;
		goto nostk;
	} else if (State2.confirm) {
		set_status(S_SURE);
	} else if (State2.hyp) {
		bp = scopy(bp, "HYP");
		if (! State2.dot)
			*bp++ = '\235';
		set_status(buf);
	} else if (State2.gtodot) {
		// const int n = 3 + (nLIB(state_pc()) & 1); // Number of digits to display/expect
		bp = scopy_char(bp, argcmds[RARG_GTO].cmd, '.');
		if (State2.numdigit > 0)
			bp = num_arg_0(bp, (unsigned int)State2.digval, (int)State2.numdigit);
		// for (i=State2.numdigit; i<n; i++)
			*bp++ = '_';
		set_status(buf);
	} else if (State2.rarg) {
		/* Commands with arguments */
		bp = scopy(bp, argcmds[CmdBase].cmd);
		bp = scopy(bp, State2.ind?"\015" : "\006\006");
		if (State2.dot) {
			*bp++ = 's';
			*bp++ = '_';
		} else if (shift == SHIFT_F) {
			*bp++ = '\021';
			*bp++ = '_';
		} else {
			/* const int maxdigits = State2.shuffle ? 4 
						: State2.ind ? 2 
						: num_arg_digits(CmdBase); */
			if (State2.local)
				*bp++ = '.';
			if (State2.numdigit > 0) {
				if (State2.shuffle)
					for (i = 0, j = State2.digval; i<State2.numdigit; i++, j >>= 2)
						*bp++ = REGNAMES[j & 3];
				else
					bp = num_arg_0(bp, (unsigned int)State2.digval, (int)State2.numdigit);
			}
			// for (i = State2.numdigit; i < maxdigits; i++)
				*bp++ = '_';
		}
		set_status(buf);
	} else if (State2.test != TST_NONE) {
		*bp++ = 'x';
		*bp++ = "=\013\035<\011>\012"[State2.test];
		*bp++ = '_';
		*bp++ = '?';
		set_status(buf);
	} else if (cata) {
		const opcode op = current_catalogue(State.catpos);
		char b2[16];
		const char *p;

		bp = scopy(bp, "\177\006\006");
		p = catcmd(op, b2);
		if (*p != COMPLEX_PREFIX && State2.cmplx)
			*bp++ = COMPLEX_PREFIX;
		bp = scopy(bp, p);
		if (cata == CATALOGUE_CONST || cata == CATALOGUE_COMPLEX_CONST) {
			// State2.disp_small = 1;
			if (op == RARG_BASEOP(RARG_INTNUM) || op == RARG_BASEOP(RARG_INTNUM_CMPLX))
				set_digits_string("0 to 255", 0);
			else
				set_x(get_const(op & RARG_MASK, 0), CNULL, 0);
			skip = 1;
		} else if (State2.runmode) {
			if (cata == CATALOGUE_CONV) {
				decNumber x, r;
				decimal64 z;

				getX(&x);
				if (opKIND(op) == KIND_MON) {
					const unsigned int f = argKIND(op);
					if (f < NUM_MONADIC && ! isNULL(monfuncs[f].mondreal)) {
						FP_MONADIC_REAL fp = (FP_MONADIC_REAL) EXPAND_ADDRESS(monfuncs[f].mondreal);
						update_speed(0);
						fp(&r, &x);
					}
					else
						set_NaN(&r);
				} else
					do_conv(&r, op & RARG_MASK, &x);
				decNumberNormalize(&r, &r, &Ctx);
				packed_from_number(&z, &r);
				set_x((REGISTER *)&z, CNULL, 0);
				skip = 1;
			} else if (op >= (OP_NIL | OP_sigmaX2Y) && op < (OP_NIL | OP_sigmaX2Y) + NUMSTATREG) {
				REGISTER z, *const x = StackBase;
				copyreg(&z, x);
				sigma_val((enum nilop) argKIND(op));
				set_x(x, CNULL, is_dblmode());
				copyreg(x, &z);
				skip = 1;
			}
		}
		set_status(buf);
	} else if (State2.multi) {
		bp = scopy_char(bp, multicmds[CmdBase].cmd, '\'');
		if (State2.numdigit > 0) {
			*bp++ = (char) State2.digval;
			if (State2.numdigit > 1)
				*bp++ = State2.digval2;
		}
		set_status(buf);
	} else if (State2.status) {
		show_status();
		skip = 1;
	} else if (State2.labellist) {
		show_label();
		skip = 1;
	} else if (State2.registerlist) {
		show_registers();
		skip = 1;
		if (shift != SHIFT_N || (State2.smode == SDISP_SHOW && is_intmode())) {
			annunciators();
		}
#ifdef SHIFT_HOLD_TEMPVIEW
	} else if (State2.disp_as_alpha) {
		set_status(alpha_rcl_s(regX_idx, buf));
#endif
	} else if (State2.runmode) {
		if (DispMsg) {
			set_status(DispMsg);
		} else if (DispPlot) {
			set_status_graphic((const unsigned char *)get_reg_n(DispPlot-1));
		} else if (State2.alphas) {
#if 0
			set_digits_string("AlpHA", 0);
#endif
			bp = scopy(buf, Alpha);
			j = State2.alpha_pos;
			if (j != 0) {
				i = slen(buf);
				j *= 6;
				if ( i - j >= 12 ) {
					buf[ (i - j) ] = '\0';
					set_status_right(buf);
				}
				else {
					set_status(buf);
				}
			} else {
				if (shift != SHIFT_N) {
					*bp++ = 021 + shift - SHIFT_F;
					*bp++ = '\0';
				}
				set_status_right(buf);
			}
		} else {
			annuc = 1;
		}
	} else {
		show_progtrace(buf);
		i = state_pc();
		if (i > 0)
			set_status(prt(getprog(i), buf));
		else
			set_status("");
		set_dot(STO_annun);
#if 0
		if (State2.smode == SDISP_SHOW) {
			unsigned short int crc;
			crc = checksum_program();
			j = SEGS_PER_DIGIT * 0;
			for (i=0; i<4; i++) {
				set_dig(j, "0123456789ABCDEF"[crc & 0xf]);
				crc >>= 4;
				j += SEGS_PER_DIGIT;
			}
			skip = 1;
		}
		else
#endif
		if (cur_shift() != SHIFT_N || State2.cmplx || State2.arrow)
			annuc = 1;
		goto nostk;
	}
	show_stack();
nostk:	show_flags();
	if (!skip) {
		if (State2.runmode) {
			p = get_cmdline();
			if (p == NULL || cata) {
				if (ShowRegister != -1) {
					x_disp = (ShowRegister == regX_idx) && !State2.hms;
					format_reg(ShowRegister, CNULL);
				}
				else
					set_digits_string(" ---", 4 * SEGS_PER_DIGIT);
			} else {
				disp_x(p);
				x_disp = 1;
			}
		} else {
			unsigned int pc = state_pc();
			unsigned int upc = user_pc(pc);
			const int n = nLIB(pc);
			xset(buf, '\0', sizeof(buf));
			set_exp(ProgFree, 1, CNULL);
			num_arg_0(scopy_spc(buf, n == 0 ? S7_STEP : libname[n]), 
				  upc, 3 + (n & 1));  // 4 digits in ROM and Library
			set_digits_string(buf, SEGS_PER_DIGIT);
#ifndef REALBUILD
			xset(buf, '\0', sizeof(buf));
			set_exp(ProgFree, 1, CNULL);
			num_arg_0(scopy_spc(buf, n == 0 ? S7_STEP_ShortText : libname_shorttext[n]),
				  upc, 3 + (n & 1));  // 4 digits in ROM and Library
      { // allow local declaration of b and l in C (not C++) on VisualStudio
			  char *b=buf;
			  char *l=LastDisplayedNumber;
			  *l++=' ';
			  while(*b) {
				  *l++=*b++;
				  *l++=' ';
			  }
			  *l=0;
      }
#endif
		}
	}
	set_annunciators();

	if (x_disp == 0 || State2.smode != SDISP_NORMAL || DispMsg != NULL || DispPlot || State2.disp_as_alpha) {
		ShowRPN = 0;
		dot(RPN, 0);
	}

	// disp_temp disables the <- key
	State2.disp_temp = ! ShowRPN && State2.runmode 
		           && (! State2.registerlist || State2.smode == SDISP_SHOW || State2.disp_as_alpha);

#if defined(INCLUDE_YREG_CODE)
	if ((annuc && (! State2.disp_temp || State2.hms)) || State2.wascomplex) // makes sure that hms numbers appear in the dot-matrix display
 		annunciators();
 	State2.hms = 0;
#else
	if ((annuc && ! State2.disp_temp) || State2.wascomplex)
		annunciators();
#endif

	State2.version = 0;
	State2.disp_as_alpha = 0;
	State2.smode = SDISP_NORMAL;
	State2.invalid_disp = 0;
	ShowRegister = regX_idx;
	DispMsg = CNULL;
	DispPlot = 0;
	State2.disp_small = 0;
	finish_display();
#ifdef CONSOLE
	JustDisplayed = 1;
#endif
}

/*
 *  Frozen display will revert to normal only after another call to display();
 */
void frozen_display()
{
	State2.disp_freeze = 0;
	display();
	State2.disp_freeze = 1;
}

static void set_status_graphic(const unsigned char *graphic) {
	int glen = *graphic++;
	int i, j;
#ifndef CONSOLE
	unsigned long long int mat[6];

	xset(mat, 0, sizeof(mat));
#endif
#ifndef REALBUILD
	forceDispPlot=1;
#endif
	if (glen <= 0)			return;
	if (glen > BITMAP_WIDTH)	glen = BITMAP_WIDTH;

	for (i=0; i<6; i++)
		for (j=1; j<=glen; j++) {
#ifndef CONSOLE
			if (graphic[j] & (1 << i))
				mat[i] |= 1LL << j;
#else
			dot(j*6+i+MATRIX_BASE, (graphic[j] & (1 << i))?1:0);
#endif
		}
#ifndef CONSOLE
	set_status_grob(mat);
#endif
}


/* Take the given string and display as much of it as possible on the top
 * line of the display.  The font size is set by the smallp parameter.
 * We allow character to go one pixel beyond the display since the rightmost
 * column is almost always blank.
 */
static void set_status_sized(const char *str, int smallp) {
	unsigned short int posns[257];
#ifdef INCLUDE_FONT_ESCAPE
	// Mark posns as uninitialized, smallp must be 0 or 1 for this to work correctly.
	int posns_state = 255;
#endif
	unsigned int x = 0;
	int i, j;
	const int offset = smallp ? 256 : 0;
#ifndef CONSOLE
	unsigned long long int mat[6];

	xset(mat, 0, sizeof(mat));
#endif
#ifndef REALBUILD
	scopy(LastDisplayedText, str);
#ifdef INCLUDE_FONT_ESCAPE
	for (i = 0; LastDisplayedText[i] != '\0'; ) { // Remove 007 escapes
		if (LastDisplayedText[i] == '\007' && LastDisplayedText[i + 1] != '\0') {
			scopy(LastDisplayedText + i, LastDisplayedText + i + 2);
		}
		else {
			++i;
		}
	}
#endif
	forceDispPlot=0;
#endif
#ifdef RP_PREFIX
	RectPolConv = 0;
#endif
#ifndef INCLUDE_FONT_ESCAPE
	findlengths(posns, smallp);
#endif
	while (*str != '\0' && x <= BITMAP_WIDTH+1)  {
		int c;
		int width;
		unsigned char cmap[6];
#ifdef INCLUDE_FONT_ESCAPE
		int real_width;
		int current_smallp;

		// A 007 byte followed by a mode byte changes the way the following character is printed.
		// Bit 7 (MSB) of the mode byte is currently unused and should be set to 1.
		// Bits 6-5: 00 -> don't change font
		//           01 -> (not used)
		//           10 -> use big font
		//           11 -> use small font
		// Bits 4-0: character will be considered this wide
		if (str[0] == '\007') {
			width = str[1] & 0x1F;
			switch (str[1] & 0x60) {
			default:
			case 0x00:	current_smallp = smallp;
					break;
			case 0x40:	current_smallp = 0;
					break;
			case 0x60:	current_smallp = 1;
					break;
			}
			c = (unsigned char) str[2] + (current_smallp ? 256 : 0);
			str += 3;

			real_width = charlengths(c);
		} else {
			c = (unsigned char) *str++ + offset;
			real_width = width = charlengths(c);
			current_smallp = smallp;
		}

		if (x + real_width > BITMAP_WIDTH+1)
			break;

		if (posns_state != current_smallp) {
			findlengths(posns, current_smallp);
			posns_state = current_smallp;
		}
		unpackchar(c, cmap, current_smallp, posns);
#else
		c = (unsigned char) *str++ + offset; //doesn't matter if c is 256 too big;

		//cmap = &charset[c][0];
		width = charlengths(c);

		if (x + width > BITMAP_WIDTH+1)
			break;

		/* Decode the packed character bytes */
		unpackchar(c, cmap, smallp, posns);
#endif

		for (i=0; i<6; i++)
			for (j=0; j<width; j++) {
				if (x+j >= BITMAP_WIDTH)
					break;
#ifndef CONSOLE
				if (cmap[i] & (1 << j))
					mat[i] |= 1LL << (x+j);
#else
				dot((x+j)*6+i+MATRIX_BASE, (cmap[i] & (1 << j))?1:0);
#endif
			}
		x += width;
	}


#ifndef CONSOLE
	set_status_grob(mat);
#else
	for (i=MATRIX_BASE + 6*x; i<400; i++)
		clr_dot(i);
#endif
}


/* Determine the pixel length of the string if it were displayed.
 */
int pixel_length(const char *s, int smallp)
{
	int len = 0;
	const int offset = smallp ? 256 : 0;
	while (*s != '\0') {
#ifdef INCLUDE_FONT_ESCAPE
		if (s[0] == '\007') {
			len += s[1] & 0x1F;
			s += 3;
			continue;
		}
#endif
		len += charlengths( (unsigned char) *s++ + offset );
	}
	return len;
}


/* Determine the pixel length of the string if it were displayed.
 * If this is larger than the display, return true.
 */
static int string_too_large(const char *s) {
	return pixel_length(s, 0) > BITMAP_WIDTH+1;
}


/* Display the given string on the screen.
 */
static void set_status(const char *str) {
	set_status_sized(str, State2.disp_small || string_too_large(str));
}


/*
 *  Display messages (global function)
 */
extern void message(const char *str1, const char *str2)
{
	State2.disp_freeze = 0;
	State2.disp_small = 0;
	if ( State2.invalid_disp && str2 == NULL ) {
		// Complete redraw necessary
		DispMsg = str1;
		display();
	}
	else {
		if ( str2 != NULL ) {
			reset_disp();
			ShowRPN = 0;
			set_annunciators();
			set_digits_string( str2, 0 );
		}
		set_status( str1 );
		finish_display();
	}
}

#ifdef INCLUDE_STOPWATCH

static void stopwatch_exponent(const char* exponent) {
	int j = SEGS_EXP_BASE;
	for (; *exponent!=0; exponent++) {
		set_dig_s(j, *exponent, CNULL);
		j += SEGS_PER_EXP_DIGIT;
	}
}

void stopwatch_message(const char *str1, const char *str2, int force_small, char* exponent)
{
#ifndef REALBUILD
	xset(LastDisplayedNumber, ' ', NUMBER_LENGTH);
#endif
	reset_disp();
	set_dot(DEG);
	set_digits_string( str2, 0 );
	State2.disp_small = force_small;
	if( exponent!=NULL ) {
		stopwatch_exponent(exponent);
	}
	set_status( str1 );
	finish_display();
}


#endif // INCLUDE_STOPWATCH

/* Display the right hand characters from the given string.
 * Trying to fit as many as possible into the bitmap area,
 * and reduce font size if required.
 */
static void set_status_right(const char *str) {
	unsigned int x = 0;
	const char *p;
	const int toolarge = State2.disp_small || string_too_large(str);
	const int offset = toolarge ? 256 : 0;

	for (p=str; *p != '\0'; p++);
	while (--p >= str) {
		const unsigned int c = (unsigned char) *p + offset;

		x += charlengths(c);
		if (x > BITMAP_WIDTH+1)
			break;
	}
	set_status_sized(p+1, toolarge);
}



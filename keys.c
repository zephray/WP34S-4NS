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
#include "keys.h"
#include "display.h"
#include "lcd.h"
#include "int.h"
#include "consts.h"
#include "storage.h"
#include "stats.h"
#include "catalogues.h"
//#include "printer.h"

#if defined(QTGUI) || defined(IOS)
extern void changed_catalog_state();
#endif

#define STATE_UNFINISHED	(OP_SPEC | OP_UNFINISHED)
#define STATE_BACKSPACE		(OP_SPEC | OP_BACKSPACE)
#define STATE_SST		(OP_SPEC | OP_SST)
#define STATE_BST		(OP_SPEC | OP_BST)
#define STATE_RUNNING		(OP_SPEC | OP_RUNNING)
#define STATE_IGNORE		(OP_SPEC | OP_IGNORE)
#define STATE_WINDOWLEFT	(OP_SPEC | OP_WINDOWLEFT)
#define STATE_WINDOWRIGHT	(OP_SPEC | OP_WINDOWRIGHT)
//#define STATE_SHOW		(OP_SPEC | OP_SHOW)

/* Define this if the key codes map rows sequentially */

#define TEST_EQ		0
#define TEST_NE		1
#define TEST_LT		2
#define TEST_LE		3
#define TEST_GT		4
#define TEST_GE		5

enum confirmations {
	// Apart from the first of these, these must be in the same
	// order as the opcodes in xeq.h: OP_CLALL, OP_RESET, OP_CLPROG, OP_CLPALL
	confirm_none=0, confirm_clall, confirm_reset, confirm_clprog, confirm_clpall
};

/* Local data to this module */
unsigned int OpCode;
FLAG OpCodeDisplayPending;
FLAG GoFast;
FLAG NonProgrammable;

/*
 *  Needed before definition
 */ 
static unsigned int advance_to_next_label(unsigned int pc, int inc, int search_end);

/*
 *  Return the shift state
 */
enum shifts cur_shift(void) {
	enum shifts s = shift_down();
	return s == SHIFT_N ? (enum shifts) State2.shifts : s;
}

/*
 *  Set new shift state, return previous state
 */
static enum shifts set_shift(enum shifts shift) {
	enum shifts r = cur_shift();
	State2.shifts = shift;
	State2.alpha_pos = 0;
	return r;
}

/*
 *  Clear shift state and return previous state
 */
enum shifts reset_shift(void) {
	return set_shift(SHIFT_N);
}

/*
 *  Toggle shift state
 */
static void toggle_shift(enum shifts shift) {
	State2.shifts = State2.shifts == shift ? SHIFT_N : shift;
}


/*
 * Mapping from the key code to a linear index
 * The trick is to move the shifts and the holes in the map out of the way
 */
static int keycode_to_linear(const keycode c)
{
	static const unsigned char linear_key_map[ 7 * 6 - 1 ] = {
		 0,  1,  2,  3,  4,  5,   // K00 - K05
		 6,  7,  8, 34, 34, 34,   // K10 - K15
		 9, 10, 11, 12, 13,  0,   // K20 - K24
		14, 15, 16, 17, 18,  0,   // K30 - K34
		19, 20, 21, 22, 23,  0,   // K40 - K44
		24, 25, 26, 27, 28,  0,   // K50 - K54
		29, 30, 31, 32, 33        // K60 - K63
	};
	return linear_key_map[c];
}

/*
 * Mapping from the key code to a row column code ('A'=11 to '+'=75)
 * Used in KEY? and for shorthand addressing
 */
int keycode_to_row_column(const int c)
{
	return 11 + ( c / 6 ) * 10 + c % 6;
}

/*
 * Mapping from a row column code ('A'=11 to '+'=75) to the key code
 * Used in PUTK and KTYPE.
 */
int row_column_to_keycode(const int c)
{
	int row = c / 10 - 1;
	int col = c % 10 - 1;

	if (row < 0 || row > 6 || col > 5 - (row >= 2))
		return -1;
	return row * 6 + col;
}

/*
 *  Mapping from a key code to a digit from 0 to 9 or to a register address
 *  Bit seven is set if the key cannot be used as a label shortcut
 */
#define NO_REG 0x7f
#define NO_SHORT 0x80
unsigned int keycode_to_digit_or_register(const keycode c)
{
	static const unsigned char map[] = {
		// K00 - K05
		NO_SHORT | regA_idx, NO_SHORT | regB_idx,
		NO_SHORT | regC_idx, NO_SHORT | regD_idx,
		NO_SHORT | NO_REG,   NO_REG,
		// K10 - K12
		NO_REG, NO_REG, regI_idx,
		// K20 - K24
		NO_SHORT | NO_REG, regJ_idx, regK_idx, regL_idx, NO_SHORT | NO_REG,
		// K30 - K34
		NO_REG, 7, 8, 9, NO_REG,
		// K40 - K44
		NO_REG, 4, 5, 6, regT_idx,
		// K50 - K54
		NO_REG, 1, 2, 3, NO_REG,
		// K60 - K63
		NO_SHORT | NO_REG, 0, NO_SHORT | LOCAL_REG_BASE,
		regY_idx, regZ_idx,
		// Shifts
		NO_REG
	};

	return (unsigned int) map[keycode_to_linear(c)];
}

/*
 *  Mapping of a keycode and shift state to a catalogue number
 */
static enum catalogues keycode_to_cat(const keycode c, enum shifts shift)
{
	enum catalogues cat = CATALOGUE_NONE;
	int i, col, max;
	const struct _map {
		unsigned char key, cat[3];
	} *cp;

	// Common to both alpha mode and normal mode is the programming X.FCN catalogue
	if (c == K53 && shift == SHIFT_H && ! State2.runmode && ! State2.cmplx && ! State2.multi)
		return CATALOGUE_PROGXFCN;

	if (! State2.alphas && ! State2.multi) {
		/*
		 *  Normal processing - Not alpha mode
		 */
		static const struct _map cmap[] = {
			{ K_ARROW, { CATALOGUE_CONV,      CATALOGUE_NONE,      CATALOGUE_CONV          } },
			{ K05,     { CATALOGUE_MODE,      CATALOGUE_MODE,      CATALOGUE_MODE          } },
#ifdef INCLUDE_USER_CATALOGUE
			{ K10,     { CATALOGUE_LABELS,    CATALOGUE_LABELS,    CATALOGUE_USER          } },
#else
			{ K10,     { CATALOGUE_LABELS,    CATALOGUE_LABELS,    CATALOGUE_LABELS        } },
#endif
			{ K20,     { CATALOGUE_CONST,     CATALOGUE_NONE,      CATALOGUE_COMPLEX_CONST } },
			{ K41,     { CATALOGUE_PROB,      CATALOGUE_NONE,      CATALOGUE_PROB          } },
			{ K42,     { CATALOGUE_STATS,     CATALOGUE_NONE,      CATALOGUE_STATS         } },
			{ K43,     { CATALOGUE_SUMS,      CATALOGUE_NONE,      CATALOGUE_SUMS          } },
			{ K44,     { CATALOGUE_MATRIX,    CATALOGUE_NONE,      CATALOGUE_MATRIX        } },
			{ K50,     { CATALOGUE_STATUS,    CATALOGUE_STATUS,    CATALOGUE_STATUS        } },
			{ K51,     { CATALOGUE_TEST,      CATALOGUE_TEST,      CATALOGUE_TEST          } },
#ifdef INCLUDE_INTERNAL_CATALOGUE
			{ K52,     { CATALOGUE_PROG,      CATALOGUE_PROG,      CATALOGUE_INTERNAL      } },
#else
			{ K52,     { CATALOGUE_PROG,      CATALOGUE_PROG,      CATALOGUE_PROG          } },
#endif
			{ K53,     { CATALOGUE_NORMAL,    CATALOGUE_INT,       CATALOGUE_COMPLEX       } },
		};

		if (c == K60 && shift == SHIFT_G) {
			/*
			 *  SHOW starts register browser
			 */
			return CATALOGUE_REGISTERS;
		}
#if 0
		// conflicts with c# 002 and c# 003
		if ((c == K52 || c == K53) && shift == SHIFT_N && State2.cmplx && State2.catalogue == CATALOGUE_NONE) {
			/*
			 *  Shorthand to complex P.FCN & X.FCN - h may be omitted
			 */
			shift = SHIFT_H;
		}
#endif
		if (shift != SHIFT_H) {
			/*
			 *  All standard catalogues are on h-shifted keys
			 */
			return CATALOGUE_NONE;
		}

		/*
		 *  Prepare search
		 */
		cp = cmap;
#ifndef WINGUI
		col = State2.cmplx || shift_down() == SHIFT_H ? 2 : UState.intm ? 1 : 0;
#else
		col = State2.cmplx ? 2 : UState.intm ? 1 : 0;
#endif
		max = sizeof(cmap) / sizeof(cmap[0]);
	}
	else {
		/*
		 *  All the alpha catalogues go here
		 */
		static const struct _map amap[] = {
			{ K_ARROW, { CATALOGUE_NONE, CATALOGUE_ALPHA_ARROWS,  CATALOGUE_NONE              } },
			{ K_CMPLX, { CATALOGUE_NONE, CATALOGUE_ALPHA_LETTERS, CATALOGUE_MODE              } },
		//	{ K10,     { CATALOGUE_NONE, CATALOGUE_NONE,          CATALOGUE_LABELS            } },
			{ K12,     { CATALOGUE_NONE, CATALOGUE_NONE,	      CATALOGUE_ALPHA_SUBSCRIPTS  } },
		//	{ K50,     { CATALOGUE_NONE, CATALOGUE_NONE,          CATALOGUE_STATUS            } },
			{ K51,     { CATALOGUE_NONE, CATALOGUE_NONE,          CATALOGUE_ALPHA_COMPARES    } },
			{ K53,     { CATALOGUE_NONE, CATALOGUE_NONE,          CATALOGUE_ALPHA             } },
			{ K62,     { CATALOGUE_NONE, CATALOGUE_NONE,          CATALOGUE_ALPHA_SYMBOLS     } },
		};
		static const char smap[] = { 0, 1, 0, 2 }; // Map shifts to columns;

		/*
		 *  Prepare search
		 */
		cp = amap;
		col = smap[shift];
		max = sizeof(amap) / sizeof(amap[0]);
	}

	/*
	 *  Search the key in one of the tables
	 */
	for (i = 0; i < max; ++i, ++cp) {
		if (cp->key == c) {
			cat = (enum catalogues) cp->cat[col];
			break;
		}
	}
	if (State2.multi && (cat < CATALOGUE_ALPHA_SYMBOLS || cat > CATALOGUE_ALPHA_SUBSCRIPTS)) {
		// Ignore the non character catalogues in multi character mode
		cat = CATALOGUE_NONE;
	}
	return cat;
}


/*
 * Mapping from key position to alpha in the four key planes plus
 * the two lower case planes.
 */
static int keycode_to_alpha(const keycode c, unsigned int shift)
{
	static const unsigned char alphamap[][6] = {
		/*upper f-sft g-sft h-sft lower g-lower */
		{ 'A',  0000, 'A',  0000, 'a',  0240,  },  // K00
		{ 'B',  0000, 'B',  0000, 'b',  0241,  },  // K01
		{ 'C',  0000, 0202, 0000, 'c',  0242,  },  // K02
		{ 'D',  0000, 0203, 0000, 'd',  0243,  },  // K03
		{ 'E',  0015, 'E',  0000, 'e',  0244,  },  // K04 ->
		{ 'F',  0000, 0224, 0000, 'f',  0264,  },  // K05

		{ 'G',  0000, 0202, 0000, 'g',  0242,  },  // K10
		{ 'H',  0000, 'X',  0000, 'h',  0265,  },  // K11
		{ 'I',  0000, 'I',  0000, 'i',  0250,  },  // K12

		{ 0000, 0000, 'H',  0000, 0000, 0246,  },  // K20 ENTER
		{ 'J',  '(',  ')',  0027, 'j',  ')',   },  // K21
		{ 'K',  0010, 'K',  0225, 'k',  0251,  },  // K22
		{ 'L',  0000, 0212, 0257, 'l',  0252,  },  // K23
		{ 0000, 0000, 0000, 0000, 0000, 0000   },  // K24 <-

		{ 0000, 0000, 0000, 0000, 0000, 0000,  },  // K30
		{ 'M',  '7',  'M',  '&',  'm',  0253,  },  // K31
		{ 'N',  '8',  'N',  '|',  'n',  0254,  },  // K32
		{ 'O',  '9',  0227, 0013, 'o',  0267,  },  // K33
		{ 'P',  '/',  0217, '\\', 'p',  0257,  },  // K34

		{ 0000, 0000, 0000, '!',  0000, 0000,  },  // K40
		{ 'Q',  '4',  0000, 0000, 'q',  0000,  },  // K41
		{ 'R',  '5',  'R',  0000, 'r',  0260,  },  // K42
		{ 'S',  '6',  0221, 0000, 's',  0261,  },  // K43
		{ 'T',  0034, 'T',  0000, 't',  0262,  },  // K44

		{ 0000, 0000, 0000, '?',  0000, 0000,  },  // K50
		{ '1',  '1',  0207, 0000, '1',  0247,  },  // K51
		{ 'U',  '2',  0000, 0000, 'u',  0000,  },  // K52
		{ 'V',  '3',  0000, 0000, 'v',  0000,  },  // K53
		{ 'W',  '-',  0000, 0000, 'w',  0000,  },  // K54

		{ 0000, 0222, 0000, 0000, 0000, 0000,  },  // K60
		{ '0',  '0',  0226, ' ',  '0',  0266,  },  // K61
		{ 'X',  '.',  0215, 0000, 'x',  0255,  },  // K62
		{ 'Y',  0000, 'Y',  0000, 'y',  0263,  },  // K63
		{ 'Z',  '+',  'Z',  0000, 'z',  0245,  },  // K64
	};
	if (State2.alphashift) {
		if (shift == SHIFT_N)
			shift = SHIFT_LC_N;
		else if (shift == SHIFT_G)
			shift = SHIFT_LC_G;
	}
	return alphamap[keycode_to_linear(c)][shift];
}

static void init_arg(const enum rarg base) {
	CmdBase = base;
	State2.ind = 0;
	State2.digval = 0;
	State2.numdigit = 0;
	State2.rarg = 1;
	State2.dot = 0;
	State2.local = 0;
	State2.shuffle = (base == RARG_SHUFFLE);
}

static void init_cat(enum catalogues cat) {
	if (cat == CATALOGUE_NONE && State2.catalogue != CATALOGUE_NONE) {
		// Save last catalogue for a later restore
		State.last_cat = State2.catalogue;
		CmdLineLength = 0;
	}
	process_cmdline();

	State2.labellist = 0;
	State2.registerlist = 0;
	State2.status = 0;
	State2.catalogue = CATALOGUE_NONE;

	switch (cat) {
	case CATALOGUE_LABELS:
		// Label browser
		State2.labellist = 1;
		State2.digval = advance_to_next_label(ProgBegin, 0, 0);
		break;
	
	case CATALOGUE_REGISTERS:
		// Register browser
		State2.registerlist = 1;
		State2.digval = regX_idx;
		State2.digval2 = 0;
		break;

	case CATALOGUE_STATUS:
		// Flag browser
		State2.status = 1;
		break;

	default:
		// Normal catalogue
		State2.catalogue = cat;
		State2.cmplx = (cat == CATALOGUE_COMPLEX || cat == CATALOGUE_COMPLEX_CONST);
		if (cat != CATALOGUE_NONE && State.last_cat != cat) {
			// Different catalogue, reset position
			State.catpos = 0;
		}
	}
	reset_shift();
#if defined(QTGUI) || defined(IOS)
	changed_catalog_state();
#endif
}

/*
 *  Reset the internal state to a sane default
 */
void init_state(void) {
#ifndef REALBUILD
	unsigned int a = State2.flags;
	unsigned int b = State2.trace;
#else
	FLAG t = TestFlag;
#endif
	int v = Voltage;
	int k = LastKey;

	CmdBase = 0;
	// Removed: will clear any locals on power off
	// clrretstk(0);

	if (State2.runmode == 0)
		update_program_bounds(1);
	xset(&State2, 0, sizeof(State2));
	State2.test = TST_NONE;
	State2.runmode = 1;
	set_lift();

	// Restore stuff that has been moved to State2 for space reasons
	// but must not be cleared.
	Voltage = v;
	LastKey = k;
#ifndef REALBUILD
	State2.trace = b;
	State2.flags = a;
#else
	TestFlag = t;
#endif
	ShowRegister = regX_idx;
}

void soft_init_state(void) {
	int soft;
	unsigned int runmode;
	unsigned int alphas;

	if (CmdLineLength) {
		CmdLineLength = 0;
		CmdLineEex = 0;
		CmdLineDot = 0;
		return;
	}
	soft = State2.multi || State2.rarg || State2.hyp || State2.gtodot || State2.labellist ||
			State2.cmplx || State2.arrow || State2.test != TST_NONE || State2.status;
	runmode = State2.runmode;
	alphas = State2.alphas;
	init_state();
	if (soft) {
		State2.runmode = runmode;
		State2.alphas = alphas;
	}
}

static int check_confirm(int op) {
	if (opKIND(op) == KIND_NIL) {
		const int nilop = argKIND(op);
		if (nilop >= OP_CLALL && nilop <= OP_CLPALL) {
			State2.confirm = confirm_clall + (nilop - OP_CLALL);
			return STATE_UNFINISHED;
		}
		if ((nilop >= OP_RECV && nilop <= OP_PSTO)
#ifdef INFRARED
			|| nilop == OP_PRINT_PGM
#endif
#ifdef INCLUDE_STOPWATCH
			|| nilop == OP_STOPWATCH
#endif
		) {
			// These commands are not programmable
			NonProgrammable = 1;
		}
	}
	return op;
}

static void set_smode(const enum single_disp d) {
	State2.smode = (State2.smode == d)?SDISP_NORMAL:d;
}

static int check_f_key(int n, const int dflt) {
	const int code = 100 + n;
	unsigned int pc = state_pc();

	if (State2.runmode) {
		if (isXROM(pc))
			pc = 1;
		if (find_label_from(pc, code, FIND_OP_ENDS))
			return RARG(RARG_XEQ, code);
	}
	return dflt;
}

/* Return non-zero if the current mode is integer and we accept letters
 * as digits.
 */
static int intltr(int d) {
	return (UState.intm && (! State2.runmode || (int) int_base() > d));
}

/*
 *  Process a key code in the unshifted mode.
 */
static int process_normal(const keycode c)
{
	static const unsigned short int op_map[] = {
		// Row 1
		OP_SPEC | OP_SIGMAPLUS, // A to D
		OP_MON  | OP_RECIP,
		OP_DYA  | OP_POW,
		OP_MON  | OP_SQRT,
		OP_SPEC | OP_E,		// ->
		OP_SPEC | OP_F,		// CPX
		// Row 2
		RARG_STO,
		RARG_RCL,
		OP_NIL  | OP_RDOWN,
		// Row 3
		OP_SPEC | OP_ENTER,
		RARG(RARG_SWAPX, regY_idx),
		OP_SPEC | OP_CHS,	// CHS
		OP_SPEC | OP_EEX,	// EEX
		OP_SPEC | OP_CLX,	// <-
		// Row 4
		RARG_XEQ,
		OP_SPEC | OP_7,
		OP_SPEC | OP_8,
		OP_SPEC | OP_9,
		OP_DYA  | OP_DIV,
		// Row 5
		STATE_BST,
		OP_SPEC | OP_4,
		OP_SPEC | OP_5,
		OP_SPEC | OP_6,
		OP_DYA  | OP_MUL,
		// Row 6
		STATE_SST,		// SST
		OP_SPEC | OP_1,
		OP_SPEC | OP_2,
		OP_SPEC | OP_3,
		OP_DYA  | OP_SUB,
		// Row 7
		STATE_UNFINISHED,	// ON/C
		OP_SPEC | OP_0,
		OP_SPEC | OP_DOT,
		OP_NIL  | OP_RS,	// R/S
		OP_DYA  | OP_ADD
	};
	int lc = keycode_to_linear(c);
	int op = op_map[lc];

	// The switch handles all the special cases
	switch (c) {
	case K00:
	case K01:
		if (UState.intm)
			op = OP_SPEC | (OP_A + lc);
	case K02:
	case K03:
		if (intltr(lc + 10)) {
			op = OP_SPEC | (OP_A + lc);
			return op;
		}
		return check_f_key(lc, op);

	case K_ARROW:
#ifdef INT_MODE_TEMPVIEW
		if (intltr(14))
			return op;
#else
		if (UState.intm)
			return op;
#endif
		process_cmdline_set_lift();
		State2.arrow = 1;
		set_shift(SHIFT_G);
		break;

	case K_CMPLX:
		if (UState.intm)
			return op;
		State2.cmplx = 1;
		break;

	case K24:				// <-
		if (State2.disp_temp)
			return STATE_UNFINISHED;
		if (State2.runmode)
			return op;
		return STATE_BACKSPACE;

	case K10:				// STO
	case K11:				// RCL
	case K30:				// XEQ
		init_arg((enum rarg)op);
		break;

	default:
		return op;			// Keys handled by table
	}
	return STATE_UNFINISHED;
}


/*
 *  Process a key code after f or g shift
 */
static int process_fg_shifted(const keycode c) {

#define NO_INT 0xf000
	static const unsigned short int op_map[][2] = {
		// Row 1
		{ 1,                               0                           }, // HYP
		{ OP_MON | OP_SIN      | NO_INT,   OP_MON | OP_ASIN   | NO_INT },
		{ OP_MON | OP_COS      | NO_INT,   OP_MON | OP_ACOS   | NO_INT },
		{ OP_MON | OP_TAN      | NO_INT,   OP_MON | OP_ATAN   | NO_INT },
		{ OP_NIL | OP_P2R      | NO_INT,   OP_NIL | OP_R2P    | NO_INT },
		{ OP_NIL | OP_FRACPROPER,	   OP_NIL | OP_FRACIMPROPER    }, // CPX
		// Row 2
		{ OP_NIL | OP_HMS,                 OP_NIL | OP_DEG             },
		{ OP_NIL | OP_FLOAT,               OP_NIL | OP_RAD             },
		{ OP_NIL | OP_RANDOM,              OP_NIL | OP_GRAD            },
		// Row 3
		{ STATE_UNFINISHED,		   OP_NIL | OP_FILL            }, // ENTER
		{ STATE_WINDOWLEFT,   		   STATE_WINDOWRIGHT   	       },
		{ RARG(RARG_BASE, 2),		   RARG(RARG_BASE, 8)          },
		{ RARG(RARG_BASE, 10),		   RARG(RARG_BASE, 16)         },
		{ OP_NIL | OP_CLPROG,		   OP_NIL | OP_SIGMACLEAR      },
		// Row 4
		{ OP_MON | OP_EXP,                 OP_MON | OP_LN              },
		{ OP_MON | OP_10POWX,		   OP_MON | OP_LOG             },
		{ OP_MON | OP_2POWX,		   OP_MON | OP_LG2             },
		{ OP_DYA | OP_POW,                 OP_DYA | OP_LOGXY           },
		{ OP_MON | OP_RECIP    | NO_INT,   OP_DYA | OP_PARAL	       },
		// Row 5
		{ OP_DYA | OP_COMB,                OP_DYA | OP_PERM            },
		{ OP_MON | OP_cdf_Q    | NO_INT,   OP_MON | OP_qf_Q   | NO_INT },
		{ OP_NIL | OP_statMEAN | NO_INT,   OP_NIL | OP_statS  | NO_INT },
		{ OP_MON | OP_yhat     | NO_INT,   OP_NIL | OP_statR           },
		{ OP_MON | OP_SQRT,		   OP_MON | OP_SQR             },
		// Row 6
		{ RARG_SF,                         RARG_CF                     },
		{ TST_EQ,                          TST_NE                      }, // tests
		{ RARG_SOLVE           | NO_INT,   RARG_INTG          | NO_INT },
		{ RARG_PROD            | NO_INT,   RARG_SUM           | NO_INT },
		{ OP_MON | OP_PERCNT   | NO_INT,   OP_MON | OP_PERCHG | NO_INT },
		// Row 7
#ifdef INFRARED
		{ RARG(RARG_PRINT_REG,regX_idx),   STATE_UNFINISHED	       },
#else
		{ STATE_UNFINISHED,		   STATE_UNFINISHED	       },
#endif
		{ OP_MON | OP_ABS,		   OP_MON | OP_RND             },
		{ OP_MON | OP_TRUNC,		   OP_MON | OP_FRAC            },
		{ RARG_LBL,			   OP_NIL | OP_RTN             },
		{ RARG_DSE,			   RARG_ISG                    }
	};

	static const unsigned short int op_map2[] = {
		STATE_UNFINISHED,
		STATE_UNFINISHED,
		OP_DYA  | OP_POW,
		OP_MON  | OP_SQRT
	};

	enum shifts shift = reset_shift();
	int lc = keycode_to_linear(c);
	int op = op_map[lc][shift == SHIFT_G];
	int no_int = ((op & NO_INT) == NO_INT);
	if (no_int)
		op &= ~NO_INT;

	switch (c) {
	case K00:
		if (! UState.intm) {
			State2.hyp = 1;
			State2.dot = op;
			// State2.cmplx = 0;
			return STATE_UNFINISHED;
		}
		// fall through
	case K01:
	case K02:
	case K03:
		if (UState.intm && shift == SHIFT_F) {
			return check_f_key(lc, op_map2[lc]);
		}
		break;

	case K20:				// Alpha
		if (shift == SHIFT_F) {
			process_cmdline_set_lift();
			State2.alphas = 1;
		}
		break;

	case K51:
		process_cmdline_set_lift();
		State2.test = op;
		return STATE_UNFINISHED;

	case K50:
#ifndef REALBUILD
		if (SHIFT_N != shift_down()) {
			State2.trace = (shift == SHIFT_F);
			return STATE_UNFINISHED;
		}
#endif
	case K52:
	case K53:
	case K63:
	case K64:
		if (op != (OP_NIL | OP_RTN)) {
			if (! (no_int && UState.intm))
				init_arg((enum rarg) op);
			return STATE_UNFINISHED;
		}
		break;

	default:
		break;
	}
	if (no_int && UState.intm)
		return STATE_UNFINISHED;

	return check_confirm(op);
#undef NO_INT
}

/*
 *  Process a key code after h shift
 */
static int process_h_shifted(const keycode c) {
#define _RARG    0x8000	// Must not interfere with existing opcode markers
#define NO_INT   0x4000
	static const unsigned short int op_map[] = {
		// Row 1
		_RARG   | RARG_STD,
		_RARG   | RARG_FIX,
		_RARG   | RARG_SCI,
		_RARG   | RARG_ENG,
		STATE_UNFINISHED,	// CONV
		STATE_UNFINISHED,	// MODE
		// Row 2
		STATE_UNFINISHED,	// CAT
		_RARG   | RARG_VIEW,
		OP_NIL  | OP_RUP,
		// Row 3
		_RARG	| RARG_INTNUM,	// CONST, will be emitted in integer mode only
		_RARG   | RARG_SWAPX,
		OP_MON  | OP_NOT,
		CONST(OP_PI) | NO_INT,
		OP_NIL  | OP_rCLX,
		// Row 4
		_RARG   | RARG_GTO,
		OP_DYA  | OP_LAND,
		OP_DYA  | OP_LOR,
		OP_DYA  | OP_LXOR,
		OP_DYA  | OP_MOD,
		// Row 5
		OP_MON  | OP_FACT,
		STATE_UNFINISHED,	// PROB
		STATE_UNFINISHED,	// STAT
		STATE_UNFINISHED,	// CFIT
		STATE_UNFINISHED,	// MATRIX
		// Row 6
		STATE_UNFINISHED,	// STATUS
		STATE_UNFINISHED,	// TEST
		STATE_UNFINISHED,	// P.FCN
		STATE_UNFINISHED,	// X.FCN
		OP_SPEC | OP_SIGMAMINUS | NO_INT,
		// Row 7
		OP_NIL | OP_OFF,
		_RARG   | RARG_PAUSE,
#ifdef MODIFY_K62_E3_SWITCH
		OP_NIL  | OP_THOUS_OFF,
#else
 		OP_NIL  | OP_RADCOM,
#endif
		STATE_UNFINISHED,	// P/R
		OP_SPEC | OP_SIGMAPLUS | NO_INT
	};

	int lc = keycode_to_linear(c);
	int op = op_map[lc];
	reset_shift();

	// The switch handles all the special cases
	switch (c) {
	case K62:
		if (UState.intm)
			op = UState.nointseparator ? (OP_NIL | OP_INTSEP_ON) : (OP_NIL | OP_INTSEP_OFF);
		else
#ifdef MODIFY_K62_E3_SWITCH
			if (UState.nothousands) op = OP_NIL | OP_THOUS_ON;
#else
			if (UState.fraccomma) op = OP_NIL | OP_RADDOT;
#endif
		break;

	case K63:					// Program<->Run mode
		State2.runmode = 1 - State2.runmode;
		process_cmdline_set_lift();
		update_program_bounds(1);
		if (! State2.runmode && state_pc() == 1 && ProgEnd == 1)
			set_pc(0);
		break;

	default:
		break;
	}

	if (op != STATE_UNFINISHED) {
		if (op & _RARG) {
			init_arg((enum rarg) (op & ~(_RARG | NO_INT)));
			op = STATE_UNFINISHED;
		}
	}
	return UState.intm && (op & NO_INT) ? STATE_UNFINISHED : op & ~NO_INT;
#undef _RARG
#undef NO_INT
}

/*
 *  Process a key code after CPX
 */
static int process_cmplx(const keycode c) {
#define _RARG   0xFF00
#define CSWAPXZ RARG(RARG_CSWAPX, regZ_idx)
#define CNUM(n) RARG(RARG_INTNUM_CMPLX, n)

	static const unsigned short int op_map[][4] = {
		// Row 1
		{ 1,			1,                   0,                   0		      }, // HYP
		{ OP_CMON | OP_RECIP,	OP_CMON | OP_SIN,    OP_CMON | OP_ASIN,   STATE_UNFINISHED    },
		{ OP_CDYA | OP_POW,	OP_CMON | OP_COS,    OP_CMON | OP_ACOS,   STATE_UNFINISHED    },
		{ OP_CMON | OP_SQRT,	OP_CMON | OP_TAN,    OP_CMON | OP_ATAN,   STATE_UNFINISHED    },
		{ STATE_UNFINISHED,	OP_NIL | OP_P2R,     OP_NIL | OP_R2P,     STATE_UNFINISHED    },
		{ STATE_UNFINISHED,	STATE_UNFINISHED,    STATE_UNFINISHED,    STATE_UNFINISHED    }, // CPX
		// Row 2
		{ _RARG | RARG_CSTO,	STATE_UNFINISHED,    STATE_UNFINISHED,    STATE_UNFINISHED    },
		{ _RARG | RARG_CRCL,	STATE_UNFINISHED,    STATE_UNFINISHED,    _RARG | RARG_CVIEW  },
		{ OP_NIL | OP_CRDOWN,	STATE_UNFINISHED,    STATE_UNFINISHED,    OP_NIL | OP_CRUP    }, // R^
		// Row 3
		{ OP_NIL | OP_CENTER,	STATE_UNFINISHED,    OP_NIL | OP_CFILL,   OP_NIL | OP_CFILL   }, // ENTER
		{ CSWAPXZ,		STATE_UNFINISHED,    STATE_UNFINISHED,    _RARG | RARG_CSWAPX },
		{ OP_CMON | OP_CCHS,	STATE_UNFINISHED,    STATE_UNFINISHED,    OP_CMON | OP_CCONJ  },
		{ CONST_CMPLX(OP_PI),	STATE_UNFINISHED,    STATE_UNFINISHED,    CONST_CMPLX(OP_PI)  },
		{ STATE_UNFINISHED,	STATE_UNFINISHED,    STATE_UNFINISHED,    STATE_UNFINISHED    },
		// Row 4
		{ STATE_UNFINISHED,	OP_CMON | OP_EXP,    OP_CMON | OP_LN,     STATE_UNFINISHED    },
		{ CNUM(7),		OP_CMON | OP_10POWX, OP_CMON | OP_LOG,    STATE_UNFINISHED    },
		{ CNUM(8),		OP_CMON | OP_2POWX,  OP_CMON | OP_LG2,    STATE_UNFINISHED    },
		{ CNUM(9),		OP_CDYA | OP_POW,    OP_CDYA | OP_LOGXY,  STATE_UNFINISHED    },
		{ OP_CDYA | OP_DIV,	OP_CMON | OP_RECIP,  OP_CDYA | OP_PARAL,  STATE_UNFINISHED    },
		// Row 5
		{ STATE_UNFINISHED,	OP_CDYA | OP_COMB,   OP_CDYA | OP_PERM,   OP_CMON | OP_FACT   },
		{ CNUM(4),		STATE_UNFINISHED,    STATE_UNFINISHED,    STATE_UNFINISHED    },
		{ CNUM(5),		STATE_UNFINISHED,    STATE_UNFINISHED,    STATE_UNFINISHED    },
		{ CNUM(6),		STATE_UNFINISHED,    STATE_UNFINISHED,    STATE_UNFINISHED    },
		{ OP_CDYA | OP_MUL,	OP_CMON | OP_SQRT,   OP_CMON | OP_SQR,    OP_CMON | OP_SQR    },
		// Row 6
		{ STATE_UNFINISHED,	STATE_UNFINISHED,    STATE_UNFINISHED,    STATE_UNFINISHED    },
		{ CNUM(1),		TST_EQ,              TST_NE,              STATE_UNFINISHED    }, // tests
		{ CNUM(2),		STATE_UNFINISHED,    STATE_UNFINISHED,    STATE_UNFINISHED    },
		{ CNUM(3),		STATE_UNFINISHED,    STATE_UNFINISHED,    STATE_UNFINISHED    },
		{ OP_CDYA | OP_SUB,	STATE_UNFINISHED,    STATE_UNFINISHED,    STATE_UNFINISHED    },
		// Row 7
		{ STATE_UNFINISHED,	STATE_UNFINISHED,    STATE_UNFINISHED,    OP_NIL | OP_OFF     },
		{ CNUM(0),		OP_CMON | OP_ABS,    OP_CMON | OP_RND,    STATE_UNFINISHED    },
		{ OP_NIL | OP_cmplxI,	OP_CMON | OP_TRUNC,  OP_CMON | OP_FRAC,   STATE_UNFINISHED    },
#ifdef INCLUDE_STOPWATCH
		{ OP_NIL | OP_STOPWATCH, STATE_UNFINISHED,   STATE_UNFINISHED,    STATE_UNFINISHED    },
#else
		{ STATE_UNFINISHED,	STATE_UNFINISHED,    STATE_UNFINISHED,    STATE_UNFINISHED    },
#endif
		{ OP_CDYA | OP_ADD,	STATE_UNFINISHED,    STATE_UNFINISHED,    STATE_UNFINISHED    }
	};

	enum shifts shift = reset_shift();
	int lc = keycode_to_linear(c);
	int op = op_map[lc][shift];
	State2.cmplx = 0;

	if ((op & _RARG) == _RARG) {
		init_arg((enum rarg) (op & ~_RARG));
		return STATE_UNFINISHED;
	}
	if (c == K00) {
		// HYP
		process_cmdline_set_lift();
		State2.hyp = 1;
		State2.dot = op;
		State2.cmplx = 1;
		return STATE_UNFINISHED;
	}

	if (shift != SHIFT_N) {
		switch (c) {
		case K_CMPLX:
			set_shift(shift);
			break;

		case K51:
			if (op != STATE_UNFINISHED) {
				process_cmdline_set_lift();
				State2.cmplx = 1;
				State2.test = op;
			}
			return STATE_UNFINISHED;

		case K60:
			init_state();
			break;

		default:
			break;
		}
	}
#ifdef INCLUDE_STOPWATCH
	return check_confirm(op);
#else
	return op;
#endif

#undef _RARG
#undef CSWAPXZ
#undef CNUM
}



/*
 * Fairly simple routine for dealing with the HYP prefix.
 * This setting can only be followed by 4, 5, or 6 to specify
 * the function.  The inverse routines use the code too, the State2.dot
 * is 1 for normal and 0 for inverse hyperbolic.  We also have to
 * deal with the complex versions and the handling of that key and
 * the ON key are dealt with by our caller.
 */
static int process_hyp(const keycode c) {
	static const unsigned char op_map[][2] = {
		{ OP_ASINH, OP_SINH },
		{ OP_ACOSH, OP_COSH },
		{ OP_ATANH, OP_TANH }
	};
	int cmplx = State2.cmplx;
	int f = State2.dot;

	State2.hyp = 0;
	State2.cmplx = 0;
	State2.dot = 0;

	switch ((int)c) {

	case K01:
	case K02:
	case K03:
		return (cmplx ? OP_CMON : OP_MON) | op_map[c - K01][f];

	case K_CMPLX:
		cmplx = ! cmplx;
		goto stay;

	case K_F:
	case K_G:
		f = (c == K_F);
		// fall trough
	stay:
		// process_cmdline_set_lift();
		State2.hyp = 1;
		State2.cmplx = cmplx;
		State2.dot = f;
		break;

	default:
		break;
	}
	return STATE_UNFINISHED;
}


/*
 *  Process a key code after ->
 */
static int process_arrow(const keycode c) {
	static const unsigned short int op_map[][2] = {
		{ OP_MON | OP_2DEG,  OP_MON | OP_2HMS },
		{ OP_MON | OP_2RAD,  OP_MON | OP_HMS2 },
		{ OP_MON | OP_2GRAD, STATE_UNFINISHED }
	};
	static const enum single_disp disp[][2] = {
		{ SDISP_OCT, SDISP_BIN },
		{ SDISP_HEX, SDISP_DEC }
	};
	const int f = (reset_shift() == SHIFT_F);

	State2.arrow = 0;
	
	if (c >= K10 && c <= K12)
		return op_map[c - K10][f];

	if (c == K22 || c == K23)
		set_smode(disp[c - K22][f]);

	return STATE_UNFINISHED;
}


/* Process a GTO . sequence
 */
static int gtodot_digit(const int n) {
	const int val = State2.digval * 10 + n;
	const int lib = nLIB(state_pc());

	if (val > sizeLIB(lib) / 10)
		return val;
	if (++State2.numdigit == 3 + (lib & 1))
		return val;
	State2.digval = val;
	return -1;
}

static int gtodot_fkey(int n) {
	const int code = 100 + n;
	unsigned int pc = state_pc();

	if(isXROM(pc))
		pc = 1;
	pc = find_label_from(pc, code, FIND_OP_ERROR | FIND_OP_ENDS);
	if (pc > 0)
		return pc;
	return state_pc();
}

static int process_gtodot(const keycode c) {
	int pc = -1;
	unsigned int rawpc = keycode_to_digit_or_register(c);

	if (rawpc <= 9) {
		// Digit 0 - 9
		pc = gtodot_digit(rawpc);
	}
	else if (c >= K00 && c <= K03) {
		// A - D
		rawpc = gtodot_fkey(c - K00);
		goto fin;
	}
	else if (c == K62) {
		// .
		rawpc = ProgSize;
		goto fin;
	}
	else if (c == K20) {
		// ENTER - short circuit processing
		pc = State2.digval;
	}
	else if (c == K24) {
		// backspace
		if (State2.numdigit == 0) {
			goto fin2;
		} else {
			State2.numdigit--;
			State2.digval /= 10;
		}
	}
	else if (c == K40) {
		// up
		rawpc = state_pc();
		if (rawpc == 1)
			rawpc = 0;
		set_pc(do_dec(rawpc, 0));
		update_program_bounds(1);
		rawpc = ProgBegin;
		goto fin;
	}
	else if (c == K50) {
		// down
		update_program_bounds(1);
		rawpc = do_inc(ProgEnd, 0);
		if (rawpc == 0 && ProgSize > 0)
			rawpc = 1;
		goto fin;
	}
	if (pc >= 0) {
		rawpc = find_user_pc(pc);
fin:		set_pc(rawpc);
fin2:		State2.gtodot = 0;
		State2.digval = 0;
		State2.numdigit = 0;
	}
	return STATE_UNFINISHED;
}


/* Process a keystroke in alpha mode
 */
static int process_alpha(const keycode c) {
	const enum shifts shift = reset_shift();
	int ch = keycode_to_alpha(c, shift);
	unsigned int alpha_pos = State2.alpha_pos, n;
        int op = STATE_UNFINISHED;
	State2.alpha_pos = 0;

	switch (c) {
	case K10:	// STO
		if (shift == SHIFT_F) {
			init_arg(RARG_ASTO);
			return STATE_UNFINISHED;
		}
		break;

	case K11:	// RCL - maybe view
		if (shift == SHIFT_F) {
			init_arg(RARG_ARCL);
			return STATE_UNFINISHED;
		} else if (shift == SHIFT_H) {
			init_arg(RARG_VIEW_REG);
			return STATE_UNFINISHED;
		}
		break;

	case K20:	// Enter - maybe exit alpha mode
		if (shift == SHIFT_G || shift == SHIFT_H)
			break;
		if (shift == SHIFT_F && ! State2.runmode) {
			State2.multi = 1;
			State2.numdigit = 0;
			CmdBase = DBL_ALPHA;
			return STATE_UNFINISHED;
		}
		State2.alphas = 0;
		State2.alphashift = 0;
		return op;

	case K24:	// Clx - backspace, clear Alpha
		if (shift == SHIFT_N)
			return STATE_BACKSPACE;
		if (shift == SHIFT_H)
			return OP_NIL | OP_CLRALPHA;
		break;

	case K40:
		if (shift == SHIFT_N) {
			if ( State2.runmode ) {
				// Alpha scroll left
				n = alpha_pos + 1;
				State2.alpha_pos = ( n < ( alen() + 5 ) / 6 ) ? n : alpha_pos;
				return STATE_UNFINISHED;
			}
			return STATE_BST;
		}
		break;

	case K50:
		if (shift == SHIFT_N) {
			if ( State2.runmode ) {
				// Alpha scroll right
				if (alpha_pos > 0)
					State2.alpha_pos = alpha_pos-1;
				return STATE_UNFINISHED;
			}
			return STATE_SST;
		}
		break;

	case K60:	// EXIT/ON maybe case switch, otherwise exit alpha
		if (shift == SHIFT_F)
			State2.alphashift = 1 - State2.alphashift;
		else if (shift == SHIFT_H)
			return OP_NIL | OP_OFF;
		else if (shift == SHIFT_N)
			init_state();
		return STATE_UNFINISHED;

	case K63:
		if (shift == SHIFT_F)
			return OP_NIL | OP_RS;		// R/S
		break;

	default:
		break;
	}

	/* Look up the character and return an alpha code if okay */
	if (ch == 0)
		return STATE_UNFINISHED;
	return RARG(RARG_ALPHA, ch);
}

/*
 *  Code to handle all commands with arguments
 */
static void reset_arg(void) {
	init_arg((enum rarg) 0);
	State2.rarg = 0;
}

static int arg_eval(unsigned int val) {
	const unsigned int base = CmdBase;
	const int r = RARG(base, val 
				 + (State2.ind ? RARG_IND : 0) 
		                 + (State2.local ? LOCAL_REG_BASE : 0));
	const unsigned int ssize = (! UState.stack_depth || ! State2.runmode ) ? 4 : 8;

	if (! State2.ind) {
		/*
		 *  Central argument checking for some commands
		 */
		if (argcmds[base].cmplx && (val > TOPREALREG - 2 && (val & 1)))
			// Disallow odd complex register > 98
			return STATE_UNFINISHED;
		if ((base == RARG_STOSTK || base == RARG_RCLSTK) && (val > TOPREALREG - ssize))
			// Avoid stack clash for STOS/RCLS
			return STATE_UNFINISHED;
	}
	// Build op-code
	reset_arg();
	return r;
}

static int arg_digit(int n) {
	const unsigned int base = CmdBase;
	const unsigned int val = State2.digval * 10 + n;
	const int is_reg = argcmds[base].reg || State2.ind;
	int lim;
	
	if (State2.local) {
		// Handle local registers and flags
		lim = MAX_LOCAL_DIRECT - 1;				// default
		if (State2.runmode) {
			if (LocalRegs == 0)
				return STATE_UNFINISHED;		// no local flags or registers
			if (is_reg) {
				lim = local_regs_rarg((enum rarg) base) - 1;
				if (lim >= MAX_LOCAL_DIRECT)
					lim = MAX_LOCAL_DIRECT - 1;	// in case of more than 16 locals
			}
		}
	}
	else if (is_reg)						// normal register
		lim = State2.runmode ? global_regs_rarg((enum rarg) base) - 1 : TOPREALREG - 1;
	else {
		lim = (int) argcmds[base].lim;				// any other command
		if (lim >= RARG_IND && argcmds[base].indirectokay)
			lim = RARG_IND - 1;
	}
	if ((int) val > lim)
		return STATE_UNFINISHED;

	State2.digval = val;
	++State2.numdigit;
	if ((int) val * 10 > lim || State2.numdigit == num_arg_digits(base)) {
		int result = arg_eval(val);
		if ( result == STATE_UNFINISHED ) {
			--State2.numdigit;
			State2.digval /= 10;
		}
		return result;
	}
	return STATE_UNFINISHED;
}

static int arg_fkey(int n) {
	const unsigned int b = CmdBase;

	if (argcmds[b].label || (b >= RARG_SF && b <= RARG_FCF))
	{
		if (State2.ind || State2.numdigit > 0)
			return STATE_UNFINISHED;
		if (argcmds[b].lim < 100)
			return STATE_UNFINISHED;
		return arg_eval(n + 100);
	}
	return STATE_UNFINISHED;
}

static int arg_storcl_check(const unsigned int b, const int cmplx) {
#ifdef INCLUDE_FLASH_RECALL
	return (b == RARG_STO || b == RARG_RCL || b == RARG_FLRCL ||
			(cmplx && (b == RARG_CSTO || b == RARG_CRCL || b == RARG_FLCRCL)));
#else
	return (b == RARG_STO || b == RARG_RCL || (cmplx && (b == RARG_CSTO || b == RARG_CRCL )));
#endif
}

static int arg_storcl(const unsigned int n, int cmplx) {
	unsigned int b = CmdBase;

	if (arg_storcl_check(b, cmplx)) {
		CmdBase += n;
		return 1;
	}
	/* And we can turn off the operation too */
	if (b >= n) {
		b -= n;
		if (arg_storcl_check(b, cmplx)) {
			CmdBase = b;
			return 1;
		}
	}
	return 0;
}

static int process_arg_dot(const unsigned int base) {

	if (State2.numdigit == 0) {
		// Only valid at beginning of entry
		if (State2.dot || State2.local) {
			// '..' or ENTER '.' = X
			State2.local = 0;
			return arg_eval(regX_idx);
		}
		if (argcmds[base].local || State2.ind) {
			// local register or flag select
			State2.local = 1;
		}
		else if (base == RARG_GTO || base == RARG_XEQ) {
			// Special GTO . sequence
			if (! State2.ind) {
				State2.gtodot = 1;
				reset_arg();
			}
		}
	}
	return STATE_UNFINISHED;
}

static int process_arg_shuffle(int r) {
	State2.digval += r << (State2.numdigit++ << 1);
	if (State2.numdigit < 4)
		return STATE_UNFINISHED;
	return arg_eval(State2.digval);
}


static int process_arg(const keycode c) {
	unsigned int base = CmdBase;
	unsigned int n = keycode_to_digit_or_register(c);
	int stack_reg = argcmds[base].stckreg || State2.ind;
	const enum shifts previous_shift = (enum shifts) State2.shifts;
	const enum shifts shift = reset_shift();
	int label_addressing = argcmds[base].label && ! State2.ind && ! State2.dot;
	int shorthand = label_addressing && c != K_F 
		        && (shift == SHIFT_F || (n > 9 && !(n & NO_SHORT)));

	n &= ~NO_SHORT;
	if (base >= NUM_RARG) {
		reset_arg();
		return STATE_UNFINISHED;
	}
	if (n <= 9 && ! shorthand && ! State2.dot && ! State2.shuffle)
		return arg_digit(n);

	if (shorthand)
		// row column shorthand addressing
		return arg_eval(keycode_to_row_column(c));

	/*
	 *  So far, we've got the digits and some special label addressing keys
	 *  Handle the rest here.
	 */
	switch ((int)c) {
	case K_F:
		if (label_addressing)
			set_shift(previous_shift == SHIFT_F ? SHIFT_N : SHIFT_F);
		break;

	case K_ARROW:		// arrow
		if (!State2.dot && argcmds[base].indirectokay) {
			State2.ind = ! State2.ind;
			if (! stack_reg)
				State2.dot = 0;
		}
		break;

	case K_CMPLX:
		if (State2.ind || State2.dot)
			break;
		if (base == RARG_STO)
			CmdBase = RARG_STOM;
		else if (base == RARG_RCL)
			CmdBase = RARG_RCLM;
		break;

	case K63:	// Y
		if (State2.shuffle)
			return process_arg_shuffle(1);
	case K00:	// A
	case K01:	// B
	case K02:	// C
	case K03:	// D
	case K12:	// I (lastY)
	case K21:	// J
	case K22:	// K
	case K23:	// L (lastX)
		if (State2.dot || stack_reg)
			return arg_eval(n);
		if ( c <= K03 )
			return arg_fkey(c - K00);		// Labels or flags A to D
		break;

	case K62:	// X, '.'
		if (State2.shuffle)
			return process_arg_shuffle(0);
		return process_arg_dot(base);

	/* STO and RCL can take an arithmetic argument */
	case K64:		// Z register
		if (State2.shuffle)
			return process_arg_shuffle(2);
		if (State2.dot || ( ! arg_storcl(RARG_STO_PL - RARG_STO, 1) && stack_reg))
			return arg_eval(n);
		break;

	case K54:
		if (base == RARG_VIEW || base == RARG_VIEW_REG) {
			reset_arg();
			return OP_NIL | OP_VIEWALPHA;
		}
		arg_storcl(RARG_STO_MI - RARG_STO, 1);
		break;

	case K44:		// T register
		if (State2.shuffle)
			return process_arg_shuffle(3);
		if (State2.dot || ( ! arg_storcl(RARG_STO_MU - RARG_STO, 1) && stack_reg))
			return arg_eval(n);
		break;

	case K34:
		arg_storcl(RARG_STO_DV - RARG_STO, 1);
		break;

	case K40:
		arg_storcl(RARG_STO_MAX - RARG_STO, 0);
		break;

	case K50:
		arg_storcl(RARG_STO_MIN - RARG_STO, 0);
		break;

	case K20:				// Enter is a short cut finisher but it also changes a few commands if it is first up
		if (State2.numdigit == 0 && !State2.ind && !State2.dot) {
			if (argcmds[base].label) {
				init_arg((enum rarg)(base - RARG_LBL));
				State2.multi = 1;
				State2.alphashift = 0;
				State2.rarg = 0;
			} else if (base == RARG_SCI) {
				reset_arg();
				return OP_NIL | OP_FIXSCI;
			} else if (base == RARG_ENG) {
				reset_arg();
				return OP_NIL | OP_FIXENG;
			} else if (argcmds[base].stckreg)
				State2.dot = 1;
		} else if (State2.numdigit > 0)
			return arg_eval(State2.digval);
		else if (stack_reg)
			State2.dot = 1 - State2.dot;
		break;

	case K24:	// <-
		if (State2.numdigit == 0) {
			if (State2.dot)
				State2.dot = 0;
			else if (State2.local)
				State2.local = 0;
			else if (State2.ind)
				State2.ind = 0;
			else
				goto reset;
		}
		else {
			--State2.numdigit;
			if (State2.shuffle)
				State2.digval &= ~(3 << (State2.numdigit << 1));
			else
				State2.digval /= 10;
		}
		break;

	case K60:
	reset:
		reset_arg();
	default:
		break;
	}
	return STATE_UNFINISHED;
}


/*
 *  Process arguments to the diverse test commands
 */
static int process_test(const keycode c) {
	int r = State2.test;
	int cmpx = State2.cmplx;
	unsigned int n = keycode_to_digit_or_register(c) & ~NO_SHORT;
	unsigned int base = (cmpx ? RARG_TEST_ZEQ : RARG_TEST_EQ) + r;

	State2.test = TST_NONE;
	State2.cmplx = 0;
	if (n != NO_REG && n >= TOPREALREG && n < LOCAL_REG_BASE ) {
		// Lettered register
		if (cmpx && (n & 1)) {
			if (n == regI_idx)
				return OP_SPEC + OP_Zeqi + r;
			// Disallow odd complex registers > A
			goto again;
		}
		// Return the command with the register completed
		return RARG(base, n);
	}
	else if ( n == 0 ) {
		// Special 0
		return OP_SPEC + (cmpx ? OP_Zeq0 : OP_Xeq0) + r;
	}
	else if ( n == 1 ) {
		// Special 1
		return OP_SPEC + (cmpx ? OP_Zeq1 : OP_Xeq1) + r;
	}
	else if ( n <= 9 || c == K_ARROW || c == K62 ) {
		// digit 2..9, -> or .
		init_arg((enum rarg)base);
		return process_arg(c);
	}

	switch (c) {
	case K11:					// tests vs register
	case K20:
		init_arg((enum rarg)base);
		return STATE_UNFINISHED;

	//case K60:
	case K24:
		return STATE_UNFINISHED;

	default:
		break;
	}
again:
	State2.test = r;
	State2.cmplx = cmpx;
	return STATE_UNFINISHED;
}

#ifdef INCLUDE_USER_CATALOGUE
/*
 *  Build the user catalogue on the fly in RAM and return the number of entries
 */
#define USER_CAT_MAX 100
s_opcode UserCat[USER_CAT_MAX];

static int build_user_cat(void)
{
	// find the label 'CAT'
	const int lbl = OP_DBL + (DBL_LBL << DBL_SHIFT) + 'C' + ('A' << 16) + ('T' << 24);
	unsigned int pc = findmultilbl(lbl, 0);
	int len = 0;
	while (pc && len < USER_CAT_MAX) {
		// do a simnple insert-sort to sort the entries
		char buf1[16];
		int i;
		s_opcode op;
	next:
		pc = do_inc(pc, 0);
		op = (s_opcode) getprog(pc);
		if (op == (OP_NIL | OP_END))
			break;
		if (isDBL(op))
			continue;
		if (isRARG(op)) {
			const s_opcode rarg = RARG_CMD(op);
			if (rarg != RARG_ALPHA && rarg != RARG_CONV
			    && rarg != RARG_CONST && rarg != RARG_CONST_CMPLX)
				op = op & 0xff00;	// remove argument
		}
		catcmd(op, buf1);
		for (i = 0; i < len; ++i) {
			// Find a position in the table to insert the new entry
			char buf2[16];
			const char *p, *q;
			int diff = 0;
			if (op == UserCat[i]) {
				// duplicate entry - ignore
				goto next;
			}
			p = buf1;
			if (*p == COMPLEX_PREFIX)
				++p;
			q = catcmd(UserCat[i], buf2);
			if (*q == COMPLEX_PREFIX)
				++q;
			diff = 0;
			while (*p != '\0' && diff == 0) {
				diff = remap_chars(*q++) - remap_chars(*p++);
			}
			if ((diff == 0 && *q == 0) ) {
				// identical according to sort order, insert after
				++i;
				break;
			}
			if (diff > 0) {
				// insert new entry before the one found because this is greater
				break;
			}
		}
		if (i < len) {
			// Make room
			xcopy(UserCat + i + 1, UserCat + i, (len - i) << 1);
		}
		UserCat[i] = op;
		++len;
	}
	// return the number of entries
	return len;
}
#endif

/* Return the number of entries in the current catalogue.
 * These are all fixed size known at compile time so a table lookup will
 * likely be the most space efficient method.
 */
int current_catalogue_max(void) {
	// A quick table of catalogue sizes
	// NB: the order here MUST match that in `enum catalogues' 
	static const unsigned char catalogue_sizes[] = 
	{
		0, // NONE
		SIZE_catalogue,
		SIZE_cplx_catalogue,
		SIZE_stats_catalogue,
		SIZE_prob_catalogue,
		SIZE_int_catalogue,
		SIZE_prog_catalogue,
		SIZE_program_xfcn,
		SIZE_test_catalogue,
		SIZE_mode_catalogue,
		SIZE_alpha_catalogue,
		SIZE_alpha_symbols,
		SIZE_alpha_compares,
		SIZE_alpha_arrows,
		SIZE_alpha_letters,
		SIZE_alpha_subscripts,
		NUM_CONSTS_CAT,
		NUM_CONSTS_CAT,
		SIZE_conv_catalogue,
		SIZE_sums_catalogue,
		SIZE_matrix_catalogue,
#ifdef INCLUDE_INTERNAL_CATALOGUE
		SIZE_internal_catalogue,
#endif
	};
#ifdef INCLUDE_USER_CATALOGUE
	const int cat = State2.catalogue;
	return cat == CATALOGUE_USER ? build_user_cat() : catalogue_sizes[cat];
#else
	return catalogue_sizes[State2.catalogue];
#endif
}


/* Look up the character position in the given byte array and
 * build the alpha op-code for it.
 */
static opcode alpha_code(int n, const char tbl[]) {
	return RARG(RARG_ALPHA, tbl[n] & 0xff);
}


/* Return the opcode for entry n from the current catalogue
 */
opcode current_catalogue(int n) {
	// A quick table of catalogue tables
	// NB: the order here MUST match that in `enum catalogues'
	static const void *catalogues[] =
	{
		NULL, // NONE
		catalogue,
		cplx_catalogue,
		stats_catalogue,
		prob_catalogue,
		int_catalogue,
		prog_catalogue,
		program_xfcn,
		test_catalogue,
		mode_catalogue,
		alpha_catalogue,
		alpha_symbols,
		alpha_compares,
		alpha_arrows,
		alpha_letters,
		alpha_subscripts,
		NULL,
		NULL,
		NULL, //CONV
		sums_catalogue,
		matrix_catalogue,
#ifdef INCLUDE_INTERNAL_CATALOGUE
		internal_catalogue,
#endif
	};
	const unsigned char *cat;
	unsigned int c = State2.catalogue;
	int m, i;
	unsigned p, q;

	if (c == CATALOGUE_CONST) {
		if (n == OP_ZERO)
			return RARG_BASEOP(RARG_INTNUM);
		return CONST(n);
	}
	if (c == CATALOGUE_COMPLEX_CONST) {
		if (n == OP_ZERO)
			return RARG_BASEOP(RARG_INTNUM_CMPLX);
		return CONST_CMPLX(n);
	}
	if (c == CATALOGUE_CONV) {
		const int cnv = conv_catalogue[n];
		if (cnv >= SIZE_conv_catalogue)
			// Monadic conversion routine
			return OP_MON | (cnv - SIZE_conv_catalogue);
		else
			return RARG(RARG_CONV, cnv);
	}
#ifdef INCLUDE_USER_CATALOGUE
	if (c == CATALOGUE_USER)
		return build_user_cat() ? UserCat[n] : STATE_IGNORE;
#endif

	if (c == CATALOGUE_ALPHA_LETTERS && State2.alphashift)
		cat = (const unsigned char *) alpha_letters_lower;
	else
		cat = (const unsigned char *) catalogues[c];

	if (c >= CATALOGUE_ALPHA_SYMBOLS && c <= CATALOGUE_ALPHA_SUBSCRIPTS) {
		return alpha_code(n, (const char *) cat);
	}
	if (c >= sizeof(catalogues) / sizeof(void *))
		return OP_NIL | OP_NOP;

	/* Unpack the opcode */
	cat += n + (n >> 2);
	p = cat[0];
	q = cat[1];
	m = 0x3ff & ((p << (2 + ((n & 3) << 1))) | (q >> (6 - ((n & 3) << 1))));

	/* Now figure out which opcode it really is */
	for (i=0; i<KIND_MAX; i++) {
		if (m < opcode_breaks[i])
			return (i << KIND_SHIFT) + m;
		m -= opcode_breaks[i];
	}
	return RARG_BASEOP(m);
}


/*
 *  Helper for navigation in alpha catalogues. Some charaters are not allowed
 *  in multi character commands.
 */
static int forbidden_alpha(int pos) {
	return (current_catalogue(pos) & 0xf0) == 0xf0;
}

/*
 *  Catalogue navigation
 */
static int process_catalogue(const keycode c, const enum shifts shift, const int is_multi) {
	int pos = State.catpos;
	int ch;
	const int ctmax = current_catalogue_max();
	const enum catalogues cat = (enum catalogues) State2.catalogue;

	if (shift == SHIFT_N) {
		switch (c) {
		case K30:			// XEQ accepts command
		case K20:			// Enter accepts command
			if (pos < ctmax && !(is_multi && forbidden_alpha(pos))) {
				const opcode op = current_catalogue(pos);

				init_cat(CATALOGUE_NONE);

				if (isRARG(op)) {
					const unsigned int rarg = RARG_CMD(op);
					if (rarg == RARG_CONST || rarg == RARG_CONST_CMPLX || rarg == RARG_CONV || rarg == RARG_ALPHA)
						return op;
					if (rarg >= RARG_TEST_EQ && rarg <= RARG_TEST_GE)
						State2.test = TST_EQ + (RARG_CMD(op) - RARG_TEST_EQ);
					else
						init_arg(RARG_CMD(op));
				}
				else {
					return check_confirm(op);
				}
			} else
				init_cat(CATALOGUE_NONE);
			return STATE_UNFINISHED;

		case K24:			// backspace
			if (CmdLineLength > 0 && Keyticks < 30) {
				if (--CmdLineLength > 0)
					goto search;
				pos = 0;
				goto set_pos;
			} else
				init_cat(CATALOGUE_NONE);
			return STATE_UNFINISHED;

		case K60:
			init_cat(CATALOGUE_NONE);
			return STATE_UNFINISHED;

		case K40:
			CmdLineLength = 0;
			if (pos == 0)
				goto set_max;
			else
				--pos;
			goto set_pos;

		case K50:
			CmdLineLength = 0;
			while (++pos < ctmax && is_multi && forbidden_alpha(pos));
			if (pos >= ctmax)
				pos = 0;
			goto set_pos;

		default:
			break;
		}
	} else if (shift == SHIFT_F) {
		if (cat == CATALOGUE_CONV && c == K01) {
			/*
			 * f 1/x in conversion catalogue
			 */
			/* A small table of commands in pairs containing inverse commands.
			 * This table could be unsigned characters only storing the monadic kind.
			 * this saves twelve bytes in the table at a cost of some bytes in the code below.
			 * Not worth it since the maximum saving will be less than twelve bytes.
			 */
			static const unsigned short int conv_mapping[] = {
				OP_MON | OP_AR_DB,	OP_MON | OP_DB_AR,
				OP_MON | OP_DB_PR,	OP_MON | OP_PR_DB,
				OP_MON | OP_DEGC_F,	OP_MON | OP_DEGF_C,
				OP_MON | OP_DEG2RAD,	OP_MON | OP_RAD2DEG,
				OP_MON | OP_DEG2GRD,	OP_MON | OP_GRD2DEG,
				OP_MON | OP_RAD2GRD,	OP_MON | OP_GRD2RAD,
			};
			const opcode op = current_catalogue(pos);
			int i;

			init_cat(CATALOGUE_NONE);
			if (isRARG(op))
				return op ^ 1;
			for (i = 0; i < sizeof(conv_mapping) / sizeof(conv_mapping[0]); ++i)
				if (op == conv_mapping[i])
					return conv_mapping[i^1];
			return STATE_UNFINISHED;		// Unreached
		}
		else if (c == K60 && (State2.alphas || State2.multi)) {
			// Handle alpha shift in alpha character catalogues
			State2.alphashift = 1 - State2.alphashift;
			return STATE_UNFINISHED;
		}
	} else if (shift == SHIFT_G) {
		if (c == K24 && cat == CATALOGUE_SUMS) {
			init_cat(CATALOGUE_NONE);
			return OP_NIL | OP_SIGMACLEAR;
		}
	}

	/* We've got a key press, map it to a character and try to
	 * jump to the appropriate catalogue entry.
	 */
	ch = remap_chars(keycode_to_alpha(c, shift == SHIFT_G ? SHIFT_LC_G : shift));
	reset_shift();
	if (ch == '\0')
		return STATE_UNFINISHED;
	if (cat > CATALOGUE_ALPHA && cat < CATALOGUE_CONST) {
		// No multi character search in alpha catalogues
		CmdLineLength = 0;
	}
	if (CmdLineLength < 10)
		Cmdline[CmdLineLength++] = ch;
	/* Search for the current buffer in the catalogue */

search:
	Cmdline[CmdLineLength] = '\0';
	for (pos = 0; pos < ctmax; ++pos) {
		char buf[16];
		const char *cmd = catcmd(current_catalogue(pos), buf);
		int i;

		if (*cmd == COMPLEX_PREFIX)
			cmd++;
		for (i=0; cmd[i] != '\0'; i++) {
			const int c = remap_chars(cmd[i]);
			const int cl = (unsigned char) Cmdline[i];
			if (c > cl)
				goto set_pos;
			else if (c < cl)
				break;
		}
		if (Cmdline[i] == '\0')
			goto set_pos;
	}
set_max:
	pos = ctmax - 1;
set_pos:
	while (is_multi && pos && forbidden_alpha(pos))
		--pos;
	State.catpos = pos;
	return STATE_UNFINISHED;
}

#ifndef REALBUILD
int find_pos(const char* text) {
	int pos;
	const int ctmax = current_catalogue_max();
	for (pos = 0; pos < ctmax; ++pos) {
		char buf[16];
		const char *cmd = catcmd(current_catalogue(pos), buf);
		int i;

		if (*cmd == COMPLEX_PREFIX)
			cmd++;
		for (i=0; cmd[i] != '\0'; i++) {
			const int c = remap_chars(cmd[i]);
			const int cl = remap_chars(text[i]);
			if (c > cl)
				return pos;
			else if (c < cl)
				break;
		}
		if (text[i] == '\0')
			return pos;
	}
	return pos;
}

#endif

/* Multi (2) word instruction entry
 */
static void reset_multi(void) {
	// Reset the multi flag and clear lowercase flag if not called from alpha mode
	State2.multi = 0;
	if (! State2.alphas )
		State2.alphashift = 0;
}

static int process_multi(const keycode c) {
	enum shifts shift = reset_shift();
	unsigned int ch = 0;
	unsigned int opcode;

	if (State2.catalogue) {
		// Alpha catalogue from within multi character command
		opcode = process_catalogue((const keycode)c, shift, State2.numdigit == 2);
		if (opcode == STATE_UNFINISHED)
			return opcode;
		ch = (unsigned char) opcode;
		goto add_char;
	}

	switch (c) {
	case K20:	// Enter - exit multi mode, maybe return a result
		if (shift != SHIFT_N)
				break;
		reset_multi();
		if (State2.numdigit == 0)
			return STATE_UNFINISHED;
		else if (State2.numdigit == 1)
			State2.digval2 = 0;
		goto fin;

	case K24:	// Clx - backspace, clear alpha
		if (shift != SHIFT_H) {
			if (State2.numdigit == 0)
				reset_multi();
			else
				State2.numdigit--;
			return STATE_UNFINISHED;
		}
		break;

	case K60:	// EXIT/ON maybe case switch, otherwise exit alpha
		if (shift == SHIFT_F)
			State2.alphashift = 1 - State2.alphashift;
		else
			reset_multi();
			return STATE_UNFINISHED;

	default:
		break;
		}

	/* Look up the character and return an alpha code if okay */
	ch = keycode_to_alpha(c, shift);
	if (ch == 0)
		return STATE_UNFINISHED;
add_char:
	if (State2.numdigit == 0) {
		State2.digval = ch;
		State2.numdigit = 1;
		return STATE_UNFINISHED;
	} else if (State2.numdigit == 1) {
		State2.digval2 = ch;
		State2.numdigit = 2;
		return STATE_UNFINISHED;
	}
	reset_multi();

fin:
	opcode = OP_DBL + (CmdBase << DBL_SHIFT) 
	       + State2.digval + (State2.digval2 << 16) + (ch << 24);
	return opcode;
}


/* Handle YES/NO confirmations
 */
static int process_confirm(const keycode c) {
	// Optimization hint: a switch is shorter then a table of function pointers!
	switch (c) {
	case K63:			// Yes
		switch (State2.confirm) {
		case confirm_clall:	 clrall();	break;
		case confirm_reset:	 reset();	break;
		case confirm_clprog:	 clrprog();	break;
		case confirm_clpall:	 clpall();	break;
		}
	case K24:
	case K32:			// No
		State2.confirm = 0;
		State2.digval = 0;
		break;
	default:			// No state change
		break;
	}
	return STATE_UNFINISHED;
}


/*
 *  STATUS
 */
static int process_status(const keycode c) {
	int n = ((int)State2.status) - 3;
	int max = LocalRegs < 0 ? 11 : 10;

	if (c == K40) {
		if (--n < -2)
			n = max;
	}
	else if (c == K50) {
		if (++n > max)
			n = -2;
	}
	else if (c == K24 /* || c == K60 */) {
		State2.status = 0;
		return STATE_UNFINISHED;
	} 
	else {
		int nn = keycode_to_digit_or_register(c) & 0x7f;
		if (nn <= 9)
			n = nn;
		else if (nn == LOCAL_REG_BASE)
			n = n == max ? 10 : max;
		else if (nn != NO_REG)
			n = 10; 
	}
	State2.status = n + 3;

	return STATE_UNFINISHED;
}


/*
 *  CAT helper
 */
static int is_label_or_end_at(unsigned int pc, int search_end) {
	const unsigned int op = getprog(pc);

	return op == (OP_NIL | OP_END) || (!search_end && (isDBL(op) && opDBL(op) == DBL_LBL));
}

static unsigned int advance_to_next_label(unsigned int pc, int inc, int search_end) {
	do {
		for (;;) {
			if (inc) {
				pc = do_inc(pc, 0);
				if (PcWrapped)
					break;
			}
			else
				inc = 1;
			if (is_label_or_end_at(pc, search_end)) {
				return pc;
			}
		}
		pc = addrLIB(1, (nLIB(pc) + 1) & 3);
	} while (! is_label_or_end_at(pc, search_end));
	return pc;
}

static unsigned int advance_to_previous_label(unsigned int pc, int search_end) {
	do {
		for (;;) {
			pc = do_dec(pc, 0);
			if (PcWrapped)
				break;
			if (is_label_or_end_at(pc, search_end)) {
				return pc;
			}
		}
		pc = addrLIB(1, (nLIB(pc) - 1) & 3);
		pc = do_dec(pc, 0);
	} while (! is_label_or_end_at(pc, search_end));
	return pc;
}


/*
 *  CAT command
 */
static int process_labellist(const keycode c) {
	unsigned int pc = State2.digval;
	const unsigned int n = c == K62 ? REGION_XROM 
		                        : keycode_to_digit_or_register(c) & ~NO_SHORT;
	const int opcode = getprog(pc);
	const int label = isDBL(opcode) ? (getprog(pc) & 0xfffff0ff) : 0;
	const int direct = State2.runmode;
	const enum shifts shift = reset_shift();
	int op = STATE_UNFINISHED;

	if (n < REGION_XROM) {
		// Digits take you to that segment
		pc = addrLIB(1, n);
		if (! is_label_or_end_at(pc, 0))
			pc = advance_to_next_label(pc, 1, 0);
		State2.digval = pc;
		return STATE_UNFINISHED;
	}

	switch (c | (shift << 8)) {

	case K40 | (SHIFT_F << 8):		// Find first label of previous program
		pc = advance_to_previous_label(advance_to_previous_label(pc, 1), 1);
		goto next;

	case K50 | (SHIFT_F << 8):		// Find next program
		pc = advance_to_next_label(pc, 0, 1);
	case K50:				// Find next label
	next:
		State2.digval = advance_to_next_label(pc, 1, 0);
		return STATE_UNFINISHED;

	case K40:				// Find previous label
		State2.digval = advance_to_previous_label(pc, 0);
		return STATE_UNFINISHED;

	case K24:				// <- exits
		break;

	case K20:				// ENTER^
	set_pc_and_exit:
		set_pc(pc);			// forced branch
		break;

	case K24 | (SHIFT_F << 8):		// CLP
		op = (OP_NIL | OP_CLPROG);
		goto set_pc_and_exit;

	case K10:				// STO
	case K11:				// RCL
		op = c == K10 ? (OP_NIL | OP_PSTO) : (OP_NIL | OP_PRCL);
		goto set_pc_and_exit;

	case K30:				// XEQ
		op = (DBL_XEQ << DBL_SHIFT) + label;
		goto xeq_or_gto;

	case K30 | (SHIFT_H << 8):		// GTO
		op = (DBL_GTO << DBL_SHIFT) + label;
	xeq_or_gto:
		if (label)
			break;
		return STATE_UNFINISHED;

	case K63:				// R/S
		if (direct && label) {
			cmdgtocommon(1, pc);	// set pc and push return address
			op = STATE_RUNNING;	// quit the browser, start program
			break;
		}
		return STATE_UNFINISHED;

	case K63 | (SHIFT_H << 8):		// P/R
		State2.runmode = 0;		// switch to program mode
		goto set_pc_and_exit;

	default:
		return STATE_UNFINISHED;
	}
	State2.digval = 0;
	State2.labellist = 0;
	return check_confirm(op);
}


static void set_window(int c) {

	if (State2.runmode) {
		process_cmdline_set_lift();
		// Make sure IntMaxWindow is recalculated
		State2.disp_freeze = 0;
		display();	
		if (c == STATE_WINDOWRIGHT) {
			if (UState.intm) {
				if (IntMaxWindow > 0 && State2.window > 0)
					State2.window--;
				return;
			}
			else 
				State2.window = is_dblmode();
		}
		else {
			if (UState.intm) {
				if (IntMaxWindow > (SMALL_INT) State2.window)
					State2.window++;
				return;
			}
			else
				State2.window = 0;
		}
		set_smode(SDISP_SHOW);
	}
}


static int process_registerlist(const keycode c) {
	unsigned int n = keycode_to_digit_or_register(c) & ~NO_SHORT;
	enum shifts shift = reset_shift();
	const int max = State2.local ? local_regs() : NUMREG;
	const int g_max = global_regs();

	if (n == LOCAL_REG_BASE) {	// '.'
		if (local_regs())
			State2.local = ! State2.local && ! State2.digval2;
		State2.digval = State2.local ? 0 : regX_idx;
		goto reset_window;
	}
	else if (n <= 9) {
		int dv = (State2.digval * 10 + n) % 100;
		if (dv >= max || (! State2.local && (State2.digval >= g_max || dv >= g_max)))
			dv = n;
		State2.digval = dv;
		goto reset_window;
	}
	else if ((shift == SHIFT_F || shift == SHIFT_G) && c == K21) {  // <( )>
		set_window(shift == SHIFT_F ? STATE_WINDOWLEFT : STATE_WINDOWRIGHT);
		set_smode(SDISP_SHOW);
	}
	else if (n != NO_REG) {
		State2.digval = n;
		goto reset_window;
	}

	switch (c) {
	case K50:
		if (State2.digval > 0) {
			if (! State2.local && State2.digval == TOPREALREG)
				State2.digval = global_regs();
			--State2.digval;
		}
		else
			State2.digval = max - 1;
		goto reset_window;

	case K40:
		if (State2.digval < max - 1) {
			State2.digval++;
			if (! State2.local && State2.digval == global_regs())
				State2.digval = regX_idx;
		}
		else	
			State2.digval = 0;
		goto reset_window;

#ifdef INCLUDE_FLASH_RECALL
	case K04:
		State2.digval2 = ! State2.digval2 && ! State2.local;
		goto reset_window;
#endif

	case K24:			
	//case K60:
		if (State2.disp_temp)
			return STATE_UNFINISHED;
		break;		// Exit doing nothing

	case K20:		// ENTER
		if (shift == SHIFT_F) {
			State2.disp_as_alpha = 1;
			goto reset_window;
		}
	case K11:		// RCL
		if ( shift == SHIFT_N ) {
#ifdef INCLUDE_FLASH_RECALL
			n = RARG( State2.digval2 ? RARG_FLRCL : RARG_RCL, State2.digval );
#else
			n = RARG( RARG_RCL, State2.digval );
#endif
			State2.registerlist = 0;
			State2.digval = 0;
			State2.digval2 = 0;
			return n;
		}
	default:
		return STATE_UNFINISHED;
	}
	State2.registerlist = 0;
	State2.digval = 0;
	State2.digval2 = 0;
reset_window:
	State2.window = 0;
	return STATE_UNFINISHED;
}


static int process(const int c) {
	const enum shifts shift = cur_shift();
	enum catalogues cat;

	if (Running || Pause) {
		/*
		 *  Abort a running program with R/S or EXIT
		 */
		if (c == K60 || c == K63) {
			if (Pause && isXROM(state_pc()))
				set_pc(0);
			Pause = 0;
			xeq_xrom();
			set_running_off();
			DispMsg = "Stopped";
			State2.disp_freeze = 0;
			return STATE_UNFINISHED;
		}
		if ( c != K_HEARTBEAT ) {
			LastKey = (char) (c + 1);	// Store for KEY?
			Pause = 0;			// leave PSE statement
			GoFast = 1;
		}
		// continue execution if really running, else ignore (PSE)
		return STATE_RUNNING;
	}

	/* Check for ON in the unshifted state -- this is a reset sequence
	 * common across all modes.  Shifted modes need to check this themselves
	 * if required.
	 */
	if (c == K60 && shift == SHIFT_N && ! State2.catalogue) {
		soft_init_state();
		return STATE_UNFINISHED;
	}

#ifndef CONSOLE
	if (c == K63 && JustStopped) {
		// Avoid an accidental restart with R/S
		JustStopped = 0;
		return STATE_IGNORE;
	}
#endif
	/*  Handle the keyboard timeout for catalogue navigation
	 *  Must be done early in the process to capture the shifts correctly
	 */
	if (State2.catalogue && Keyticks > 30)
		CmdLineLength = 0;

	/*
	 *  Process the various cases
	 *  The handlers in this block here do not handle shifts at all or do it themselves
	 */
	if (State2.confirm)
		return process_confirm((const keycode)c);

	if (State2.rarg)
		return process_arg((const keycode)c);

	if (State2.gtodot)
		return process_gtodot((const keycode)c);

	if (State2.hyp)
		return process_hyp((const keycode)c);

	if (State2.test != TST_NONE)
		return process_test((const keycode)c);

	if (State2.status)
		return process_status((const keycode)c);

	/*
	 *  Process shift keys directly
	 */
	if (c == K_F) {
		toggle_shift(SHIFT_F);
		return STATE_UNFINISHED;
	}
	if (c == K_G) {
		toggle_shift(SHIFT_G);
		return STATE_UNFINISHED;
	}
	if (c == K_H) {
		toggle_shift(SHIFT_H);
		return STATE_UNFINISHED;
	}

	/*
	 *  The handlers in this block need to call reset_shift somewhere
	 */

	/*
	 * Here the keys are mapped to catalogues
	 * The position of this code decides where catalogue switching
	 * works and were not
	 */
	cat = keycode_to_cat((keycode)c, shift);
	if ( cat != CATALOGUE_NONE ) {
		init_cat( CATALOGUE_NONE );
		init_cat( cat );
		State2.arrow = 0;
		return STATE_UNFINISHED;
	}

	/*
	 *  More handlers...
	 */
	if (State2.multi)
		return process_multi((const keycode)c);

	if (State2.arrow)
		return process_arrow((const keycode)c);

	if (State2.labellist)
		return process_labellist((const keycode)c);

	if (State2.registerlist)
		return process_registerlist((const keycode)c);

	if (State2.catalogue)
		return process_catalogue((const keycode)c, reset_shift(), 0);

	if (State2.alphas) {
#ifndef INFRARED
		return process_alpha((const keycode)c);
#else
		int i = process_alpha((const keycode)c);
		if (! State2.alphas && get_user_flag(T_FLAG)) {
			print_tab(0);
			print_alpha(OP_PRINT_ALPHA);
		}
		return i;
#endif
	}

	if (State2.cmplx) {
		return process_cmplx((const keycode)c);
	} else {
		if (shift == SHIFT_F || shift == SHIFT_G)
			return process_fg_shifted((const keycode)c);
		if (shift == SHIFT_H)
			return process_h_shifted((const keycode)c);
		return process_normal((const keycode)c);
	}
}


/*
 *  Fed with key codes by the event loop
 */
void process_keycode(int c)
{
	static int was_paused;

	if (was_paused && Pause == 0) {
		/*
		 *  Continue XROM execution after a pause
		 */
		xeq_xrom();
	}
	was_paused = Pause;

	if (c == K_HEARTBEAT) {
		/*
		 *  Heartbeat processing goes here.
		 *  This is totally thread safe!
		 */
		if (Keyticks >= 2) {
			/*
			 *  Some time has passed after last key press
			 */
			if (OpCode != 0) {
				/*
				 *  Handle command display and NULL here
				 */
				if (OpCodeDisplayPending) {
					/*
					 *  Show command to the user
					 */
					OpCodeDisplayPending = 0;
					if (OpCode == (OP_NIL | OP_RS)) {
						DispMsg = "RUN";
					}
					else {
						scopy_char(TraceBuffer, prt(OpCode, TraceBuffer), '\0');
						DispMsg = TraceBuffer;
					}
					display();
					ShowRPN = 1;	// Off because of DispMsg setting
				}
				else if (Keyticks > 12) {
					/*
					 *  Key is too long held down
					 */
					OpCode = 0;
					message("NULL", CNULL);
					// Force display update on key-up
					State2.disp_temp = 0;
				}
			}
			if (Keyticks > 12 && shift_down() != SHIFT_N) {
				// Rely on the held shift key instead of the toggle
				State2.shifts = SHIFT_N;
			}
		}

		/*
		 *  Serve the watchdog
		 */
		watchdog();

#ifndef CONSOLE
		/*
		 *  If buffer is empty re-allow R/S to start a program
		 */
		if (JustStopped && !is_key_pressed()) {
			JustStopped = 0;
		}
#endif

		/*
		 *  Do nothing if not running a program
		 */
		if (!Running && !Pause)
			return;
	}
	else {
		/*
		 *  Not the heartbeat - prepare for execution of any commands
		 */
		xeq_init_contexts();
		State2.wascomplex = 0;

		if (is_dot(RPN)) {
			/*
			 * Turn off the RPN annunciator as a visual feedback
			 */
			clr_dot(RPN);
			finish_display();
		}

#ifndef CONSOLE
		/*
		 *  Reallow display refresh which is temporarily disabled after a stop
		 *  All keys execpt R/S trigger this. The latter will only be reenabled
		 *  from the heartbeat after the keybord buffer has become empty to avoid
		 *  an accidental restart of the program.
		 */
		if (c != K63)
			JustStopped = 0;
#endif
	}

	/*
	 *  Handle key release
	 */
	if (c == K_RELEASE) {
		if (OpCode != 0) {
			/*
			 * Execute the key on release
			 */
			GoFast = 1;
			c = OpCode;
			OpCode = 0;

			if (c == STATE_SST)
				xeq_sst_bst(1);
			else {
				xeq(c);
				if (Running || Pause)
					xeqprog();
			}
			dot(RPN, ShowRPN);
		}
		else {
			// Ignore key-up if no operation was pending
			dot(RPN, ShowRPN);
#ifndef CONSOLE
			if (! State2.disp_temp ) {
				// This will get rid of the last displayed op-code
				display();
			}
#endif
			return;
		}

		/*
		 *  Turn on the RPN symbol if desired
		 */
		if (ShowRPN) {
			finish_display();
		}
	}
	else {
		/*
		 *  Decode the key 
		 */
		ShowRPN = ! Running;	// Default behaviour, may be turned off later

		c = process(c);		// returns an op-code or state
		switch (c) {
		case STATE_SST:
			OpCode = c;
			OpCodeDisplayPending = 0;
			xeq_sst_bst(0);
			break;

		case STATE_BST:
			xeq_sst_bst(-1);
			break;

		case STATE_BACKSPACE:
			if (! State2.runmode)
				delprog();
			else if (State2.alphas) {
				char *p = find_char(Alpha, '\0');
				if (p > Alpha)
					*--p = '\0';
			}
			break;

		case STATE_RUNNING:
			xeqprog();  // continue execution
			break;

		case STATE_WINDOWRIGHT:
		case STATE_WINDOWLEFT:
			set_window(c);
			break;

		case STATE_UNFINISHED:
		case STATE_IGNORE:
			break;

		default:
			if (State2.runmode || NonProgrammable) {
				NonProgrammable = 0;
				if (c >= (OP_SPEC | OP_ENTER) && c <= (OP_SPEC | OP_F))
					// Data entry key
					xeq(c);
				else {
					// Save the op-code for execution on key-up
					OpCode = c;
					OpCodeDisplayPending = 1;
				}
			}
			else {
				stoprog(c);
			}
		}
	}
#ifndef CONSOLE
	if (! Running && ! Pause && ! JustStopped && ! JustDisplayed && c != STATE_IGNORE) {
		display();
	}
#else
	if (! Running && ! Pause && c != STATE_IGNORE && ! JustDisplayed) {
		display();
	}
#endif
        JustDisplayed = 0;
        watchdog();
}

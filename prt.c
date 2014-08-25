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

#include "xeq.h"
#include "display.h"
#include "consts.h"


static const char *prt_monadic(const unsigned int f, char *instr) {
	if (f < NUM_MONADIC && (! isNULL(monfuncs[f].mondreal) || ! isNULL(monfuncs[f].monint)))
		return sncopy(instr, monfuncs[f].fname, NAME_LEN);
	return "???";
}

static const char *prt_dyadic(const unsigned int f, char *instr) {
	if (f < NUM_DYADIC && (! isNULL(dyfuncs[f].dydreal) || ! isNULL(dyfuncs[f].dydint)))
		return sncopy(instr, dyfuncs[f].fname, NAME_LEN);
	return "???";
}

static const char *prt_triadic(const unsigned int f, char *instr) {
	if (f < NUM_TRIADIC && (! isNULL(trifuncs[f].trireal) || ! isNULL(trifuncs[f].triint)))
		return sncopy(instr, trifuncs[f].fname, NAME_LEN);
	return "???";
}

static const char *prt_cmplx(int add_prefix, const char *name, char *instr) {
	char *p = instr;
	if (add_prefix)
		*p++ = COMPLEX_PREFIX;
	sncopy(p, name, NAME_LEN);
	return instr;
}

static const char *prt_monadic_cmplx(const unsigned int f, char *instr) {
	if (f < NUM_MONADIC && ! isNULL(monfuncs[f].mondcmplx))
		return prt_cmplx(! isNULL(monfuncs[f].mondreal), monfuncs[f].fname, instr);
	return "???";
}

static const char *prt_dyadic_cmplx(const unsigned int f, char *instr) {
	if (f < NUM_DYADIC && ! isNULL(dyfuncs[f].dydcmplx))
		return prt_cmplx(! isNULL(dyfuncs[f].dydreal), dyfuncs[f].fname, instr);
	return "???";
}

static const char *prt_niladic(const unsigned int idx, char *instr) {
	if (idx < NUM_NILADIC) {
		const char *p = niladics[idx].nname;
		return sncopy(instr, p, NAME_LEN);
	}
	return "???";
}

static const char *prt_tst(const char r, const enum tst_op op, char *instr, int cmplx) {
	char *p = instr;
	if (cmplx)
		*p++ = COMPLEX_PREFIX;
	*p++ = 'x';
	*p++ = "=\013\035<\011>\012"[op];
	*p++ = r;
	*p++ = '?';
	return instr;
}

static const char *prt_specials(unsigned int opm, char *instr) {
	static const char digits[16] = "0123456789ABCDEF";
	char cmp = '0';

	if (opm >= OP_0 && opm <= OP_F) {
		*instr = digits[opm - OP_0];
		return instr;
	}
	if (opm >= OP_Xeq1 && opm <= OP_Xge1) {
		opm += OP_Xeq0 - OP_Xeq1;
		cmp = '1';
	}
	if (opm >= OP_Xeq0 && opm <= OP_Xge0)
		return prt_tst(cmp, (enum tst_op)(opm - OP_Xeq0), instr, 0);

	switch (opm) {
#ifdef COMPILE_CATALOGUES
	case OP_DOT:	return ".";
#else
	case OP_DOT:	return UState.fraccomma ? "," : ".";
#endif
	case OP_CHS:	return "+/-";
	//case OP_CLX:	return "CLx";
	case OP_ENTER:	return "ENTER\020";
	case OP_EEX:	return "EEX";
	case OP_SIGMAPLUS:	return "\221+";
	case OP_SIGMAMINUS:	return "\221-";

	case OP_Zeq0:	case OP_Zne0:	//case OP_Zapx0:
		return prt_tst('0', (enum tst_op)(opm - OP_Zeq0), instr, 1);

	case OP_Zeq1:	case OP_Zne1:	//case OP_Zapx1:
		return prt_tst('1', (enum tst_op)(opm - OP_Zeq1), instr, 1);

	case OP_Zeqi:	case OP_Znei:
		return prt_tst('i', (enum tst_op)(opm - OP_Zeqi), instr, 1);
	}
	return "???";
}


/* Metric <-> imperial conversions */
static const char *prt_conv(unsigned int arg, char *instr) {
	const unsigned int conv = arg / 2;
	const unsigned int dirn = arg & 1;

	if (conv >= NUM_CONSTS_CONV)
		return "???";
	if (dirn == 0) {		// metric to imperial
		sncopy(sncopy_char(instr, cnsts_conv[conv].metric, METRIC_NAMELEN, '\015'),
				cnsts_conv[conv].imperial, IMPERIAL_NAMELEN);
	} else {			// imperial to metric
		sncopy(sncopy_char(instr, cnsts_conv[conv].imperial, IMPERIAL_NAMELEN, '\015'),
				cnsts_conv[conv].metric, METRIC_NAMELEN);
	}
	return instr;
}


/* The number of argument digits needed for a command */
int num_arg_digits(int cmd) {
	int lim = argcmds[cmd].lim;
	if (lim < 10)
		return 1;
	if (lim < 100)
		return 2;
#ifdef COMPILE_CATALOGUES
	if (argcmds[cmd].reg || argcmds[cmd].flag || argcmds[cmd].label)
#else
	if (State2.ind || argcmds[cmd].reg || argcmds[cmd].flag || argcmds[cmd].label)
#endif
		return 2;
	return 3;
}

#ifdef COMPILE_CATALOGUES
#define SPACE_AFTER_CMD " "
#else
#define SPACE_AFTER_CMD "\006\006"
#endif
/* Commands that take an argument */
static const char *prt_rargs(const opcode op, char *instr) {
	unsigned int arg = op & RARG_MASK;
	int ind = op & RARG_IND;
	const unsigned int cmd = RARG_CMD(op);
	char *p;
	int n = 2;

	if (! argcmds[cmd].indirectokay) {
		if (ind) arg += RARG_IND;
		ind = 0;
	}

	if (cmd == RARG_ALPHA) {
		*scopy(instr, "\240" SPACE_AFTER_CMD) = arg;
	} 
	else if (cmd >= NUM_RARG)
		return "???";

	else if (!ind) {
		if (arg > argcmds[cmd].lim)
			return "???";

		p = instr;
		switch(cmd) {

		case RARG_CONST_CMPLX:
			*p++ = COMPLEX_PREFIX;
		case RARG_CONST:
			sncopy(scopy(p, "#" SPACE_AFTER_CMD), cnsts[arg].cname, CONST_NAMELEN);
			return instr;

		case RARG_CONV:
			return prt_conv(arg, instr);

		case RARG_SHUFFLE:
			p = scopy(instr, "\027" SPACE_AFTER_CMD);
			for (n = 0; n < 4; n++) {
				*p++ = REGNAMES[arg & 3];
				arg >>= 2;
			}
			return instr;

		default:
#ifdef COMPILE_CATALOGUES
			p = sncopy_spc(instr, argcmds[cmd].cmd, NAME_LEN);
#else
			p = sncopy_char(instr, argcmds[cmd].cmd, NAME_LEN, '\006');
			*p++ = '\006';
#endif
			if (argcmds[cmd].label && arg >= 100) {
				*p = arg - 100 + 'A';
			}
			else {
				n = num_arg_digits(cmd);
				goto print_reg;
			}
		}
	} 
	else {
		if (!argcmds[cmd].indirectokay)
			return "???";

		p = sncopy_char(instr, argcmds[cmd].cmd, NAME_LEN, '\015');	// ->

	print_reg:
		if (arg >= regX_idx && arg <= regK_idx && (ind || argcmds[cmd].stckreg))
			*p = REGNAMES[arg - regX_idx];
		else {
			if (arg >= LOCAL_REG_BASE && (ind || argcmds[cmd].local)) {
				arg -= LOCAL_REG_BASE;
				*p++ = '.';
			}
			num_arg_0(p, arg, n );
		}
	}
	return instr;
}

static const char *prt_multi(const opcode op, char *instr) {
	char *p, c;
	int cmd = opDBL(op);

	if (cmd >= NUM_MULTI)
		return "???";
	p = sncopy_char(instr, multicmds[cmd].cmd, NAME_LEN, '\'');
	*p++ = op & 0xff;
	c = (op >> 16) & 0xff;
	if (c != '\0') {
		*p++ = c;
		c = op>>24;
		if (c != '\0')
			*p++ = c;
	}
	*p = '\'';
	return instr;
}

const char *prt(opcode op, char *instr) {
	unsigned int arg;
	xset(instr, '\0', 16);
	if (isDBL(op))
		return prt_multi(op, instr);
	if (isRARG(op))
		return prt_rargs(op, instr);
	arg = argKIND(op);
	switch (opKIND(op)) {
	case KIND_SPEC:	return prt_specials(arg, instr);
	case KIND_NIL:	return prt_niladic(arg, instr);
	case KIND_MON:	return prt_monadic(arg, instr);
	case KIND_DYA:	return prt_dyadic(arg, instr);
	case KIND_TRI:	return prt_triadic(arg, instr);
	case KIND_CMON:	return prt_monadic_cmplx(arg, instr);
	case KIND_CDYA:	return prt_dyadic_cmplx(arg, instr);
	}
	return "???";
}

const char *catcmd(opcode op, char instr[16]) {
	unsigned int f;
	const char *name;

	xset(instr, '\0', 16);
	if (isDBL(op)) {
		return prt_multi(op, instr);
	} else if (isRARG(op)) {
		f = RARG_CMD(op);
		if (f < NUM_RARG) {
			if (f == RARG_CONV) {
				return prt_conv(op & RARG_MASK, instr);
			}
#if defined(INCLUDE_USER_CATALOGUE) && !defined(COMPILE_CATALOGUES)
			else if (f == RARG_CONST || f == RARG_CONST_CMPLX || f == RARG_ALPHA) {
				return prt(op, instr);
			}
#else
			else if (f == RARG_CONST || f == RARG_CONST_CMPLX) {
				const unsigned int arg = op & RARG_MASK;
				if (arg < NUM_CONSTS)
					return sncopy(instr, cnsts[arg].cname, CONST_NAMELEN);
			}
			else if (f == RARG_ALPHA) {
				*instr = op & 0xff;
				return instr;
			}
#endif
			else {
				return sncopy(instr, argcmds[f].cmd, NAME_LEN);
			}
		}
	} else {
		f = argKIND(op);
		switch (opKIND(op)) {
		default:
			break;
		case KIND_SPEC:
			return prt_specials(f, instr);

		case KIND_NIL:
			if (f >= NUM_NILADIC) break;
			name = niladics[f].nname;
			goto copy;

		case KIND_MON:
		case KIND_CMON:
			if (f >= NUM_MONADIC) break;
			name = monfuncs[f].fname;
			goto copy;

		case KIND_DYA:
		case KIND_CDYA:
			if (f >= NUM_DYADIC) break;
			name = dyfuncs[f].fname;
			goto copy;

		case KIND_TRI:
			if (f >= NUM_TRIADIC) break;
			name = trifuncs[f].fname;
		copy:
			return sncopy(instr, name, NAME_LEN);
		}
	}
	return "???";
}

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
//#include <stdio.h>
//#include <stdarg.h>
//#include <string.h>
//#include <stdlib.h>
#include <os.h>

#include "pretty.h"
#include "xeq.h"		// This helps the syntax checker

#include "consts.h"
#include "display.h"

//#include "translate.c"		// unicode[]
extern const int unicode[256];
extern const unsigned char to_cp1252[256];
extern const unsigned char from_cp1252[256];
extern const char *charnames[256];
#include "font_alias.h"	// more character aliases

#ifdef _MSC_VER
#define strdup _strdup
#define strcasecmp _stricmp
#endif

const char *pretty(unsigned char z) {
	if (z < 32)
		return map32[z & 0x1f];
	if (z >= 127)
		return maptop[z - 127];
	return CNULL;
}


void prettify(const char *in, char *out, int no_brackets) {
	const char *p;
	char c;

	while (*in != '\0') {
		c = *in++;
		p = pretty(c);
		if (p == NULL) {
			*out++ = c;
		}
		else {
			if (! no_brackets) {
				*out++ = '[';
			}
			else if (no_brackets == 1) {
				if (strcmp(p, "approx") == 0) {
					p = "~";
				}
				else if (strcmp(p, "cmplx") == 0) {
					p = "c";
				}
			}
			while (*p != '\0') {
				*out++ = *p++;
			}
			if (! no_brackets ) {
				*out++ = ']';
			}
		}
	}
	*out = '\0';
}

enum eCmdType {
	E_CMD_MULTI,
	E_CMD_CMD,
	E_CMD_ARG,

	E_ALIAS
};

static const char *command_type(enum eCmdType t) {
	static const char * tbl[E_ALIAS + 1] = {
		"mult",
		"cmd",
		"arg",
		"???"
	};
	return tbl[t];
}

static const char *alias_type(enum eCmdType a, enum eCmdType ct) {
	static const char * tbl[E_ALIAS + 1] = {
		"alias-m",
		"alias-c",
		"alias-a",
		"alias-?"
	};
	if (a != E_ALIAS)
		return tbl[a];
	return tbl[ct];
}



enum {
	E_MASK_MAX	=	0x0000ffff,
	E_ATTR_XROM	=	0x00010000,
	E_ATTR_INDIRECT	=	0x00020000,
	E_ATTR_STOSTACK	=	0x00040000,
	E_ATTR_STACK	=	0x00080000,
	E_ATTR_LOCAL	=	0x00100000,
	E_ATTR_COMPLEX	=	0x00200000,
	E_ATTR_LIMIT	=	0x00400000,
	E_ATTR_NO_CMD	=	0x80000000	/* Don't output the base command -- duplicate alias */
} eAttributes;


static void dump_attributes(FILE *f, unsigned int attributes) {
	int first = 1;

	if ((attributes & E_ATTR_LIMIT) != 0) {
		fprintf(f, "%c%s=%u", first?'\t':',', "max", attributes & E_MASK_MAX);
		first = 0;
	}
	if ((attributes & E_ATTR_INDIRECT) != 0) {
		fprintf(f, "%c%s", first?'\t':',', "indirect");
		first = 0;
	}
	if ((attributes & E_ATTR_STOSTACK) != 0) {
		fprintf(f, "%c%s", first?'\t':',', "stostack");
		first = 0;
	}
	if ((attributes & E_ATTR_STACK) != 0) {
		fprintf(f, "%c%s", first?'\t':',', "stack");
		first = 0;
	}
	if ((attributes & E_ATTR_LOCAL) != 0) {
		fprintf(f, "%c%s", first?'\t':',', "local");
		first = 0;
	}
	if ((attributes & E_ATTR_COMPLEX) != 0) {
		fprintf(f, "%c%s", first?'\t':',', "complex");
		first = 0;
	}
	if ((attributes & E_ATTR_XROM) != 0) {
		fprintf(f, "%c%s", first?'\t':',', "xrom");
		first = 0;
	}
}

struct xref_s {
	char *name;
	char *pretty;
	char *alias;
};
static struct xref_s alias_table[10000];
static int alias_tbl_n = 0;

struct alpha_s {
	char *pretty;
	int n;
	unsigned short codes[20];
};

static struct alpha_s alpha_table[256];

static int xref_pretty_compare(const void *v1, const void *v2);
static int xref_alias_compare(const void *v1, const void *v2);

static int xref_cmd_compare(const void *v1, const void *v2) {
	const struct xref_s *s1 = (const struct xref_s *)v1;
	const struct xref_s *s2 = (const struct xref_s *)v2;
	const unsigned char *p1 = (const unsigned char *)s1->name;
	const unsigned char *p2 = (const unsigned char *)s2->name;

	if (*(s1->name) == '\024')
		++p1;
	if (*(s2->name) == '\024')
		++p2;

	while (*p1 != '\0' && *p2 != '\0') {
		const int c1 = remap_chars(*p1++);
		const int c2 = remap_chars(*p2++);
		if (c1 < c2)
			return -1;
		if (c1 > c2)
			return 1;
	}
	if (*p1 == '\0' && *p2 == '\0')
		return xref_pretty_compare(v1, v2);
	if (*p1 == '\0') return -1;
	return 1;
}

static int xref_alias_compare(const void *v1, const void *v2) {
	const struct xref_s *s1 = (const struct xref_s *)v1;
	const struct xref_s *s2 = (const struct xref_s *)v2;
	const char *p1 = (const char *)s1->alias;
	const char *p2 = (const char *)s2->alias;
	int result;

	if (p1 == CNULL)
		p1 = "";
	if (p2 == CNULL)
		p2 = "";

	if (*(s1->name) == '\024')
		++p1;
	if (*(s2->name) == '\024')
		++p2;

	result = strcasecmp(p1, p2);
	if (0 == result) {
		if (*(s1->name) == '\024' && *(s2->name) != '\024')
			return 1;
		if (*(s1->name) != '\024' && *(s2->name) == '\024')
			return -1;
	}
	return result;
}

static int xref_pretty_compare(const void *v1, const void *v2) {
	const struct xref_s *s1 = (const struct xref_s *)v1;
	const struct xref_s *s2 = (const struct xref_s *)v2;
	const char *p1 = (const char *)s1->pretty;
	const char *p2 = (const char *)s2->pretty;
	int result;

	if (*(s1->name) == '\024')
		p1 += 7;	// "[cmplx]"
	if (*(s2->name) == '\024')
		p2 += 7;

	result = strcasecmp(p1, p2);
	if (0 == result)
		return xref_alias_compare(v1, v2);
	return result;
}

static void uputc(int c, FILE *f)
{
	if (c <= 0x7f) {
		fputc(c, f);
	}
	else if (c <= 0x7ff) {
		fputc(0xc0 | (c >> 6), f);
		fputc(0x80 | (c & 0x3f), f);
	}
	else {
		fputc(0xe0 | (c >> 12), f);
		fputc(0x80 | ((c >> 6) & 0x3f), f);
		fputc(0x80 | (c & 0x3f), f);
	}
}

static void uprintf(FILE *f, int mode, const char *fmt, ...)
{
	char line[500];
	unsigned char *p;
	int c;

	va_list ap;

	va_start(ap, fmt);
	vsprintf(line, fmt, ap);
	va_end(ap);

	for (p = (unsigned char *) line; *p != 0; ++p) {
		c = mode ? unicode[*p] : *p;
		uputc(c, f);
	}
}

static void output_xref_table(FILE *f) {
	int i, j;
	const char *p;
	
	fprintf(f, "By Command\n");
	qsort(alias_table, alias_tbl_n, sizeof(struct xref_s), &xref_cmd_compare);
	for (i = 0; i < alias_tbl_n; ++i) {
		p = alias_table[i].alias;
		uprintf(f, 1, "%s", alias_table[i].name);
		uprintf(f, 0, "\t%s\t%s\n", alias_table[i].pretty, p == CNULL ? "" : p);
	}
	fprintf(f, "\n\fBy Alias\n");
	qsort(alias_table, alias_tbl_n, sizeof(struct xref_s), &xref_alias_compare);
	for (i = 0; i < alias_tbl_n; ++i) {
		p = alias_table[i].alias;
		if (p) {
			uprintf(f, 0, "%s\t",   p);
			uprintf(f, 1, "%s",     alias_table[i].name);
			uprintf(f, 0, "\t%s\n", alias_table[i].pretty);
		}
	}
	fprintf(f, "\n\fBy Pretty Command\n");
	qsort(alias_table, alias_tbl_n, sizeof(struct xref_s), &xref_pretty_compare);
	for (i = 0; i < alias_tbl_n; ++i) {
		p = alias_table[i].alias;
		uprintf(f, 0, "%s\t",   alias_table[i].pretty);
		uprintf(f, 1, "%s",     alias_table[i].name);
		uprintf(f, 0, "\t%s\n", p == CNULL ? "" : p);
	}
	fprintf(f, "\n\fAlpha Characters\n");
	for (i = 1; i < 256; ++i) {
		uputc(unicode[i], f);
		uprintf(f, 0, "\t%s\t", alpha_table[i].pretty);
		for (j = 0; j < alpha_table[i].n; ++j) {
			if ( j ) fputc(' ', f);
			uputc(alpha_table[i].codes[j], f);
		}
		fputc('\n', f);
	}
}

static void dump_one_opcode(FILE *f, int c, const char *cmdname, enum eCmdType type, const char *cmdpretty,
				enum eCmdType atype, const char *cmdalias, unsigned int attributes, int xrefonly) {
	/* Output the opcode */
	if (xrefonly == 0) {
		if ((attributes & E_ATTR_NO_CMD) == 0) {
			fprintf(f, "0x%04x\t%s\t%s", c, command_type(type), cmdpretty);
			dump_attributes(f, attributes);
			fprintf(f, "\n");
		}

		if (cmdalias != CNULL) {
			fprintf(f, "0x%04x\t%s\t%s", c, alias_type(atype, type), cmdalias);
			dump_attributes(f, attributes);
			fprintf(f, "\n");
		}
	}
	else {
		// command xref
		if ((cmdalias != CNULL || strcmp(cmdname,cmdpretty) != 0)
		    && (! isDBL(c) || c == (OP_DBL | DBL_ALPHA))
		    && ! (attributes & E_ATTR_XROM)
		   ) 
		{
			alias_table[alias_tbl_n].name = strdup(cmdname);
			alias_table[alias_tbl_n].pretty = strdup(cmdpretty);
			alias_table[alias_tbl_n].alias = cmdalias == CNULL ? CNULL : strdup(cmdalias);
			alias_tbl_n++;
		}
	}
}

void dump_opcodes(FILE *f, int xref) {
	int c, d;
	char cmdname[16];
	char cmdpretty[500];
	char cmdalias[500];
	char buf[50], temp[500], temp2[50];
	const char *p, *cn;
	char *q;

	for (c=0; c<65536; c++) {
		if (isDBL(c)) {
			const unsigned int cmd = opDBL(c);
			if ((c & 0xff) != 0)
				continue;
			if (cmd >= NUM_MULTI)
				continue;
#ifdef INCLUDE_MULTI_DELETE
			if (cmd == DBL_DELPROG)
				continue;
#endif
			xset(cmdname, '\0', 16);
			xcopy(cmdname, multicmds[cmd].cmd, NAME_LEN);
			prettify(cmdname, cmdpretty, 0);
#ifdef XROM_LONG_BRANCH
			if (cmd == DBL_XBR)
				dump_one_opcode(f, c, cmdname, E_CMD_MULTI, cmdpretty, E_ALIAS, multicmds[cmd].alias, E_ATTR_XROM, xref);
			else
#endif
				dump_one_opcode(f, c, cmdname, E_CMD_MULTI, cmdpretty, E_ALIAS, multicmds[cmd].alias, 0, xref);

		} 
		else if (isRARG(c)) {
			const unsigned int cmd = RARG_CMD(c);
			unsigned int limit;

			if (cmd >= NUM_RARG)
				continue;
#ifdef INCLUDE_MULTI_DELETE
			if (cmd == RARG_DELPROG)
				continue;
#endif
			limit = argcmds[cmd].lim + 1;
			if (cmd != RARG_ALPHA && cmd != RARG_SHUFFLE && (c & RARG_IND) != 0)
				continue;
			p = cn = catcmd(c, cmdname);
			if (strcmp(p, "???") == 0)
				continue;
			prettify(p, cmdpretty, 0);
			prettify(p, cmdalias, 2);
			if (cmd == RARG_ALPHA) {
				const int n = c & 0xff;
				const struct _altuni *alt;

				if (n == 0)
					continue;
#if defined(INCLUDE_USER_CATALOGUE) && !defined(COMPILE_CATALOGUES)
				cn = p += 3;		// Skip the alpha prefix
				prettify(p, cmdalias, 2);
#endif
				if (n == ' ') {
					strcpy(cmdpretty, "[space]");
				}
				if (xref) {
					// alpha xref
					alpha_table[n].pretty = strdup(cmdpretty);
					alpha_table[n].codes[0] = unicode[n];
					alpha_table[n].n = 1;
					for (alt = AltUni; alt->code != 0; ++alt) {
						if (alt->code == n) {
							alpha_table[n].codes[alpha_table[n].n++] = alt->alternate;
						}
					}
					continue;
				}
				sprintf(buf, "'%s'", strlen(cmdpretty) == 3 ? cmdpretty : cmdalias);
				sprintf(temp2, "\240 %s", cn);
				sprintf(temp, "[alpha] %s", cmdpretty);
				dump_one_opcode(f, c, temp2, E_CMD_CMD, temp, E_ALIAS, buf, 0, 0);
				if (to_cp1252[n] >= 0x80) {
					sprintf(buf, "'%c'", to_cp1252[n]);
					dump_one_opcode(f, c, temp2, E_CMD_CMD, temp, E_ALIAS, buf, E_ATTR_NO_CMD, 0);
					sprintf(buf, "[alpha] %c", to_cp1252[n]);
					dump_one_opcode(f, c, temp2, E_CMD_CMD, temp, E_ALIAS, buf, E_ATTR_NO_CMD, 0);
				}
				for (alt = AltUni; alt->code != 0; ++alt) {
					if (alt->code == n && alt->alternate <= 0xff && alt->alternate != to_cp1252[n]) {
						sprintf(buf, "'%c'", alt->alternate);
						dump_one_opcode(f, c, temp2, E_CMD_CMD, temp, E_ALIAS, buf, E_ATTR_NO_CMD, 0);
						sprintf(buf, "[alpha] %c", alt->alternate);
						dump_one_opcode(f, c, temp2, E_CMD_CMD, temp, E_ALIAS, buf, E_ATTR_NO_CMD, 0);
					}
				}
				continue;
			} 
			else if (cmd == RARG_CONST || cmd == RARG_CONST_CMPLX) {
				const int n = c & 0xff;
				const char *pre = n == OP_PI ? "" : "# ";
				const char *alias = cnsts[n].alias;

				if (xref && cmd == RARG_CONST_CMPLX)
					continue;
				if (n == OP_ZERO || n == OP_ONE)
					continue;
				if (strchr(cmdpretty, '[') == NULL)
					alias = NULL;
				if (cmd == RARG_CONST_CMPLX) {
					sprintf(temp2, "\024# %s", cn);
					sprintf(temp, "[cmplx]# %s", cmdpretty);
					sprintf(buf, "c%s%s", pre, alias ? alias : cmdpretty);
					dump_one_opcode(f, c, temp2, E_CMD_CMD, temp, E_ALIAS, buf, 0, xref);
				}
				else {
					sprintf(temp2, "# %s", cn);
					sprintf(temp, "# %s", cmdpretty);
					if (alias)
						sprintf(buf, "%s%s", pre, alias);
					dump_one_opcode(f, c, temp2, E_CMD_CMD, temp, E_ALIAS, alias ? buf : CNULL, 0, xref);
				}
				continue;
			} 
			else if (cmd == RARG_CONV) {
				strcpy(temp, cmdpretty);
				q = strstr(temp, "[->]");
				if (p) {
					*q++ = '>';
					while (q[3] != '\0') {
						*q = q[3];
						q++;
					}
					*q = '\0';
					p = temp;
				} else p = CNULL;
				dump_one_opcode(f, c, cn, E_CMD_CMD, cmdpretty, E_ALIAS, p, 0, xref);
				continue;
			}
			else if (cmd == RARG_SHUFFLE) {
				if (xref) {
					if ((c & 0xff) == 0)
						dump_one_opcode(f, c, cn, E_CMD_CMD, cmdpretty, E_ALIAS, "<>", 0, 1);
					continue;
				}

				sprintf(temp, "%s %c%c%c%c", cmdpretty,
						REGNAMES[c & 3], REGNAMES[(c >> 2) & 3],
						REGNAMES[(c >> 4) & 3], REGNAMES[(c >> 6) & 3]);
				sprintf(buf, "%s %c%c%c%c", "<>",
						REGNAMES[c & 3], REGNAMES[(c >> 2) & 3],
						REGNAMES[(c >> 4) & 3], REGNAMES[(c >> 6) & 3]);
				dump_one_opcode(f, c, cn, E_CMD_CMD, temp, E_ALIAS, buf, 0, xref);
				continue;
			}
			else if (c == RARG(RARG_SWAPX, regY_idx)) {
				sprintf(temp, "%s Y", cn);
				sprintf(buf, "%s Y", cmdpretty); 
				dump_one_opcode(f, c, temp, E_CMD_CMD, buf, E_ALIAS, "x<>y", E_ATTR_NO_CMD, xref);
				dump_one_opcode(f, c, temp, E_CMD_CMD, buf, E_ALIAS, "SWAP", E_ATTR_NO_CMD, xref);
			} 
			else if (c == RARG(RARG_CSWAPX, regZ_idx)) {
				sprintf(temp, "%s Z", cn);
				sprintf(buf, "%s Z", cmdpretty); 
				dump_one_opcode(f, c, temp, E_CMD_CMD, buf, E_ALIAS, "cSWAP", E_ATTR_NO_CMD, xref);
			}
			if ((c & 0xff) != 0)
				continue;
			if (argcmds[cmd].indirectokay && limit > RARG_IND)
				limit = RARG_IND;

			limit |= E_ATTR_LIMIT;
			if (argcmds[cmd].indirectokay)
				limit |= E_ATTR_INDIRECT;
			if (cmd == RARG_STOSTK || cmd == RARG_RCLSTK)
				limit |= E_ATTR_STOSTACK;
			else if (argcmds[cmd].stckreg)
				limit |= E_ATTR_STACK;
			else if (argcmds[cmd].local)
				limit |= E_ATTR_LOCAL;
			if (argcmds[cmd].cmplx)
				limit |= E_ATTR_COMPLEX;
			if (cmd == RARG_MODE_SET 
			 || cmd == RARG_MODE_CLEAR 
			 || cmd == RARG_XROM_IN 
			 || cmd == RARG_XROM_OUT
#ifndef INCLUDE_INDIRECT_BRANCHES
		       //|| cmd == RARG_iSKIP
#endif
		       //|| cmd == RARG_BSF
		       //|| cmd == RARG_BSB
		       //|| cmd == RARG_INDEX
		       //|| cmd == RARG_CONST_INDIRECT
#ifdef XROM_RARG_COMMANDS
			 || cmd == RARG_XROM_ARG
#endif
			   )
				limit |= E_ATTR_XROM;

			dump_one_opcode(f, c, cn, E_CMD_ARG, cmdpretty, E_ALIAS, argcmds[cmd].alias, limit, xref);
		}
		else {
			p = cn = catcmd(c, cmdname);
			if (strcmp(p, "???") == 0)
				continue;
			prettify(p, cmdpretty, 0);
			if (CNULL != strchr(cmdpretty, '[')) {
				prettify(p, cmdalias, 1);
				p = cmdalias;
			}
			else
				p = CNULL;

			d = argKIND(c);
			switch (opKIND(c)) {
			default:
				break;

			case KIND_SPEC:
				if (d == OP_ENTER)
					p = "ENTER";
				else if (d == OP_CHS)
					p = "CHS";
				break;

			case KIND_MON:
				if (d < NUM_MONADIC && (! isNULL(monfuncs[d].mondreal) || ! isNULL(monfuncs[d].monint))) {
					p = monfuncs[d].alias;
					break;
				}
				continue;

			case KIND_DYA:
				if (d < NUM_DYADIC && (! isNULL(dyfuncs[d].dydreal) || ! isNULL(dyfuncs[d].dydint))) {
					p = dyfuncs[d].alias;
					break;
				}
				continue;

			case KIND_TRI:
				if (d < NUM_TRIADIC) {
					p = trifuncs[d].alias;
					break;
				}
				continue;

			case KIND_CMON:
				if (d < NUM_MONADIC && ! isNULL(monfuncs[d].mondcmplx)) {
					if (cmdname[0] == COMPLEX_PREFIX)
						break;
					p = monfuncs[d].alias;
					sprintf(temp2, "\024%s", cn);
					sprintf(temp, "[cmplx]%s", cmdpretty);
					sprintf(buf, "c%s", p ? p : cmdpretty);
					dump_one_opcode(f, c, temp2, E_CMD_CMD, temp, E_ALIAS, buf, 0, xref);
				}
				continue;

			case KIND_CDYA:
				if (d < NUM_DYADIC && ! isNULL(dyfuncs[d].dydcmplx)) {
					if (cmdname[0] == COMPLEX_PREFIX)
						break;
					p = dyfuncs[d].alias;
					sprintf(temp2, "\024%s", cn);
					sprintf(temp, "[cmplx]%s", cmdpretty);
					sprintf(buf, "c%s", p ? p : cmdpretty);
					dump_one_opcode(f, c, temp2, E_CMD_CMD, temp, E_ALIAS, buf, 0, xref);
				}
				continue;

			case KIND_NIL:
#ifdef INCLUDE_STOPWATCH
				if (d == OP_STOPWATCH)
					continue;
#endif
				if (d >= OP_CLALL && d <= OP_CLPALL) {
					continue;
				}
				if (d == OP_LOADA2D || d == OP_SAVEA2D ||
						d == OP_GSBuser || d == OP_POPUSR) {
					dump_one_opcode(f, c, cn, E_CMD_CMD, cmdpretty, E_ALIAS, CNULL, E_ATTR_XROM, xref);
					continue;
				}
				if (d < NUM_NILADIC) {
					p = niladics[d].alias;
					break;
				}
				continue;
			}
			dump_one_opcode(f, c, cn, E_CMD_CMD, cmdpretty, E_ALIAS, p, 0, xref);
			if (c == (OP_CMON | OP_CCHS))
				dump_one_opcode(f, c, cn, E_CMD_CMD, cmdpretty, E_ALIAS, "cCHS", E_ATTR_NO_CMD, xref);
		}
	}
	if (xref)
		output_xref_table(f);
}


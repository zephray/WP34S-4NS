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

/*
 *  This is the console emulator part
 */
#include <os.h>

#include "xeq.h" 
#include "keys.h"
#include "display.h"
#include "lcd.h"
#include "int.h"
#include "consts.h"
#include "storage.h"

#include "image.h"

#include "catalogues.h"

#define CH_QUIT		'Q'
#define CH_TRACE	'T'
#define CH_FLAGS	'F'
#define CH_ICOUNT	'C'
#define CH_REFRESH	12	/* ^L */

char *VRAM_A;

unsigned long long int instruction_count = 0;
int view_instruction_counter = 0;

/*
 *  PC keys to calculator keys
 */
static int remap(const int c) {
	switch (c) {

	case 50:	return K00;//A
	case 49:	return K01;//B
	case 48:	return K02;//C
	case 46:	return K03;//D
	case 45:	return K04;//E
	case 91:	return K04;
	case 44:	return K05;//F
	case 71:	return K05;

	case 39:	return K10;//G
	case 60:	return K10;
	case 38:	return K11;//H
	case 58:	return K11;
	case 37:	return K12;//I
	case 51:	return K12;
	case 75:	return K_F;
	case 86:	return K_G;
	case 85:	return K_H;

	case  1:	return K20;
	case 35:	return K21;//J
	case 90:	return K21;
	case 34:	return K22;//K
	case  3:	return K22;
	case 30:	return K23;//EE
	case 33:	return K23;//L
	case 21:	return K23;//x10^
	case 64:	return K24;	// Backspace

	case 20:	return K30;
	case 28:	return K31;//M
	case 40:	return K31;//7
	case 27:	return K32;//N
	case 72:	return K32;//8
	case 26:	return K33;//O
	case 36:	return K33;//9
	case 24:	return K34;//P
	case 41:	return K34;

	case 88:	return K40;
	case 23:	return K41;//Q
	case 29:	return K41;//4
	case 22:	return K42;//R
	case 61:	return K42;//5
	case 17:	return K43;//S
	case 25:	return K43;//6
	case 16:	return K44;//T
	case 52:	return K44;

	case 89:	return K50;
	case 18:	return K51;//1
	case 15:	return K52;//U
	case 70:	return K52;//2
	case 13:	return K53;//V
	case 14:	return K53;//3
	case 12:	return K54;//W
	case 57:	return K54;

	case 73:	return K60;	// ON
	case  7:	return K61;
	case 11:	return K62;//X
	case 59:	return K62;
	case  6:	return K63;//Y
	case 69:	return K63;
	case  5:	return K64;//Z
	case 68:	return K64;
	}
	return K_UNKNOWN;
}


//#include "pretty.c"

static void rarg_values(const opcode c, int l) {
	char bf[100];

	if (isRARG(c)) {
		const unsigned int cmd = RARG_CMD(c);
		const unsigned int idx = c & 0x7f;

		if (cmd == RARG_CONV) {
			while (l-- > 0)
				putchar(' ');
			printf("%c %s", (idx & 1)?'*':'/', decimal64ToString(&CONSTANT_CONV(idx/2), bf));
		} else if (cmd == RARG_CONST || cmd == RARG_CONST_CMPLX) {
			while (l-- > 0)
				putchar(' ');
			printf("%s", decimal64ToString((decimal64 *) get_const(c & 0x7f, 0), bf));
		}
	}
}
	
static int dumpop(const opcode c, int pt) {
	char tracebuf[25];
	const char *s, *m;

	if (c == RARG(RARG_ALPHA, 0))
		return 0;
	xset(tracebuf, '\0', sizeof(tracebuf));
	s = prt(c, tracebuf);
	if (strcmp(s, "???") != 0) {
		char t[100], *q = t;
		int l = 35;

		if (c == RARG(RARG_ALPHA, ' '))
			strcpy(tracebuf+2, "[space]");

		q += sprintf(t, "%04x  ", (unsigned int)c);
		while (*s != '\0') {
			const unsigned char z = 0xff & *s++;

			//m = pretty(z);
			if (m == NULL) {
				*q++ = z;
				l--;
			} else {
				l -= slen(m) + 2;
				q += sprintf(q, "[%s]", m);
			}
		}
		*q++ = '\0';
		if (pt) {
			printf("%s", t);
			rarg_values(c, l);
			putchar('\n');
		}
		return 1;
	}
	return 0;
}


static void dump_menu(const char *name, const char *prefix, const enum catalogues cata) {
	int i;
	char cmd[16];
	const char *p;
	const char *buf;
	const char *m;
	const int oldcata = State2.catalogue;
	int n;

	State2.catalogue = cata;
	n = current_catalogue_max();
	printf("%s catalogue:\n", name);
	for (i=0; i<n; i++) {
		int l = 35 - slen(prefix);
		const opcode cati = current_catalogue(i);
		buf = catcmd(cati, cmd);

		if (cati == RARG(RARG_ALPHA, ' '))
			strcpy(cmd+2, "[space]");

		printf("\t%d\t%s", i+1, prefix);
		for (p=buf; *p != '\0'; p++) {
			const unsigned char c = 0xff & *p;
			//m = pretty(c);
			if (m == NULL) {
				printf("%c", c);
				l--;
			} else {
				printf("[%s]", m);
				l -= slen(m) + 2;
			}
		}
		rarg_values(cati, l);
		printf("\n");
	}
	printf("\n");
	State2.catalogue = oldcata;
}

#include "xrom.h"
#include "xrom_labels.h"
static const struct {
	unsigned int address;
	const char *const name;
} xrom_entry_points[] = {
#define XE(l)		{ XROM_ ## l, # l }
	XE(2DERIV),			XE(F_DENANY),			XE(QF_WEIB),
	XE(AGM),			XE(F_DENFAC),			XE(QUAD),
	XE(Bn),				XE(F_DENFIX),			XE(RADIANS),
	XE(Bn_star),			XE(GRADIANS),			XE(RADIX_COM),
	XE(CDFU_BINOMIAL),		XE(HR12),			XE(RADIX_DOT),
	XE(CDFU_CAUCHY),		XE(HR24),			XE(SEPOFF),
	XE(CDFU_CHI2),			XE(HermiteH),			XE(SEPON),
	XE(CDFU_EXPON),			XE(HermiteHe),			XE(SETCHN),
	XE(CDFU_F),			XE(IDIV),			XE(SETEUR),
	XE(CDFU_GEOM),			XE(IM_LZOFF),			XE(SETIND),
	XE(CDFU_LOGIT),			XE(IM_LZON),			XE(SETJAP),
	XE(CDFU_LOGNORMAL),		XE(INTEGRATE),			XE(SETUK),
	XE(CDFU_NORMAL),		XE(ISGN_1C),			XE(SETUSA),
	XE(CDFU_POIS2),			XE(ISGN_2C),			XE(SIGMA),
	XE(CDFU_POISSON),		XE(ISGN_SM),			XE(SIGN),
	XE(CDFU_Q),			XE(ISGN_UN),			XE(SOLVE),
	XE(CDFU_T),			XE(JG1582),			XE(STACK_4_LEVEL),
	XE(CDFU_WEIB),			XE(JG1752),			XE(STACK_8_LEVEL),
	XE(CDF_BINOMIAL),		XE(LaguerreLn),			XE(START),
	XE(CDF_CAUCHY),			XE(LaguerreLnA),		XE(W0),
	XE(CDF_CHI2),			XE(LegendrePn),			XE(W1),
	XE(CDF_EXPON),			XE(MARGIN),			XE(WHO),
	XE(CDF_F),			XE(NEXTPRIME),			XE(W_INVERSE),
	XE(CDF_GEOM),			XE(PARL),			XE(ZETA),
	XE(CDF_LOGIT),			XE(PDF_BINOMIAL),		XE(beta),
	XE(CDF_LOGNORMAL),		XE(PDF_CAUCHY),			XE(cpx_ACOS),
	XE(CDF_NORMAL),			XE(PDF_CHI2),			XE(cpx_ACOSH),
	XE(CDF_POIS2),			XE(PDF_EXPON),			XE(cpx_ASIN),
	XE(CDF_POISSON),		XE(PDF_F),			XE(cpx_ASINH),
	XE(CDF_Q),			XE(PDF_GEOM),			XE(cpx_ATAN),
	XE(CDF_T),			XE(PDF_LOGIT),			XE(cpx_ATANH),
	XE(CDF_WEIB),			XE(PDF_LOGNORMAL),		XE(cpx_CONJ),
	XE(CPX_AGM),			XE(PDF_NORMAL),			XE(cpx_CROSS),
	XE(CPX_COMB),			XE(PDF_POIS2),			XE(cpx_DOT),
	XE(CPX_FIB),			XE(PDF_POISSON),		XE(cpx_EXPM1),
	XE(CPX_I),			XE(PDF_Q),			XE(cpx_FACT),
	XE(CPX_PARL),			XE(PDF_T),			XE(cpx_FRAC),
	XE(CPX_PERM),			XE(PDF_WEIB),			XE(cpx_IDIV),
	XE(CPX_W0),			XE(PERCENT),			XE(cpx_LN1P),
	XE(CPX_W_INVERSE),		XE(PERCHG),			XE(cpx_LOG10),
	XE(ChebychevTn),		XE(PERMARGIN),			XE(cpx_LOG2),
	XE(ChebychevUn),		XE(PERMMR),			XE(cpx_LOGXY),
	XE(DATE_ADD),			XE(PERTOT),			XE(cpx_POW10),
	XE(DATE_DELTA),			XE(PRODUCT),			XE(cpx_POW2),
	XE(DATE_TO),			XE(QF_BINOMIAL),		XE(cpx_ROUND),
	XE(DEGREES),			XE(QF_CAUCHY),			XE(cpx_SIGN),
	XE(DERIV),			XE(QF_CHI2),			XE(cpx_TRUNC),
	XE(D_DMY),			XE(QF_EXPON),			XE(cpx_beta),
	XE(D_MDY),			XE(QF_F),			XE(cpx_gd),
	XE(D_YMD),			XE(QF_GEOM),			XE(cpx_inv_gd),
	XE(E3OFF),			XE(QF_LOGIT),			XE(cpx_lnbeta),
	XE(E3ON),			XE(QF_LOGNORMAL),		XE(cpx_x2),
	XE(ERF),			XE(QF_NORMAL),			XE(cpx_x3),
	XE(ERFC),			XE(QF_POIS2),			XE(gd),
	XE(FIB),			XE(QF_POISSON),			XE(int_ULP),
	XE(FIXENG),			XE(QF_Q),			XE(inv_gd),
	XE(FIXSCI),			XE(QF_T),
#undef XE
};
#define num_xrom_entry_points	(sizeof(xrom_entry_points) / sizeof(*xrom_entry_points))
	
static const struct {
	opcode op;
	const char *const name;
} xrom_labels[] = {
#define X(op, n, s)	{ RARG(RARG_ ## op, (n) & RARG_MASK), s},
#define XE(n, s)	X(ERROR, n, "Error: " # s)	X(MESSAGE, n, "Message: " # s)
	XE(ERR_DOMAIN, "Domain Error")
	XE(ERR_BAD_DATE, "Bad Date Error")
	XE(ERR_PROG_BAD, "Undefined Op-code")
	XE(ERR_INFINITY, "+infinity")
	XE(ERR_MINFINITY, "-infinity")
	XE(ERR_NO_LBL, "no such label")
	XE(ERR_ILLEGAL, "Illegal operation")
	XE(ERR_RANGE, "out of range error")
	XE(ERR_DIGIT, "bad digit error")
	XE(ERR_TOO_LONG, "too long error")
	XE(ERR_RAM_FULL, "RTN stack full")
	XE(ERR_STK_CLASH, "stack clash")
	XE(ERR_BAD_MODE, "bad mode error")
	XE(ERR_INT_SIZE, "word size too small")
	XE(ERR_MORE_POINTS, "more data points required")
	XE(ERR_BAD_PARAM, "invalid parameter")
	XE(ERR_IO, "input / output problem")
	XE(ERR_INVALID, "invalid data")
	XE(ERR_READ_ONLY, "write protected")
	XE(ERR_SOLVE, "solve failed")
	XE(ERR_MATRIX_DIM, "matrix dimension mismatch")
	XE(ERR_SINGULAR, "matrix singular")
	XE(ERR_FLASH_FULL, "flash is full")
	XE(MSG_INTEGRATE, "integration progress")
#undef XE
#undef X
};
#define num_xrom_labels		(sizeof(xrom_labels) / sizeof(*xrom_labels))

static void dump_code(unsigned int pc, unsigned int max, int annotate) {
/*	int dbl = 0, sngl = 0;

	printf("ADDR  OPCODE     MNEMONIC%s\n\n", annotate?"\t\tComment":"");
	do {
		char instr[16];
		const opcode op = getprog(pc);
		const char *p = prt(op, instr);
		int i;
		if (isDBL(op)) {
			dbl++;
			printf("%04x: %04x %04x  ", pc, op & 0xffff, (op >> 16)&0xffff);
		} else {
			sngl++;
			printf("%04x: %04x       ", pc, op);
		}
		//printf("%04x: %04x  ", pc, op);
		if (op == RARG(RARG_ALPHA, ' '))
			strcpy(instr+2, "[space]");

		while (*p != '\0') {
			char c = *p++;
			const char *q = pretty(c);
			if (q == NULL) putchar(c);
			else if (strcmp("narrow-space", q) == 0 && *p == c) {
				printf(" ");
				p++;
			} else printf("[%s]", q);
		}
		if (annotate) {
			extern const unsigned short int xrom_targets[];
			for (i=0; i<num_xrom_entry_points; i++)
				if (addrXROM(xrom_entry_points[i].address) == pc)
					printf("\t\t\tXLBL %s", xrom_entry_points[i].name);
			for (i=0; i<num_xrom_labels; i++)
				if (xrom_labels[i].op == op)
					printf("\t\t\t%s", xrom_labels[i].name);
			if (RARG_CMD(op) == RARG_SKIP || RARG_CMD(op) == RARG_BSF)
				printf("\t\t-> %04x", pc + (op & 0xff) + 1);
			else if (RARG_CMD(op) == RARG_BACK || RARG_CMD(op) == RARG_BSB)
				printf("\t\t-> %04x", pc - (op & 0xff));
			else if (RARG_CMD(op) == RARG_XEQ || RARG_CMD(op) == RARG_GTO)
				printf("\t\t\t-> %04x", addrXROM(0) + xrom_targets[op & RARG_MASK]);
		}
		putchar('\n');
		pc = do_inc(pc, 0);
	} while (! PcWrapped);
	if (annotate)
		printf("%u XROM words\n%d single word instructions\n%d double word instructions\n", max-pc, sngl, dbl);*/
}

static void dump_xrom(void) {
//	dump_code(addrXROM(0), addrXROM(xrom_size), 1);
}

static void dump_ram(void) {
/*	if (ProgSize > 0)
		dump_code(1, ProgSize + 1, 0);
	else
		printf("no RAM program\n");*/
}

static void dump_prog(unsigned int n) {
/*	unsigned int pc;
	if (n > REGION_LIBRARY - 1)
		printf("no such program region %u\n", n);
	else if (sizeLIB(n+1) == 0)
		printf("region %u empty\n", n);
	else {
		pc = addrLIB(0, n+1);
		dump_code(pc, pc + sizeLIB(n+1), 0);
	}*/
}

static void dump_registers(void) {
/*	char buf[100];
	int i;
	if (is_dblmode()) {
		for (i=0; i<100; i += 2) {
			//decimal128ToString(&(get_reg_n(i)->d), buf);
			printf("register %02d: %s\n", i, buf);
		}
		return;
	}
	for (i=0; i<100; i++) {
		//decimal64ToString(&(get_reg_n(i)->s), buf);
		printf("register %02d: %s\n", i, buf);
	}*/
}


static void dump_constants(void) {
/*	char buf[100];
	int i;

	for (i=0; i<NUM_CONSTS; i++) {
		//decimal128ToString(&(get_const(i, 1)->d), buf);
		printf("\t%02d\t%s\n", i, buf);
	}*/
}


static void dump_cmd_op(unsigned int op, unsigned int *n, int silent) {
/*	char buf[16];
	char out[1000];
	const char *pre;
	const char *p = catcmd(op, buf);
	const unsigned int opk = opKIND(op);
	const unsigned int opa = argKIND(op);

	if (strcmp(p, "???") == 0)
		return;
	if (! isRARG(op)) {
		if (opk == KIND_CMON && isNULL(monfuncs[opa].mondcmplx))
			return;
		if (opk == KIND_CDYA && isNULL(dyfuncs[opa].dydcmplx))
			return;
		if (opk == KIND_MON && isNULL(monfuncs[opa].mondreal) && isNULL(monfuncs[opa].monint))
			return ;
		if (opk == KIND_DYA && isNULL(dyfuncs[opa].dydreal) && isNULL(dyfuncs[opa].dydint))
			return ;
	}
	++*n;
	if (silent)
		return;
	pre = "";
	if (opk == KIND_CMON || opk == KIND_CDYA)
		pre = "[cmplx]";
	else if (isRARG(op) && RARG_CMD(op) == RARG_CONST_CMPLX)
		pre = "[cmplx]# ";
	else if (isRARG(op) && RARG_CMD(op) == RARG_CONST)
		pre = "# ";
	else if (isRARG(op) && RARG_CMD(op) == RARG_ALPHA)
		pre = "Alpha ";
	//prettify(p, out, 0);
	if (strncmp("[cmplx]", out, 7) == 0 && !(isRARG(op) && RARG_CMD(op) == RARG_ALPHA))
		pre = "";
	printf("%-5u  %04x   %s%s\n", *n, op, pre, out);*/
}

static unsigned int dump_commands(int silent) {
/*	unsigned int i, j, n=0;

	if (! silent)
		printf("Number Opcode Command\n");
	for (i=0; i<KIND_MAX; i++)
		for (j=0; j<opcode_breaks[i]; j++)
			dump_cmd_op((i<<KIND_SHIFT) + j, &n, silent);
	for (i=0; i<NUM_RARG; i++) {
		if (i == RARG_CONST || i == RARG_CONST_CMPLX) {
			for (j=0; j<NUM_CONSTS_CAT-1; j++)
				dump_cmd_op(RARG(i, j), &n, silent);
			dump_cmd_op(RARG(i, OP_PI), &n, silent);
		} else if (i == RARG_CONV) {
			for (j=0; j<NUM_CONSTS_CONV*2; j++)
				dump_cmd_op(RARG(i, j), &n, silent);
		} else if (i == RARG_ALPHA) {
			for (j=1; j<256; j++)
				dump_cmd_op(RARG(i, j), &n, silent);
		} else
			dump_cmd_op(RARG(i, 0), &n, silent);
	}
	return n;*/
}


void shutdown( void )
{
	checksum_all();
	setuptty( 1 );
	save_statefile();
	exit( 0 );
}


/*
 *  Dummies
 */
int is_key_pressed(void) 
{
	return 0;
}

int get_key(void)
{
	return 0;
}

int put_key( int k )
{
	return k;
}

enum shifts shift_down(void)
{
	return SHIFT_N;
}

#ifndef WIN32  // Windows uses winserial.c
/*
 *  Open a COM port for transmission
 */
int open_port( int baud, int bits, int parity, int stopbits )
{
	return 0;
}


/*
 *  Close the COM port after transmission is complete
 */
extern void close_port( void )
{
}


/*
 *  Output a single byte to the serial
 */
void put_byte( unsigned char byte )
{
	err(ERR_PROG_BAD);
}


/*
 *  Force buffer flush
 */
void flush_comm( void )
{
}

#endif

unsigned char GETCHAR()
{
	volatile uint16_t *mem;
	uint16_t keymap[8];
	uint16_t now;
	uint8_t i,j;
	uint8_t key;
	mem = 0x900E0010;
	wait_key_pressed();
	for (i=0;i<8;i++)
	{
		keymap[i]=*mem++;
	}
	for (i=0;i<8;i++)
	{
		now = keymap[i];
		if (!(is_cx)) now = ~now;
		if (now != 0)
		{
			for (j=0;j<11;j++)
			{
				if ((now&0x01)==0x01)
					key=i*11+j;
				now = now>>1;
			}
			sleep(100);
		}
	}
	if isKeyPressed(KEY_NSPIRE_UP) return 88;
	if isKeyPressed(KEY_NSPIRE_DOWN) return 89;
	if isKeyPressed(KEY_NSPIRE_LEFT) return 90;
	if isKeyPressed(KEY_NSPIRE_RIGHT) return 91;
	
	return key;
}

unsigned char WaitKeyReleased(unsigned char key)
{
	volatile uint16_t *mem;
	uint16_t now;
	uint8_t i,j;
	mem = 0x900E0010;
	if (key<88)
	{
		i=key/11;
		j=key%11;
		mem+=i;
		if (is_cx)
		{
			do{
				now = *mem;
				now = now >> j;
			}while((now&0x01)==0x01);
		}
	}else
	{
		do{
			readTP();
		}while(isTPPressed());
	}
	//printf("key released!\r\n");
}

void showAbout()
{
	char *VRAM_B;
	int conti;
	
	VRAM_B = init_VRAM();
	memcpy(VRAM_B,SCREEN_BASE_ADDRESS,SCREEN_BYTES_SIZE);
	if (is_cx)
		slide_down(VRAM_B,gImage_about,10);
	else
		slide_down(VRAM_B,gImage_about_bw,10);
	conti = 1;
	while (conti)
	{
		wait_key_pressed();
		if isKeyPressed(KEY_NSPIRE_DOWN)
		{
			conti=0;
			if (is_cx)
				slide_up(VRAM_B,gImage_about,10);
			else
				slide_up(VRAM_B,gImage_about_bw,10);
			WaitKeyReleased(89);
		}
	}
	close_VRAM(VRAM_B);
}

void showHelp1()
{
	char *VRAM_B;
	int conti;
	
	VRAM_B = init_VRAM();
	memcpy(VRAM_B,SCREEN_BASE_ADDRESS,SCREEN_BYTES_SIZE);
	if (is_cx)
		slide_down(VRAM_B,gImage_help1,10);
	else
		slide_down(VRAM_B,gImage_help1_bw,10);
	conti = 1;
	while (conti)
	{
		wait_key_pressed();
		if isKeyPressed(KEY_NSPIRE_DOWN)
		{
			conti=0;
			if (is_cx)
				slide_up(VRAM_B,gImage_help1,10);
			else
				slide_up(VRAM_B,gImage_help1_bw,10);
			WaitKeyReleased(56);
		}else
			if isKeyPressed(KEY_NSPIRE_UP)
			{
				showAbout();
			}
	}
	close_VRAM(VRAM_B);
}

void showHelp0()
{
	char *VRAM_B;
	int conti;
	
	VRAM_B = init_VRAM();
	memcpy(VRAM_B,SCREEN_BASE_ADDRESS,SCREEN_BYTES_SIZE);
	if (is_cx)
		slide_down(VRAM_B,gImage_help0,10);
	else
		slide_down(VRAM_B,gImage_help0_bw,10);
	conti = 1;
	while (conti)
	{
		wait_key_pressed();
		if (isKeyPressed(KEY_NSPIRE_VAR)|| isKeyPressed(KEY_NSPIRE_DOWN))
		{
			conti=0;
			if (is_cx)
				slide_up(VRAM_B,gImage_help0,10);
			else
				slide_up(VRAM_B,gImage_help0_bw,10);
			WaitKeyReleased(56);
		}else
			if isKeyPressed(KEY_NSPIRE_UP)
			{
				showHelp1();
			}
	}
	close_VRAM(VRAM_B);
}

/*
 *  Main loop
 */
int main(int argc, char *argv[]) {
	int c, n = 0;
	uint16_t key;
	int warm = 0;
	
	xeq_init_contexts();
	load_statefile();
	if (!warm)
		init_34s();
	State2.flags = 1;
	if (is_cx)
	{
		lcd_incolor();
	}else
	{
		lcd_ingray();
	}
	initTP();
	VRAM_A = init_VRAM();
	LCD_FillAll(VRAM_A,0x0000);
	LCD_DispBmp(VRAM_A,0,0,320,24,(uint16_t *)gImage_bar,0xF81F);
	if (setuptty(0) == 0) {
		display();
		JustDisplayed = 0;
		while ((key = GETCHAR()) != GETCHAR_ERR) {
#ifdef USECURSES
			if (c == CH_TRACE) {
				State2.trace = 1 - State2.trace;
				display();
			} else if (c == CH_FLAGS) {
				State2.flags = 1 - State2.flags;
				display();
			} else if (c == CH_REFRESH) {
				clear();
				display();
			} else if (c == CH_ICOUNT) {
				instruction_count = 0;
				view_instruction_counter = 1 - view_instruction_counter;
				display();
			} else
#endif
			c=remap(key);
			if(c != K_UNKNOWN) {
				process_keycode(c);
				WaitKeyReleased(key);
				process_keycode(K_RELEASE);
			}else
			if (key == 56)
			{
				WaitKeyReleased(key);
				showHelp0();
			}else
			if (key == 65)
			{
				showpage++;
				if (showpage>2)
					showpage=0;
				display();
				WaitKeyReleased(key);
			}
		}
		setuptty(1);
	}
	wait_key_pressed();
	shutdown();
	return 0;
}


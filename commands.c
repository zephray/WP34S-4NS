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
#if COMMANDS_PASS != 2
#include "xeq.h"
#include "xrom.h"
#include "decn.h"
#include "complex.h"
#include "stats.h"
#include "int.h"
#include "date.h"
#include "display.h"
#include "consts.h"
#include "alpha.h"
#include "lcd.h"
#include "storage.h"
#include "serial.h"
#ifdef INFRARED
#include "printer.h"
#endif
#include "matrix.h"
#ifdef INCLUDE_STOPWATCH
#include "stopwatch.h"
#endif
#endif

/*
 *  Macro to define pointers to XROM routines
 *  Usage: XPTR(WHO) instead of a function pointer
 */
#define XLBL(name) XROM_ ## name
#define XPTR(name) (xrom+XLBL(name)-XROM_START)

/* Utility macros to reduce the horizontal space of using XPTR directly
 * Usage is the same.
 */
#define XMR(name)	(FP_MONADIC_REAL) XPTR(name)
#define XDR(name)	(FP_DYADIC_REAL) XPTR(name)
#define XTR(name)	(FP_TRIADIC_REAL) XPTR(name)

#define XMC(name)	(FP_MONADIC_CMPLX) XPTR(name)
#define XDC(name)	(FP_DYADIC_CMPLX) XPTR(name)

#define XMI(name)	(FP_MONADIC_INT) XPTR(name)
#define XDI(name)	(FP_DYADIC_INT) XPTR(name)
#define XTI(name)	(FP_TRIADIC_INT) XPTR(name)

#define XNIL(name)	(FP_NILADIC) XPTR(name)
#define XARG(name)	(FP_RARG) XPTR(name)
#define XMULTI(name)	(FP_MULTI) XPTR(name)


/* Infrared command wrappers to maintain binary compatibility across images */
#ifdef INFRARED
#define IRN(x)		& (x)
#define IRA(x)		& (x)
#else
#define IRN(x)		(FP_NILADIC) NOFN
#define IRA(x)		NOFN
#endif


#ifdef SHORT_POINTERS
#ifndef COMMANDS_PASS
#define COMMANDS_PASS 1
#else
/*
 *  This is pass 2 of the compile.
 *
 *  Create a dummy segment and store the full blown tables there
 */
#define CMDTAB __attribute__((section(".cmdtab"),used))

/*
 *  Help the post-processor to find the data in the flash image
 */
extern const struct monfunc_cmdtab monfuncs_ct[];
extern const struct dyfunc_cmdtab dyfuncs_ct[];
extern const struct trifunc_cmdtab trifuncs_ct[];
extern const struct niladic_cmdtab niladics_ct[];
extern const struct argcmd_cmdtab argcmds_ct[];
extern const struct multicmd_cmdtab multicmds_ct[];

CMDTAB __attribute__((externally_visible))
const struct _command_info command_info = {
	NUM_MONADIC, monfuncs,  monfuncs_ct,
	NUM_DYADIC,  dyfuncs,   dyfuncs_ct,
	NUM_TRIADIC, trifuncs,  trifuncs_ct,
	NUM_NILADIC, niladics,  niladics_ct,
	NUM_RARG,    argcmds,   argcmds_ct,
	NUM_MULTI,   multicmds, multicmds_ct,
};

#endif
#endif

// Dummy definition for catalogue generation
#ifdef COMPILE_CATALOGUES
#undef NOFN 
#else
#define NOFN NULL
#endif

/* Define our table of monadic functions.
 * These must be in the same order as the monadic function enum but we'll
 * validate this only if debugging is enabled.
 */
#ifdef COMPILE_CATALOGUES
#undef NOFN
#define FUNC(name, d, c, i, fn, alias) { #d, #c, #i, fn, alias },
#elif DEBUG
#define FUNC(name, d, c, i, fn, alias) { name, d, c, i, fn, alias },
#elif COMMANDS_PASS == 1
#define FUNC(name, d, c, i, fn, alias) { 0xaa55, 0x55aa, 0xa55a, fn },
#elif defined(REALBUILD)
#define FUNC(name, d, c, i, fn, alias) { d, c, i, fn },
#else
#define FUNC(name, d, c, i, fn, alias) { d, c, i, fn, alias },
#endif

#if COMMANDS_PASS == 2
CMDTAB const struct monfunc_cmdtab monfuncs_ct[ NUM_MONADIC ] = {
#else
const struct monfunc monfuncs[ NUM_MONADIC ] = {
#endif
	FUNC(OP_FRAC,	&decNumberFrac,		XMC(cpx_FRAC),	&intFP,		"FP",		CNULL)
	FUNC(OP_FLOOR,	&decNumberFloor,	NOFN,		&intIP,		"FLOOR",	CNULL)
	FUNC(OP_CEIL,	&decNumberCeil,		NOFN,		&intIP,		"CEIL",		CNULL)
	FUNC(OP_ROUND,	&decNumberRound,	NOFN,		&intIP,		"ROUNDI",	CNULL)
	FUNC(OP_TRUNC,	&decNumberTrunc,	XMC(cpx_TRUNC),	&intIP,		"IP",		CNULL)
	FUNC(OP_ABS,	&dn_abs,		&cmplxAbs,	&intAbs,	"ABS",		CNULL)
	FUNC(OP_RND,	&decNumberRnd,		XMC(cpx_ROUND),	&intIP,		"ROUND",	CNULL)
	FUNC(OP_SIGN,	XMR(SIGN),		XMC(cpx_SIGN),	&intSign,	"SIGN",		CNULL)
	FUNC(OP_LN,	&dn_ln,			&cmplxLn,	&intMonadic,	"LN",		CNULL)
	FUNC(OP_EXP,	&dn_exp,		&cmplxExp,	&intMonadic,	"e\234",	"EXP")
	FUNC(OP_SQRT,	&dn_sqrt,		&cmplxSqrt,	&intSqrt,	"\003",		"SQRT")
	FUNC(OP_RECIP,	&decNumberRecip,	&cmplxRecip,	NOFN,		"1/x",		"INV")
	FUNC(OP__1POW,	&decNumberPow_1,	&cmplx_1x,	&int_1pow,	"(-1)\234",	"(-1)^x")
	FUNC(OP_LOG,	&dn_log10,		XMC(cpx_LOG10),	&intLog10,	"LOG\271\270",	"LG")
	FUNC(OP_LG2,	&dn_log2,		XMC(cpx_LOG2),	&intLog2,	"LOG\272",	"LB")
	FUNC(OP_2POWX,	&decNumberPow2,		XMC(cpx_POW2),	&int2pow,	"2\234",	"2^x")
	FUNC(OP_10POWX,	&decNumberPow10,	XMC(cpx_POW10),	&int10pow,	"10\234",	"10^x")
	FUNC(OP_LN1P,	&decNumberLn1p,		XMC(cpx_LN1P),	NOFN,		"LN1+x",	CNULL)
	FUNC(OP_EXPM1,	&decNumberExpm1,	XMC(cpx_EXPM1),	NOFN,		"e\234-1",	"EXP-1")
	FUNC(OP_LAMW,	XMR(W0),		XMC(CPX_W0),	NOFN,		"W\276",	"W0")
	FUNC(OP_LAMW1,	XMR(W1),		NOFN,		NOFN,		"W\033",	"W1")
	FUNC(OP_INVW,	XMR(W_INVERSE),		XMC(CPX_W_INVERSE),NOFN,	"W\235",	"INV-W")
	FUNC(OP_SQR,	&decNumberSquare,	XMC(cpx_x2),	&intSqr,	"x\232",	"x^2")
	FUNC(OP_CUBE,	&decNumberCube,		XMC(cpx_x3),	&intCube,	"x\200",	"x^3")
	FUNC(OP_CUBERT,	&decNumberCubeRoot,	&cmplxCubeRoot,	&intMonadic,	"\200\003",	"CROOT")
	FUNC(OP_FIB,	XMR(FIB),		XMC(CPX_FIB),	&intFib,	"FIB",		CNULL)
	FUNC(OP_2DEG,	&decNumberDRG,		NOFN,		NOFN,		"\015DEG",	">DEG")
	FUNC(OP_2RAD,	&decNumberDRG,		NOFN,		NOFN,		"\015RAD",	">RAD")
	FUNC(OP_2GRAD,	&decNumberDRG,		NOFN,		NOFN,		"\015GRAD",	">GRAD")
	FUNC(OP_DEG2,	&decNumberDRG,		NOFN,		NOFN,		"DEG\015",	"DEG>")
	FUNC(OP_RAD2,	&decNumberDRG,		NOFN,		NOFN,		"RAD\015",	"RAD>")
	FUNC(OP_GRAD2,	&decNumberDRG,		NOFN,		NOFN,		"GRAD\015",	"GRAD>")
	FUNC(OP_SIN,	&decNumberSin,		&cmplxSin,	NOFN,		"SIN",		CNULL)
	FUNC(OP_COS,	&decNumberCos,		&cmplxCos,	NOFN,		"COS",		CNULL)
	FUNC(OP_TAN,	&decNumberTan,		&cmplxTan,	NOFN,		"TAN",		CNULL)
	FUNC(OP_ASIN,	&decNumberArcSin,	XMC(cpx_ASIN),	NOFN,		"ASIN",		CNULL)
	FUNC(OP_ACOS,	&decNumberArcCos,	XMC(cpx_ACOS),	NOFN,		"ACOS",		CNULL)
	FUNC(OP_ATAN,	&decNumberArcTan,	XMC(cpx_ATAN),	NOFN,		"ATAN",		CNULL)
	FUNC(OP_SINC,	&decNumberSinc,		&cmplxSinc,	NOFN,		"SINC",		CNULL)
	FUNC(OP_SINH,	&decNumberSinh,		&cmplxSinh,	NOFN,		"SINH",		CNULL)
	FUNC(OP_COSH,	&decNumberCosh,		&cmplxCosh,	NOFN,		"COSH",		CNULL)
	FUNC(OP_TANH,	&decNumberTanh,		&cmplxTanh,	NOFN,		"TANH",		CNULL)
	FUNC(OP_ASINH,	&decNumberArcSinh,	XMC(cpx_ASINH),	NOFN,		"ASINH",	CNULL)
	FUNC(OP_ACOSH,	&decNumberArcCosh,	XMC(cpx_ACOSH),	NOFN,		"ACOSH",	CNULL)
	FUNC(OP_ATANH,	&decNumberArcTanh,	XMC(cpx_ATANH),	NOFN,		"ATANH",	CNULL)
#ifdef INCLUDE_GUDERMANNIAN
	FUNC(OP_GUDER,	XMR(gd),		XMC(cpx_gd),	NOFN,		"g\206",	"GUD")
	FUNC(OP_INVGUD,	XMR(inv_gd),		XMC(cpx_inv_gd),NOFN,		"g\206\235",	"INV-GUD")
#endif

	FUNC(OP_FACT,	&decNumberFactorial,	XMC(cpx_FACT),	&intMonadic,	"x!",		CNULL)
	FUNC(OP_GAMMA,	&decNumberGamma,	&cmplxGamma,	&intMonadic,	"\202",		"GAMMA")
	FUNC(OP_LNGAMMA,&decNumberLnGamma,	&cmplxLnGamma,	NOFN,		"LN\202",	"LNGAMMA")
	FUNC(OP_DEG2RAD,&decNumberD2R,		NOFN,		NOFN,		"\005\015rad",	"DEG>RAD")
	FUNC(OP_RAD2DEG,&decNumberR2D,		NOFN,		NOFN,		"rad\015\005",	"RAD>DEG")
	FUNC(OP_DEG2GRD,&decNumberD2G,		NOFN,		NOFN,		"\005\015G",	"DEG>GRAD")
	FUNC(OP_GRD2DEG,&decNumberG2D,		NOFN,		NOFN,		"G\015\005",	"GRAD>DEG")
	FUNC(OP_RAD2GRD,&decNumberR2G,		NOFN,		NOFN,		"rad\015G",	"RAD>GRAD")
	FUNC(OP_GRD2RAD,&decNumberG2R,		NOFN,		NOFN,		"G\015rad",	"GRAD>RAD")
	FUNC(OP_CCHS,	NOFN,			&cmplxMinus,	NOFN,		"\024+/-",	"c+/-")
	FUNC(OP_CCONJ,	NOFN,			XMC(cpx_CONJ),	NOFN,		"CONJ",		CNULL)
	FUNC(OP_ERF,	XMR(ERF),		NOFN,		NOFN,		"erf",		CNULL)
	FUNC(OP_ERFC,	XMR(ERFC),		NOFN,		NOFN,		"erfc",		CNULL)
	FUNC(OP_pdf_Q,	XMR(PDF_Q), 		NOFN,		NOFN,		"\264(x)",	"phi(x)")
	FUNC(OP_cdf_Q,	XMR(CDF_Q), 		NOFN,		NOFN,		"\224(x)",	"PHI(x)")
	FUNC(OP_qf_Q,	XMR(QF_Q),  		NOFN,		NOFN,		"\224\235(p)",	"INV-PHI")
	FUNC(OP_pdf_chi2, XMR(PDF_CHI2),	NOFN,		NOFN,		"\265\232\276",	"chi2-p")
	FUNC(OP_cdf_chi2, XMR(CDF_CHI2),	NOFN,		NOFN,		"\265\232",	"CHI2")
	FUNC(OP_qf_chi2,  XMR(QF_CHI2),		NOFN,		NOFN,		"\265\232INV",	"INV-CHI2")
	FUNC(OP_pdf_T,	XMR(PDF_T),		NOFN,		NOFN,		"t\276(x)",	"t-p(x)")
	FUNC(OP_cdf_T,	XMR(CDF_T),		NOFN,		NOFN,		"t(x)",		CNULL)
	FUNC(OP_qf_T,	XMR(QF_T),		NOFN,		NOFN,		"t\235(p)",	"INV-t")
	FUNC(OP_pdf_F,	XMR(PDF_F),		NOFN,		NOFN,		"F\276(x)",	"F-p(x)")
	FUNC(OP_cdf_F,	XMR(CDF_F),		NOFN,		NOFN,		"F(x)",		CNULL)
	FUNC(OP_qf_F,	XMR(QF_F),		NOFN,		NOFN,		"F\235(p)",	"INV-F")
	FUNC(OP_pdf_WB,	XMR(PDF_WEIB),		NOFN,		NOFN,		"Weibl\276",	"Weibl-p")
	FUNC(OP_cdf_WB,	XMR(CDF_WEIB),		NOFN,		NOFN,		"Weibl",	CNULL)
	FUNC(OP_qf_WB,	XMR(QF_WEIB),		NOFN,		NOFN,		"Weibl\235",	"INV-Weibl")
	FUNC(OP_pdf_EXP,XMR(PDF_EXPON),		NOFN,		NOFN,		"Expon\276",	"Expon-p")
	FUNC(OP_cdf_EXP,XMR(CDF_EXPON),		NOFN,		NOFN,		"Expon",	CNULL)
	FUNC(OP_qf_EXP,	XMR(QF_EXPON),		NOFN,		NOFN,		"Expon\235",	"INV-Expon")
	FUNC(OP_pdf_B,	XMR(PDF_BINOMIAL),	NOFN,		NOFN,		"Binom\276",	"Binom-p")
	FUNC(OP_cdf_B,	XMR(CDF_BINOMIAL),	NOFN,		NOFN,		"Binom",	CNULL)
	FUNC(OP_qf_B,	XMR(QF_BINOMIAL),	NOFN,		NOFN,		"Binom\235",	"INV-Binom")
	FUNC(OP_pdf_Plam, XMR(PDF_POISSON),	NOFN,		NOFN,		"Pois\252\276",	"Pois-p")
	FUNC(OP_cdf_Plam, XMR(CDF_POISSON),	NOFN,		NOFN,		"Pois\252",	"Pois")
	FUNC(OP_qf_Plam,  XMR(QF_POISSON),	NOFN,		NOFN,		"Pois\252\235",	"INV-Pois")
	FUNC(OP_pdf_P,	XMR(PDF_POIS2),		NOFN,		NOFN,		"Poiss\276",	"Pois2-p")
	FUNC(OP_cdf_P,	XMR(CDF_POIS2),		NOFN,		NOFN,		"Poiss",	"Pois2")
	FUNC(OP_qf_P,	XMR(QF_POIS2),		NOFN,		NOFN,		"Poiss\235",	"INV-Pois2")
	FUNC(OP_pdf_G,	XMR(PDF_GEOM),		NOFN,		NOFN,		"Geom\276",	"Geom-p")
	FUNC(OP_cdf_G,	XMR(CDF_GEOM),		NOFN,		NOFN,		"Geom",		CNULL)
	FUNC(OP_qf_G,	XMR(QF_GEOM),		NOFN,		NOFN,		"Geom\235",	"INV-Geom")
	FUNC(OP_pdf_N,	XMR(PDF_NORMAL),	NOFN,		NOFN,		"Norml\276",	"Norml-p")
	FUNC(OP_cdf_N,	XMR(CDF_NORMAL),	NOFN,		NOFN,		"Norml",	CNULL)
	FUNC(OP_qf_N,	XMR(QF_NORMAL),		NOFN,		NOFN,		"Norml\235",	"INV-Norml")
	FUNC(OP_pdf_LN,	XMR(PDF_LOGNORMAL),	NOFN,		NOFN,		"LgNrm\276",	"LgNorm-p")
	FUNC(OP_cdf_LN,	XMR(CDF_LOGNORMAL),	NOFN,		NOFN,		"LgNrm",	CNULL)
	FUNC(OP_qf_LN,	XMR(QF_LOGNORMAL),	NOFN,		NOFN,		"LgNrm\235",	"INV-LgNorm")
	FUNC(OP_pdf_LG,	XMR(PDF_LOGIT),		NOFN,		NOFN,		"Logis\276",	"Logis-p")
	FUNC(OP_cdf_LG,	XMR(CDF_LOGIT),		NOFN,		NOFN,		"Logis",	CNULL)
	FUNC(OP_qf_LG,	XMR(QF_LOGIT),		NOFN,		NOFN,		"Logis\235",	"INV-Logis")
	FUNC(OP_pdf_C,	XMR(PDF_CAUCHY),	NOFN,		NOFN,		"Cauch\276",	"Cauch-p")
	FUNC(OP_cdf_C,	XMR(CDF_CAUCHY),	NOFN,		NOFN,		"Cauch",	CNULL)
	FUNC(OP_qf_C,	XMR(QF_CAUCHY),		NOFN,		NOFN,		"Cauch\235",	"INV-Cauch")
#ifdef INCLUDE_CDFU
	FUNC(OP_cdfu_Q,	XMR(CDFU_Q),		NOFN,		NOFN,		"\224\277(x)",	"Q-u")
	FUNC(OP_cdfu_chi2, XMR(CDFU_CHI2),	NOFN,		NOFN,		"\265\232\277",	"CHI2-u")
	FUNC(OP_cdfu_T,	XMR(CDFU_T),		NOFN,		NOFN,		"t\277(x)",	"t-u")
	FUNC(OP_cdfu_F,	XMR(CDFU_F),		NOFN,		NOFN,		"F\277(x)",	"F-u")
	FUNC(OP_cdfu_WB, XMR(CDFU_WEIB),	NOFN,		NOFN,		"Weibl\277",	"Weibl-u")
	FUNC(OP_cdfu_EXP, XMR(CDFU_EXPON),	NOFN,		NOFN,		"Expon\277",	"Expon-u")
	FUNC(OP_cdfu_B,	XMR(CDFU_BINOMIAL),	NOFN,		NOFN,		"Binom\277",	"Binom-u")
	FUNC(OP_cdfu_Plam, XMR(CDFU_POISSON),	NOFN,		NOFN,		"Pois\252\277",	"Pois-u")
	FUNC(OP_cdfu_P,	XMR(CDFU_POIS2),	NOFN,		NOFN,		"Poiss\277",	"Pois2-u")
	FUNC(OP_cdfu_G,	XMR(CDFU_GEOM),		NOFN,		NOFN,		"Geom\277",	"Geom-u")
	FUNC(OP_cdfu_N,	XMR(CDFU_NORMAL),	NOFN,		NOFN,		"Norml\277",	"Norml-u")
	FUNC(OP_cdfu_LN, XMR(CDFU_LOGNORMAL),	NOFN,		NOFN,		"LgNrm\277",	"LgNrm-u")
	FUNC(OP_cdfu_LG, XMR(CDFU_LOGIT),	NOFN,		NOFN,		"Logis\277",	"Logis-u")
	FUNC(OP_cdfu_C,	XMR(CDFU_CAUCHY),	NOFN,		NOFN,		"Cauch\277",	"Cauch-u")
#endif
	FUNC(OP_xhat,	&stats_xhat,		NOFN,		NOFN,		"\031",		"FCSTx")
	FUNC(OP_yhat,	&stats_yhat,		NOFN,		NOFN,		"\032",		"FCSTy")
	FUNC(OP_sigper,	&stats_sigper,		NOFN,		NOFN,		"%\221",	"%SUM")
	FUNC(OP_PERCNT,	XMR(PERCENT),		NOFN,		NOFN,		"%",		CNULL)
	FUNC(OP_PERCHG,	XMR(PERCHG),		NOFN,		NOFN,		"\203%",	"%CH")
	FUNC(OP_PERTOT,	XMR(PERTOT),		NOFN,		NOFN,		"%T",		CNULL)
	FUNC(OP_HMS2,	&decNumberHMS2HR,	NOFN,		NOFN,		"\015HR",	">HR")
	FUNC(OP_2HMS,	&decNumberHR2HMS,	NOFN,		NOFN,		"\015H.MS",	">H.MS")
	FUNC(OP_NOT,	&decNumberNot,		NOFN,		&intNot,	"NOT",		CNULL)
	FUNC(OP_BITCNT,	NOFN,			NOFN,		&intNumBits,	"nBITS",	CNULL)
	FUNC(OP_MIRROR,	NOFN,			NOFN,		&intMirror,	"MIRROR",	CNULL)
	FUNC(OP_DOWK,	&dateDayOfWeek,		NOFN,		NOFN,		"WDAY",		CNULL)
	FUNC(OP_D2J,	&dateToJ,		NOFN,		NOFN,		"D\015J",	"D>J")
	FUNC(OP_J2D,	&dateFromJ,		NOFN,		NOFN,		"J\015D",	"J>D")
	FUNC(OP_DEGC_F,	&convC2F,		NOFN,		NOFN,		"\005C\015\005F", "C>F")
	FUNC(OP_DEGF_C,	&convF2C,		NOFN,		NOFN,		"\005F\015\005C", "F>C")
	FUNC(OP_DB_AR,	&convDB2AR,		NOFN,		NOFN,		"dB\015ar.",	"dB>ar.")
	FUNC(OP_AR_DB,	&convAR2DB,		NOFN,		NOFN,		"ar.\015dB",	"ar.>dB")
	FUNC(OP_DB_PR,	&convDB2PR,		NOFN,		NOFN,		"dB\015pr.",	"dB>pr.")
	FUNC(OP_PR_DB,	&convPR2DB,		NOFN,		NOFN,		"pr.\015dB",	"pr.>dB")
	FUNC(OP_ZETA,	XMR(ZETA),		NOFN,		NOFN,		"\245",		"ZETA")
	FUNC(OP_Bn,	XMR(Bn),		NOFN,		NOFN,		"B\275",	"Bn")
	FUNC(OP_BnS,	XMR(Bn_star),		NOFN,		NOFN,		"B\275\220",	"Bn*")

#ifdef INCLUDE_EASTER
	FUNC(OP_EASTER,	&dateEaster,		NOFN,		NOFN,		"EASTER",	CNULL)
#endif
#ifdef INCLUDE_FACTOR
	FUNC(OP_FACTOR,	&decFactor,		NOFN,		&intFactor,	"FACTOR",	CNULL)
#endif
	FUNC(OP_DATE_YEAR, &dateExtraction,	NOFN,		NOFN,		"YEAR",		CNULL)
	FUNC(OP_DATE_MONTH, &dateExtraction,	NOFN,		NOFN,		"MONTH",	CNULL)
	FUNC(OP_DATE_DAY, &dateExtraction,	NOFN,		NOFN,		"DAY",		CNULL)
#ifdef INCLUDE_USER_IO
	FUNC(OP_RECV1,	&decRecv,		NOFN,		&intRecv,	"RECV1",	CNULL)
#endif
#ifdef INCLUDE_MANTISSA
	FUNC(OP_MANTISSA, &decNumberMantissa,	NOFN,		NOFN,		"MANT",		CNULL)
	FUNC(OP_EXPONENT, &decNumberExponent,	NOFN,		NOFN,		"EXPT",		CNULL)
	FUNC(OP_ULP,	  &decNumberULP,	NOFN,		XMI(int_ULP),	"ULP",		CNULL)
#endif
	FUNC(OP_MAT_ALL, &matrix_all,		NOFN,		NOFN,		"M-ALL",	CNULL)
	FUNC(OP_MAT_DIAG, &matrix_diag,		NOFN,		NOFN,		"M-DIAG",	CNULL)
	FUNC(OP_MAT_TRN, &matrix_transpose,	NOFN,		NOFN,		"TRANSP",	CNULL)
	FUNC(OP_MAT_RQ,	&matrix_rowq,		NOFN,		NOFN,		"nROW",		CNULL)
	FUNC(OP_MAT_CQ,	&matrix_colq,		NOFN,		NOFN,		"nCOL",		CNULL)
	FUNC(OP_MAT_IJ,	&matrix_getrc,		NOFN,		NOFN,		"M.IJ",		CNULL)
	FUNC(OP_MAT_DET, &matrix_determinant,	NOFN,		NOFN,		"DET",		CNULL)
#ifdef MATRIX_LU_DECOMP
	FUNC(OP_MAT_LU, &matrix_lu_decomp,	NOFN,		NOFN,		"M.LU",		CNULL)
#endif
#ifdef INCLUDE_XROM_DIGAMMA
	FUNC(OP_DIGAMMA,XMR(DIGAMMA),		XMC(CPX_DIGAMMA),	NOFN,	"\226",		"DIGAMMA")
#endif
#undef FUNC
};


/* Define our table of dyadic functions.
 * These must be in the same order as the dyadic function enum but we'll
 * validate this only if debugging is enabled.
 */
#ifdef COMPILE_CATALOGUES
#define FUNC(name, d, c, i, fn, alias) { #d, #c, #i, fn, alias },
#elif DEBUG
#define FUNC(name, d, c, i, fn, alias) { name, d, c, i, fn, alias },
#elif COMMANDS_PASS == 1
#define FUNC(name, d, c, i, fn, alias) { 0xaa55, 0x55aa, 0xa55a, fn },
#elif defined(REALBUILD)
#define FUNC(name, d, c, i, fn, alias) { d, c, i, fn },
#else
#define FUNC(name, d, c, i, fn, alias) { d, c, i, fn, alias },
#endif

#if COMMANDS_PASS == 2
CMDTAB const struct dyfunc_cmdtab dyfuncs_ct[ NUM_DYADIC ] = {
#else
const struct dyfunc dyfuncs[ NUM_DYADIC ] = {
#endif
	FUNC(OP_POW,	&dn_power,		&cmplxPower,	&intPower,	"y\234",	"y^x")
	FUNC(OP_ADD,	&dn_add,		&cmplxAdd,	&intAdd,	"+",		CNULL)
	FUNC(OP_SUB,	&dn_subtract,		&cmplxSubtract,	&intSubtract,	"-",		CNULL)
	FUNC(OP_MUL,	&dn_multiply,		&cmplxMultiply,	&intMultiply,	"\034",		"*")
	FUNC(OP_DIV,	&dn_divide,		&cmplxDivide,	&intDivide,	"/",		CNULL)
#ifdef INCLUDE_INTEGER_DIVIDE
	FUNC(OP_IDIV,	XDR(IDIV),		XDC(cpx_IDIV),	&intDivide,	"IDIV",		CNULL)
#endif
	FUNC(OP_MOD,	&decNumberBigMod,	NOFN,		&intMod,	"RMDR",		CNULL)
#ifdef INCLUDE_MOD41
	FUNC(OP_MOD41,	&decNumberBigMod,	NOFN,		&intMod,	"MOD",		CNULL)
#endif
	FUNC(OP_LOGXY,	&decNumberLogxy,	XDC(cpx_LOGXY),	&intDyadic,	"LOG\213",	"LOGx")
	FUNC(OP_MIN,	&dn_min,		NOFN,		&intMin,	"MIN",		CNULL)
	FUNC(OP_MAX,	&dn_max,		NOFN,		&intMax,	"MAX",		CNULL)
	FUNC(OP_ATAN2,	&decNumberArcTan2,	NOFN,		NOFN,		"ANGLE",	CNULL)
	FUNC(OP_BETA,	XDR(beta),		XDC(cpx_beta),	NOFN,		"\241",		"BETA")
	FUNC(OP_LNBETA,	&decNumberLnBeta,	XDC(cpx_lnbeta),NOFN,		"LN\241",	"LNBETA")
	FUNC(OP_GAMMAg,	&decNumberGammap,	NOFN,		NOFN,		"\242\213\214",	"gammaxy")
	FUNC(OP_GAMMAG,	&decNumberGammap,	NOFN,		NOFN,		"\202\213\214",	"GAMMAxy")
	FUNC(OP_GAMMAP,	&decNumberGammap,	NOFN,		NOFN,		"\202\276",	"GAMMAP")
	FUNC(OP_GAMMAQ,	&decNumberGammap,	NOFN,		NOFN,		"\202\223",	"GAMMAQ")
#ifdef INCLUDE_ELLIPTIC
	FUNC(OP_SN,	&decNumberSN,		&cmplxSN,	NOFN,		"SN",		CNULL)
	FUNC(OP_CN,	&decNumberCN,		&cmplxCN,	NOFN,		"CN",		CNULL)
	FUNC(OP_DN,	&decNumberDN,		&cmplxDN,	NOFN,		"DN",		CNULL)
#endif
	FUNC(OP_COMB,	&decNumberComb,		XDC(CPX_COMB),	&intDyadic,	"COMB",		CNULL)
	FUNC(OP_PERM,	&decNumberPerm,		XDC(CPX_PERM),	&intDyadic,	"PERM",		CNULL)
	FUNC(OP_PERMG,	XDR(PERMARGIN),		NOFN,		NOFN,		"%+MG",		CNULL)
	FUNC(OP_MARGIN,	XDR(MARGIN),		NOFN,		NOFN,		"%MG",		CNULL)
	FUNC(OP_PARAL,	XDR(PARL),		XDC(CPX_PARL),	XDI(PARL),	"||",		CNULL)
	FUNC(OP_AGM,	XDR(AGM),		XDC(CPX_AGM),	NOFN,		"AGM",		CNULL)
	FUNC(OP_HMSADD,	&decNumberHMSAdd,	NOFN,		NOFN,		"H.MS+",	CNULL)
	FUNC(OP_HMSSUB,	&decNumberHMSSub,	NOFN,		NOFN,		"H.MS-",	CNULL)
	FUNC(OP_GCD,	&decNumberGCD,		NOFN,		&intGCD,	"GCD",		CNULL)
	FUNC(OP_LCM,	&decNumberLCM,		NOFN,		&intLCM,	"LCM",		CNULL)

	FUNC(OP_LAND,	&decNumberBooleanOp,	NOFN,		&intBooleanOp,	"AND",		CNULL)
	FUNC(OP_LOR,	&decNumberBooleanOp,	NOFN,		&intBooleanOp,	"OR",		CNULL)
	FUNC(OP_LXOR,	&decNumberBooleanOp,	NOFN,		&intBooleanOp,	"XOR",		CNULL)
	FUNC(OP_LNAND,	&decNumberBooleanOp,	NOFN,		&intBooleanOp,	"NAND",		CNULL)
	FUNC(OP_LNOR,	&decNumberBooleanOp,	NOFN,		&intBooleanOp,	"NOR",		CNULL)
	FUNC(OP_LXNOR,	&decNumberBooleanOp,	NOFN,		&intBooleanOp,	"XNOR",		CNULL)

	FUNC(OP_DTADD,	XDR(DATE_ADD),		NOFN,		NOFN,		"DAYS+",	CNULL)
	FUNC(OP_DTDIF,	XDR(DATE_DELTA),	NOFN,		NOFN,		"\203DAYS",	"DDAYS")

	FUNC(OP_LEGENDRE_PN,	XDR(LegendrePn),	NOFN,	NOFN,		"P\275",	"Pn")
	FUNC(OP_CHEBYCHEV_TN,	XDR(ChebychevTn),	NOFN,	NOFN,		"T\275",	"Tn")
	FUNC(OP_CHEBYCHEV_UN,	XDR(ChebychevUn),	NOFN,	NOFN,		"U\275",	"Un")
	FUNC(OP_LAGUERRE,	XDR(LaguerreLn),	NOFN,	NOFN,		"L\275",	"Ln")
	FUNC(OP_HERMITE_HE,	XDR(HermiteHe),		NOFN,	NOFN,		"H\275",	"Hn")
	FUNC(OP_HERMITE_H,	XDR(HermiteH),		NOFN,	NOFN,		"H\275\276",	"Hnp")
#ifdef INCLUDE_XROOT
	FUNC(OP_XROOT,	&decNumberXRoot,	&cmplxXRoot,	&intDyadic,	"\234\003y",	"XROOT")
#endif
	FUNC(OP_MAT_ROW, &matrix_row,		NOFN,		NOFN,		"M-ROW",	CNULL)
	FUNC(OP_MAT_COL, &matrix_col,		NOFN,		NOFN,		"M-COL",	CNULL)
	FUNC(OP_MAT_COPY, &matrix_copy,		NOFN,		NOFN,		"M.COPY",	CNULL)
#ifdef INCLUDE_MANTISSA
	FUNC(OP_NEIGHBOUR,&decNumberNeighbour,	NOFN,		NOFN,		"NEIGHB",	CNULL)
#endif
#ifdef INCLUDE_XROM_BESSEL
	FUNC(OP_BESJN,	XDR(BES_JN),		XDC(CPX_JN),	NOFN,		"Jn",		CNULL)
	FUNC(OP_BESIN,	XDR(BES_IN),		XDC(CPX_IN),	NOFN,		"In",		CNULL)
	FUNC(OP_BESYN,	XDR(BES_YN),		XDC(CPX_YN),	NOFN,		"Yn",		CNULL)
	FUNC(OP_BESKN,	XDR(BES_KN),		XDC(CPX_KN),	NOFN,		"Kn",		CNULL)
#endif
#undef FUNC
};

/* Define our table of triadic functions.
 * These must be in the same order as the triadic function enum but we'll
 * validate this only if debugging is enabled.
 */
#ifdef COMPILE_CATALOGUES
#define FUNC(name, d, i, fn, alias) { #d, #i, fn, alias },
#elif DEBUG
#define FUNC(name, d, i, fn, alias) { name, d, i, fn, alias },
#elif COMMANDS_PASS == 1
#define FUNC(name, d, i, fn, alias) { 0xaa55, 0xa55a, fn },
#elif defined(REALBUILD)
#define FUNC(name, d, i, fn, alias) { d, i, fn },
#else
#define FUNC(name, d, i, fn, alias) { d, i, fn, alias },
#endif

#if COMMANDS_PASS == 2
CMDTAB const struct trifunc_cmdtab trifuncs_ct[ NUM_TRIADIC ] = {
#else
const struct trifunc trifuncs[ NUM_TRIADIC ] = {
#endif
	FUNC(OP_BETAI,		&betai,			(FP_TRIADIC_INT) NOFN,	"I\213",	"IBETA")
	FUNC(OP_DBL_DIV, 	(FP_TRIADIC_REAL) NOFN,	&intDblDiv,		"DBL/",		CNULL)
	FUNC(OP_DBL_MOD, 	(FP_TRIADIC_REAL) NOFN,	&intDblRmdr,		"DBLR",		CNULL)
#ifdef INCLUDE_MULADD
	FUNC(OP_MULADD, 	&decNumberMAdd,		&intMAdd,		"\034+",	"*+")
#endif
	FUNC(OP_PERMRR,		XTR(PERMMR),		(FP_TRIADIC_INT) NOFN,	"%MRR",		CNULL)
        FUNC(OP_GEN_LAGUERRE,   XTR(LaguerreLnA),	(FP_TRIADIC_INT) NOFN,	"L\275\240",	"LnAlpha")

	FUNC(OP_MAT_MUL,	&matrix_multiply,	(FP_TRIADIC_INT) NOFN,	"M\034",	"M*")
	FUNC(OP_MAT_GADD,	&matrix_genadd,		(FP_TRIADIC_INT) NOFN,	"M+\034",	"M+*")
	FUNC(OP_MAT_REG,	&matrix_getreg,		(FP_TRIADIC_INT) NOFN,	"M.REG",	CNULL)
	FUNC(OP_MAT_LIN_EQN,	&matrix_linear_eqn,	(FP_TRIADIC_INT) NOFN,	"LINEQS",	CNULL)
	FUNC(OP_TO_DATE,	&dateFromYMD,		(FP_TRIADIC_INT) NOFN,	"\015DATE",	">DATE")
#ifdef INCLUDE_INT_MODULO_OPS
	FUNC(OP_MULMOD, 	(FP_TRIADIC_REAL) NOFN,	&intmodop,		"\034MOD",	CNULL)
	FUNC(OP_EXPMOD, 	(FP_TRIADIC_REAL) NOFN,	&intmodop,		"^MOD",		CNULL)
#endif
#undef FUNC
};



#ifdef COMPILE_CATALOGUES
#define FUNC(name, d, fn, arg, alias)		{ #d, arg, fn, alias },
#elif DEBUG
#define FUNC(name, d, fn, arg, alias)		{ name, d, arg, fn, alias },
#elif COMMANDS_PASS == 1
#define FUNC(name, d, fn, arg, alias)		{ 0xaa55, arg, fn },
#elif defined(REALBUILD)
#define FUNC(name, d, fn, arg, alias)		{ d, arg, fn },
#else
#define FUNC(name, d, fn, arg, alias)		{ d, arg, fn, alias },
#endif

#define FUNC0(name, d, fn, alias)	FUNC(name, d, fn, 0, alias)
#define FUNC1(name, d, fn, alias)	FUNC(name, d, fn, 1, alias)
#define FUNC2(name, d, fn, alias)	FUNC(name, d, fn, 2, alias)
#define FN_I0(name, d, fn, alias)	FUNC(name, d, fn, NILADIC_NOINT | 0, alias)
#define FN_I1(name, d, fn, alias)	FUNC(name, d, fn, NILADIC_NOINT | 1, alias)
#define FN_I2(name, d, fn, alias)	FUNC(name, d, fn, NILADIC_NOINT | 2, alias)

#if COMMANDS_PASS == 2
CMDTAB const struct niladic_cmdtab niladics_ct[ NUM_NILADIC ] = {
#else
const struct niladic niladics[ NUM_NILADIC ] = {
#endif
	FUNC0(OP_NOP,		(FP_NILADIC) NOFN,	"NOP",		CNULL)
	FUNC0(OP_VERSION,	&version,		"VERS",		CNULL)
	FUNC0(OP_OFF,		&cmd_off,		"OFF",		CNULL)
	FUNC1(OP_STKSIZE,	&get_stack_size,	"SSIZE?",	CNULL)
	FUNC0(OP_STK4,		XNIL(STACK_4_LEVEL),	"SSIZE4",	CNULL)
	FUNC0(OP_STK8,		XNIL(STACK_8_LEVEL),	"SSIZE8",	CNULL)
	FUNC1(OP_INTSIZE,	&get_word_size,		"WSIZE?",	CNULL)
	FUNC0(OP_RDOWN,		&roll_down,		"R\017",	"RDN")
	FUNC0(OP_RUP,		&roll_up,		"R\020",	"RUP")
	FUNC0(OP_CRDOWN,	&cpx_roll_down,		"\024R\017",	"cRDN")
	FUNC0(OP_CRUP,		&cpx_roll_up,		"\024R\020",	"cRUP")
	FUNC0(OP_CENTER,	&cpx_enter,		"\024ENTER",	"cENTER")
	FUNC0(OP_FILL,		&fill,			"FILL",		CNULL)
	FUNC0(OP_CFILL,		&cpx_fill,		"\024FILL",	"cFILL")
	FUNC0(OP_DROP,		&drop,			"DROP",		CNULL)
	FUNC0(OP_DROPXY,	&drop,			"\024DROP",	"cDROP")
	FN_I1(OP_sigmaX2Y,	&sigma_val,		"\221x\232y",	"SUMx2y")
	FN_I1(OP_sigmaX2,	&sigma_val,		"\221x\232",	"SUMx2")
	FN_I1(OP_sigmaY2,	&sigma_val,		"\221y\232",	"SUMy2")
	FN_I1(OP_sigmaXY,	&sigma_val,		"\221xy",	"SUMxy")
	FN_I1(OP_sigmaX,	&sigma_val,		"\221x",	"SUMx")
	FN_I1(OP_sigmaY,	&sigma_val,		"\221y",	"SUMy")
	FN_I1(OP_sigmalnX,	&sigma_val,		"\221lnx",	"SUMlnx")
	FN_I1(OP_sigmalnXlnX,	&sigma_val,		"\221ln\232x",	"SUMln2x")
	FN_I1(OP_sigmalnY,	&sigma_val,		"\221lny",	"SUMlny")
	FN_I1(OP_sigmalnYlnY,	&sigma_val,		"\221ln\232y",	"SUMln2y")
	FN_I1(OP_sigmalnXlnY,	&sigma_val,		"\221lnxy",	"SUMlnxy")
	FN_I1(OP_sigmaXlnY,	&sigma_val,		"\221xlny",	"SUMxlny")
	FN_I1(OP_sigmaYlnX,	&sigma_val,		"\221ylnx",	"SUMylnx")
	FN_I1(OP_sigmaN,	&sigma_val,		"n\221",	"nSUM")
	FN_I2(OP_statS,		&stats_deviations,	"s",		CNULL)
	FN_I2(OP_statSigma,	&stats_deviations,	"\261",		"sigma")
	FN_I2(OP_statGS,	&stats_deviations,	"\244",		"epsilon")
	FN_I2(OP_statGSigma,	&stats_deviations,	"\244\276",	"epsilon-pop")
	FN_I1(OP_statWS,	&stats_wdeviations,	"sw",		CNULL)
	FN_I1(OP_statWSigma,	&stats_wdeviations,	"\261w",	"sigma-w")
	FN_I2(OP_statMEAN,	&stats_mean,		"\001",		"MEAN")
	FN_I1(OP_statWMEAN,	&stats_wmean,		"\001w",	"MEAN-w")
	FN_I2(OP_statGMEAN,	&stats_gmean,		"\001g",	"GEOMEAN")
	FN_I1(OP_statR,		&stats_correlation,	"CORR",		CNULL)
	FN_I2(OP_statLR,	&stats_LR,		"L.R.",		CNULL)
	FN_I2(OP_statSErr,	&stats_deviations,	"SERR",		CNULL)
	FN_I2(OP_statGSErr,	&stats_deviations,	"\244m",	"epsilon-m")
	FN_I1(OP_statWSErr,	&stats_wdeviations,	"SERRw",	CNULL)
	FN_I1(OP_statCOV,	&stats_COV,		"COV",		CNULL)
	FN_I1(OP_statSxy,	&stats_COV,		"s\213\214",	"sxy")
	FUNC0(OP_LINF,		&stats_mode,		"LinF",		CNULL)
	FUNC0(OP_EXPF,		&stats_mode,		"ExpF",		CNULL)
	FUNC0(OP_PWRF,		&stats_mode,		"PowerF",	CNULL)
	FUNC0(OP_LOGF,		&stats_mode,		"LogF",		CNULL)
	FUNC0(OP_BEST,		&stats_mode,		"BestF",	CNULL)
	FUNC1(OP_RANDOM,	&stats_random,		"RAN#",		CNULL)
	FUNC0(OP_STORANDOM,	&stats_sto_random,	"SEED",		CNULL)
	FUNC0(OP_DEG,		XNIL(DEGREES),		"DEG",		CNULL)
	FUNC0(OP_RAD,		XNIL(RADIANS),		"RAD",		CNULL)
	FUNC0(OP_GRAD,		XNIL(GRADIANS),		"GRAD",		CNULL)
	FUNC0(OP_RTN,		&op_rtn,		"RTN",		CNULL)
	FUNC0(OP_RTNp1,		&op_rtn,		"RTN+1",	CNULL)
	FUNC0(OP_END,		&op_rtn,		"END",		CNULL)
	FUNC0(OP_RS,		&op_rs,			"STOP",		CNULL)
	FUNC0(OP_PROMPT,	&op_prompt,		"PROMPT",	CNULL)
	FUNC0(OP_SIGMACLEAR,	&sigma_clear,		"CL\221",	"CLSUMS")
	FUNC0(OP_CLREG,		&clrreg,		"CLREGS",	CNULL)
	FUNC0(OP_rCLX,		&clrx,			"CLx",		CNULL)
	FUNC0(OP_CLSTK,		&clrstk,		"CLSTK",	CNULL)
	FUNC0(OP_CLALL,		NOFN,			"CLALL",	CNULL)
	FUNC0(OP_RESET,		NOFN,			"RESET",	CNULL)
	FUNC0(OP_CLPROG,	NOFN,			"CLPROG",	CNULL)
	FUNC0(OP_CLPALL,	NOFN,			"CLPALL",	CNULL)
	FUNC0(OP_CLFLAGS,	&clrflags,		"CFALL",	CNULL)
	FN_I0(OP_R2P,		&op_r2p,		"\015POL",	">POL")
	FN_I0(OP_P2R,		&op_p2r,		"\015REC",	">REC")
	FN_I0(OP_FRACDENOM,	&op_fracdenom,		"DENMAX",	CNULL)
	FN_I1(OP_2FRAC,		&op_2frac,		"DECOMP",	CNULL)
	FUNC0(OP_DENANY,	XNIL(F_DENANY),		"DENANY",	CNULL)
	FUNC0(OP_DENFIX,	XNIL(F_DENFIX),		"DENFIX",	CNULL)
	FUNC0(OP_DENFAC,	XNIL(F_DENFAC),		"DENFAC",	CNULL)
	FUNC0(OP_FRACIMPROPER,	&op_fract,		"IMPFRC",	CNULL)
	FUNC0(OP_FRACPROPER,	&op_fract,		"PROFRC",	CNULL)
	FUNC0(OP_RADDOT,	XNIL(RADIX_DOT),	"RDX.",		CNULL)
	FUNC0(OP_RADCOM,	XNIL(RADIX_COM),	"RDX,",		CNULL)
	FUNC0(OP_THOUS_ON,	XNIL(E3ON),		"TSON",		"E3ON")
	FUNC0(OP_THOUS_OFF,	XNIL(E3OFF),		"TSOFF",	"E3OFF")
	FUNC0(OP_INTSEP_ON,	XNIL(SEPON),		"SEPON",	CNULL)
	FUNC0(OP_INTSEP_OFF,	XNIL(SEPOFF),		"SEPOFF",	CNULL)
	FUNC0(OP_FIXSCI,	XNIL(FIXSCI),		"SCIOVR",	CNULL)
	FUNC0(OP_FIXENG,	XNIL(FIXENG),		"ENGOVR",	CNULL)
	FUNC0(OP_2COMP,		XNIL(ISGN_2C),		"2COMPL",	CNULL)
	FUNC0(OP_1COMP,		XNIL(ISGN_1C),		"1COMPL",	CNULL)
	FUNC0(OP_UNSIGNED,	XNIL(ISGN_UN),		"UNSIGN",	CNULL)
	FUNC0(OP_SIGNMANT,	XNIL(ISGN_SM),		"SIGNMT",	CNULL)
	FUNC0(OP_FLOAT,		&op_float,		"DECM",		CNULL)
	FUNC0(OP_HMS,		&op_float,		"H.MS",		CNULL)
	FUNC0(OP_FRACT,		&op_fract,		"FRACT",	CNULL)
	FUNC0(OP_LEAD0,		XNIL(IM_LZON),		"LZON",		CNULL)
	FUNC0(OP_TRIM0,		XNIL(IM_LZOFF),		"LZOFF",	CNULL)
	FUNC1(OP_LJ,		&int_justify,		"LJ",		CNULL)
	FUNC1(OP_RJ,		&int_justify,		"RJ",		CNULL)
	FUNC0(OP_DBL_MUL, 	&intDblMul,		"DBL\034",	"DBL*")
	FN_I2(OP_RCLSIGMA,	&sigma_sum,		"SUM",		CNULL)
	FUNC0(OP_DATEDMY,	XNIL(D_DMY),		"D.MY",		CNULL)
	FUNC0(OP_DATEYMD,	XNIL(D_YMD),		"Y.MD",		CNULL)
	FUNC0(OP_DATEMDY,	XNIL(D_MDY),		"M.DY",		CNULL)
	FUNC0(OP_JG1752,	XNIL(JG1752),		"JG1752",	CNULL)
	FUNC0(OP_JG1582,	XNIL(JG1582),		"JG1582",	CNULL)
	FN_I0(OP_ISLEAP,	&date_isleap,		"LEAP?",	CNULL)
	FN_I0(OP_ALPHADAY,	&date_alphaday,		"\240DAY",	"aDAY")
	FN_I0(OP_ALPHAMONTH,	&date_alphamonth,	"\240MONTH",	"aMONTH")
	FN_I0(OP_ALPHADATE,	&date_alphadate,	"\240DATE",	"aDATE")
	FN_I0(OP_ALPHATIME,	&date_alphatime,	"\240TIME",	"aTIME")
	FN_I1(OP_DATE,		&date_date,		"DATE",		CNULL)
	FN_I1(OP_TIME,		&date_time,		"TIME",		CNULL)
	FUNC0(OP_24HR,		XNIL(HR24),		"24H",		CNULL)
	FUNC0(OP_12HR,		XNIL(HR12),		"12H",		CNULL)
	FN_I0(OP_SETDATE,	&date_setdate,		"SETDAT",	CNULL)
	FN_I0(OP_SETTIME,	&date_settime,		"SETTIM",	CNULL)
	FUNC0(OP_CLRALPHA,	&clralpha,		"CL\240",	"CLa")
	FUNC0(OP_VIEWALPHA,	&alpha_view,		"VIEW\240",	"VIEWa")
	FUNC1(OP_ALPHALEN,	&alpha_length,		"\240LENG",	"aLENG")
	FUNC1(OP_ALPHATOX,	&alpha_tox,		"\240\015x",	"a>x")
	FUNC0(OP_XTOALPHA,	&alpha_fromx,		"x\015\240",	"x>a")
	FUNC0(OP_ALPHAON,	&alpha_onoff,		"\240ON",	"aON")
	FUNC0(OP_ALPHAOFF,	&alpha_onoff,		"\240OFF",	"aOFF")
	FN_I0(OP_REGCOPY,	&op_regcopy,		"R-COPY",	CNULL)
	FN_I0(OP_REGSWAP,	&op_regswap,		"R-SWAP",	CNULL)
	FN_I0(OP_REGCLR,	&op_regclr,		"R-CLR",	CNULL)
	FN_I0(OP_REGSORT,	&op_regsort,		"R-SORT",	CNULL)

	FUNC0(OP_LOADA2D,	&store_a_to_d,		"\015A..D",	CNULL)
	FUNC0(OP_SAVEA2D,	&store_a_to_d,		"A..D\015",	CNULL)

	FUNC0(OP_GSBuser,	&do_usergsb,		"XEQUSR",	CNULL)
	FUNC0(OP_POPUSR,	&op_popusr,		"POPUSR",	CNULL)

	FN_I0(OP_XisInf,	&isInfinite,		"\237?",	"INF?")
	FN_I0(OP_XisNaN,	&isNan,			"NaN?",		CNULL)
	FN_I0(OP_XisSpecial,	&isSpecial,		"SPEC?",	CNULL)
	FUNC0(OP_XisPRIME,	&XisPrime,		"PRIME?",	CNULL)
	FUNC0(OP_XisINT,	&XisInt,		"INT?",		CNULL)
	FUNC0(OP_XisFRAC,	&XisInt,		"FP?",		CNULL)
	FUNC0(OP_XisEVEN,	&XisEvenOrOdd,		"EVEN?",	CNULL)
	FUNC0(OP_XisODD,	&XisEvenOrOdd,		"ODD?",		CNULL)
	FUNC0(OP_ENTRYP,	&op_entryp,		"ENTRY?",	CNULL)

	FUNC1(OP_TICKS,		&op_ticks,		"TICKS",	CNULL)
	FUNC1(OP_VOLTAGE,	&op_voltage,		"BATT",		CNULL)

	FUNC0(OP_QUAD,		XNIL(QUAD),		"SLVQ",		CNULL)
	FUNC0(OP_NEXTPRIME,	XNIL(NEXTPRIME),	"NEXTP",	CNULL)
	FUNC0(OP_SETEUR,	XNIL(SETEUR),		"SETEUR",	CNULL)
	FUNC0(OP_SETUK,		XNIL(SETUK),		"SETUK",	CNULL)
	FUNC0(OP_SETUSA,	XNIL(SETUSA),		"SETUSA",	CNULL)
	FUNC0(OP_SETIND,	XNIL(SETIND),		"SETIND",	CNULL)
	FUNC0(OP_SETCHN,	XNIL(SETCHN),		"SETCHN",	CNULL)
	FUNC0(OP_SETJPN,	XNIL(SETJAP),		"SETJPN",	CNULL)
	FUNC0(OP_WHO,		XNIL(WHO),		"WHO",		CNULL)	

	FUNC0(OP_XEQALPHA,	&op_gtoalpha,		"XEQ\240",	"XEQa")
	FUNC0(OP_GTOALPHA,	&op_gtoalpha,		"GTO\240",	"GTOa")

	FUNC1(OP_ROUNDING,	&op_roundingmode,	"RM?",		CNULL)
	FUNC0(OP_SLOW,		&op_setspeed,		"SLOW",		CNULL)
	FUNC0(OP_FAST,		&op_setspeed,		"FAST",		CNULL)

	FUNC0(OP_TOP,		&isTop,			"TOP?",		CNULL)
	FUNC1(OP_GETBASE,	&get_base,		"BASE?",	CNULL)
	FUNC1(OP_GETSIGN,	&get_sign_mode,		"SMODE?",	CNULL)
	FUNC0(OP_ISINT,		&check_mode,		"INTM?",	CNULL)
	FUNC0(OP_ISFLOAT,	&check_mode,		"REALM?",	CNULL)

	FUNC0(OP_Xeq_pos0,	&check_zero,		"x=+0?",	CNULL)
	FUNC0(OP_Xeq_neg0,	&check_zero,		"x=-0?",	CNULL)

#ifdef MATRIX_ROWOPS
	FN_I0(OP_MAT_ROW_SWAP,	&matrix_rowops,		"MROW\027",	"MROW<>")
	FN_I0(OP_MAT_ROW_MUL,	&matrix_rowops,		"MROW\034",	"MROW*")
	FN_I0(OP_MAT_ROW_GADD,	&matrix_rowops,		"MROW+\034",	"MROW+*")
#endif
	FN_I0(OP_MAT_CHECK_SQUARE, &matrix_is_square,	"M.SQR?",	CNULL)
	FN_I0(OP_MAT_INVERSE,	&matrix_inverse,	"M\235",	"M.INV")
#ifdef SILLY_MATRIX_SUPPORT
	FN_I0(OP_MAT_ZERO,	&matrix_create,		"M.ZERO",	CNULL)
	FN_I0(OP_MAT_IDENT,	&matrix_create,		"M.IDEN",	CNULL)
#endif
	FUNC0(OP_POPLR,		&cmdlpop,		"PopLR",	CNULL)
	FUNC1(OP_MEMQ,		&get_mem,		"MEM?",		CNULL)
	FUNC1(OP_LOCRQ,		&get_mem,		"LocR?",	CNULL)
	FUNC1(OP_REGSQ,		&get_mem,		"REGS?",	CNULL)
	FUNC1(OP_FLASHQ,	&get_mem,		"FLASH?",	CNULL)

#ifdef INCLUDE_USER_IO
	FUNC0(OP_SEND1,		&send_byte,		"SEND1",	CNULL)
	FUNC0(OP_SERIAL_OPEN,	&serial_open,		"SOPEN",	CNULL)
	FUNC0(OP_SERIAL_CLOSE,	&serial_close,		"SCLOSE",	CNULL)
	FUNC0(OP_ALPHASEND,	&send_alpha,		"SEND\240",	"SENDa")
	FUNC0(OP_ALPHARECV,	&recv_alpha,		"RECV\240",	"RECVa")
#endif
	FUNC0(OP_SENDP,		&send_program,		"SENDP",	CNULL)
	FUNC0(OP_SENDR,		&send_registers,	"SENDR",	CNULL)
	FUNC0(OP_SENDsigma,	&send_sigma,		"SEND\221",	"SENDSUMS")
	FUNC0(OP_SENDA,		&send_all,		"SENDA",	CNULL)

	FUNC0(OP_RECV,		&recv_any,		"RECV",		CNULL)
	FUNC0(OP_SAVE,		&flash_backup,		"SAVE",		CNULL)
	FUNC0(OP_LOAD,		&flash_restore,		"LOAD",		CNULL)
	FUNC0(OP_LOADST,	&load_state,		"LOADSS",	CNULL)
	FUNC0(OP_LOADP,		&load_program,		"LOADP",	CNULL)
	FUNC0(OP_PRCL,		&recall_program,	"PRCL",		CNULL)
	FUNC0(OP_PSTO,		&store_program,		"PSTO",		CNULL)

	FUNC0(OP_LOADR,		&load_registers,	"LOADR",	CNULL)
	FUNC0(OP_LOADsigma,	&load_sigma,		"LOAD\221",	"LOADSUMS")

	FUNC0(OP_DBLON,		&op_double,		"DBLON",	CNULL)
	FUNC0(OP_DBLOFF,	&op_double,		"DBLOFF",	CNULL)
	FUNC0(OP_ISDBL,		&check_dblmode,		"DBL?",		CNULL)

	FN_I2(OP_cmplxI,	XNIL(CPX_I),		"\024i",	"ci")

	FUNC0(OP_DATE_TO,	XNIL(DATE_TO),		"DATE\015",	"DATE>")

	FUNC0(OP_DOTPROD,	XNIL(cpx_DOT),		"\024DOT",	"cDOT")
	FUNC0(OP_CROSSPROD,	XNIL(cpx_CROSS),	"\024CROSS",	"cCROSS")

	/* INFRARED commands */
	FUNC0(OP_PRINT_PGM,	IRN(print_program),	"\222PROG",	"P.PROG")
	FUNC0(OP_PRINT_REGS,	IRN(print_registers),	"\222REGS",	"P.REGS")
	FUNC0(OP_PRINT_STACK,	IRN(print_registers),	"\222STK",	"P.STK")
	FUNC0(OP_PRINT_SIGMA,	IRN(print_sigma),	"\222\221",	"P.SUMS")
	FUNC0(OP_PRINT_ALPHA,	IRN(print_alpha),	"\222\240",	"P.a")
	FUNC0(OP_PRINT_ALPHA_NOADV, IRN(print_alpha),	"\222\240+",	"P.a+")
	FUNC0(OP_PRINT_ALPHA_JUST,  IRN(print_alpha),	"\222+\240",	"P.+a")
	FUNC0(OP_PRINT_ADV,	IRN(print_lf),		"\222ADV",	"P.ADV")
	FUNC1(OP_PRINT_WIDTH,	IRN(cmdprintwidth),	"\222WIDTH",	"P.WIDTH")
	/* end of INFRARED commands */

	FUNC0(OP_QUERY_XTAL,	&op_query_xtal,		"XTAL?",	CNULL)
	FUNC0(OP_QUERY_PRINT,	&op_query_print,	"\222?",	"PRT?")

	FUNC0(OP_SHOWY,		XNIL(SHOW_Y_REG),	"YDON",		CNULL)
	FUNC0(OP_HIDEY,		XNIL(HIDE_Y_REG),	"YDOFF",	CNULL)

#ifdef INCLUDE_STOPWATCH
	FUNC0(OP_STOPWATCH,	&stopwatch,		"STOPW",	CNULL)
#endif
#ifdef _DEBUG
	FUNC0(OP_DEBUG,		XNIL(DBG),		"DBG",		CNULL)
#endif

#undef FUNC
#undef FUNC0
#undef FUNC1
#undef FUNC2
#undef FN_I0
#undef FN_I1
#undef FN_I2
};


#ifdef COMPILE_CATALOGUES
#define allCMD(name, func, limit, nm, alias, ind, reg, stk, loc, cpx, lbl, flag, plot)	\
	{ #func, (limit) - 1,                ind, reg, stk, loc, cpx, lbl, flag, plot, nm, alias },
#elif DEBUG
#define allCMD(name, func, limit, nm, alias, ind, reg, stk, loc, cpx, lbl, flag, plot)	\
	{ name, func, (limit) - 1,           ind, reg, stk, loc, cpx, lbl, flag, plot, nm, alias },
#elif COMMANDS_PASS == 1
#define allCMD(name, func, limit, nm, alias, ind, reg, stk, loc, cpx, lbl, flag, plot)	\
	{ 0xaa55, (limit) - 1,               ind, reg, stk, loc, cpx, lbl, flag, plot, nm },
#elif defined(REALBUILD)
#define allCMD(name, func, limit, nm, alias, ind, reg, stk, loc, cpx, lbl, flag, plot)	\
	{ func, (limit) - 1,                 ind, reg, stk, loc, cpx, lbl, flag, plot, nm },
#else
#define allCMD(name, func, limit, nm, alias, ind, reg, stk, loc, cpx, lbl, flag, plot)	\
	{ func, (limit) - 1,                 ind, reg, stk, loc, cpx, lbl, flag, plot, nm, alias },
#endif
                                                                                      // in rg st lo cx lb fl  pl
#define CMD(n, f, lim, nm, alias)	allCMD(n, f, lim,                      nm, alias, 1, 0, 0, 0, 0, 0, 0, 0)
#define CMDnoI(n, f, lim, nm, alias)	allCMD(n, f, lim,                      nm, alias, 0, 0, 0, 0, 0, 0, 0, 0)
#define CMDreg(n, f, nm, alias)		allCMD(n, f, TOPREALREG,               nm, alias, 1, 1, 0, 1, 0, 0, 0, 0)
#define CMDstk(n, f, nm, alias)		allCMD(n, f, NUMREG+MAX_LOCAL,         nm, alias, 1, 1, 1, 1, 0, 0, 0, 0)
#define CMDcstk(n, f, nm, alias)	allCMD(n, f, NUMREG+MAX_LOCAL-1,       nm, alias, 1, 1, 1, 1, 1, 0, 0, 0)
#define CMDregnL(n, f, nm, alias)	allCMD(n, f, TOPREALREG,               nm, alias, 1, 1, 0, 0, 0, 0, 0, 0)
#define CMDstknL(n, f, nm, alias)	allCMD(n, f, NUMREG,                   nm, alias, 1, 0, 1, 0, 0, 0, 0, 0)
#define CMDcstknL(n, f, nm, alias)	allCMD(n, f, NUMREG-1,                 nm, alias, 1, 0, 1, 0, 1, 0, 0, 0)
#define CMDlbl(n, f, nm, alias)		allCMD(n, f, NUMLBL,                   nm, alias, 1, 0, 0, 0, 0, 1, 0, 0)
#define CMDlblnI(n, f, nm, alias)	allCMD(n, f, NUMLBL,                   nm, alias, 0, 0, 0, 0, 0, 1, 0, 0)
#define CMDflg(n, f, nm, alias)		allCMD(n, f, NUMFLG+16,		       nm, alias, 1, 0, 1, 1, 0, 0, 1, 0)
#define CMDplt(n, f, nm, alias)		allCMD(n, f, NUMREG+MAX_LOCAL,	       nm, alias, 1, 1, 0, 1, 0, 0, 0, 1)

#if COMMANDS_PASS == 2
CMDTAB const struct argcmd_cmdtab argcmds_ct[ NUM_RARG ] = {
#else
const struct argcmd argcmds[ NUM_RARG ] = {
#endif
	CMDnoI(RARG_CONST,	&cmdconst,	NUM_CONSTS,		"#",		CNULL)
	CMDnoI(RARG_CONST_CMPLX,&cmdconst,	NUM_CONSTS,		"\024#",	"c#")
	CMD(RARG_ERROR,		&cmderr,	MAX_ERROR,		"ERR",		CNULL)
	CMDstk(RARG_STO, 	&cmdsto,				"STO",		CNULL)
	CMDstk(RARG_STO_PL, 	&cmdsto,				"STO+",		CNULL)
	CMDstk(RARG_STO_MI, 	&cmdsto,				"STO-",		CNULL)
	CMDstk(RARG_STO_MU, 	&cmdsto,				"STO\034",	"STO*")
	CMDstk(RARG_STO_DV, 	&cmdsto,				"STO/",		CNULL)
	CMDstk(RARG_STO_MIN,	&cmdsto,				"STO\017",	"STOMIN")
	CMDstk(RARG_STO_MAX,	&cmdsto,				"STO\020",	"STOMAX")
	CMDstk(RARG_RCL, 	&cmdrcl,				"RCL",		CNULL)
	CMDstk(RARG_RCL_PL, 	&cmdrcl,				"RCL+",		CNULL)
	CMDstk(RARG_RCL_MI, 	&cmdrcl,				"RCL-",		CNULL)
	CMDstk(RARG_RCL_MU, 	&cmdrcl,				"RCL\034",	"RCL*")
	CMDstk(RARG_RCL_DV, 	&cmdrcl,				"RCL/",		CNULL)
	CMDstk(RARG_RCL_MIN,	&cmdrcl,				"RCL\017",	"RCLMIN")
	CMDstk(RARG_RCL_MAX,	&cmdrcl,				"RCL\020",	"RCLMAX")
	CMDstk(RARG_SWAPX,	&cmdswap,				"x\027",	"x<>")
	CMDstk(RARG_SWAPY,	&cmdswap,				"y\027",	"y<>")
	CMDstk(RARG_SWAPZ,	&cmdswap,				"z\027",	"z<>")
	CMDstk(RARG_SWAPT,	&cmdswap,				"t\027",	"t<>")
	CMDcstk(RARG_CSTO, 	&cmdcsto,				"\024STO",	"cSTO")
	CMDcstk(RARG_CSTO_PL, 	&cmdcsto,				"\024STO+",	"cSTO+")
	CMDcstk(RARG_CSTO_MI, 	&cmdcsto,				"\024STO-",	"cSTO-")
	CMDcstk(RARG_CSTO_MU, 	&cmdcsto,				"\024STO\034",	"cSTO*")
	CMDcstk(RARG_CSTO_DV, 	&cmdcsto,				"\024STO/",	"cSTO/")
	CMDcstk(RARG_CRCL, 	&cmdcrcl,				"\024RCL",	"cRCL")
	CMDcstk(RARG_CRCL_PL, 	&cmdcrcl,				"\024RCL+",	"cRCL+")
	CMDcstk(RARG_CRCL_MI, 	&cmdcrcl,				"\024RCL-",	"cRCL-")
	CMDcstk(RARG_CRCL_MU, 	&cmdcrcl,				"\024RCL\034",	"cRCL*")
	CMDcstk(RARG_CRCL_DV, 	&cmdcrcl,				"\024RCL/",	"cRCL/")
	CMDcstk(RARG_CSWAPX,	&cmdswap,				"\024x\027",	"cx<>")
	CMDcstk(RARG_CSWAPZ,	&cmdswap,				"\024z\027",	"cz<>")
	CMDstk(RARG_VIEW,	&cmdview,				"VIEW",		CNULL)
	CMDstk(RARG_STOSTK,	&cmdstostk,				"STOS",		CNULL)
	CMDstk(RARG_RCLSTK,	&cmdrclstk,				"RCLS",		CNULL)
	CMDnoI(RARG_ALPHA,	&cmdalpha,	256,			"",		"a")
	CMDstk(RARG_AREG,	&alpha_reg,				"\240RC#",	"aRC#")
	CMDstk(RARG_ASTO,	&alpha_sto,				"\240STO",	"aSTO")
	CMDstk(RARG_ARCL,	&alpha_rcl,				"\240RCL",	"aRCL")
	CMDstk(RARG_AIP,	&alpha_ip,				"\240IP",	"aIP")
	CMD(RARG_ALRL,		&alpha_shift_l,	NUMALPHA,		"\240RL",	"aRL")
	CMD(RARG_ALRR,		&alpha_rot_r,	NUMALPHA,		"\240RR",	"aRR")
	CMD(RARG_ALSL,		&alpha_shift_l,	NUMALPHA+1,		"\240SL",	"aSL")
	CMD(RARG_ALSR,		&alpha_shift_r,	NUMALPHA+1,		"\240SR",	"aSR")
	CMDstk(RARG_TEST_EQ,	&cmdtest,				"x=?",		CNULL)
	CMDstk(RARG_TEST_NE,	&cmdtest,				"x\013?",	"x!=?")
	CMDstk(RARG_TEST_APX,	&cmdtest,				"x\035?",	"x~?")
	CMDstk(RARG_TEST_LT,	&cmdtest,				"x<?",		CNULL)
	CMDstk(RARG_TEST_LE,	&cmdtest,				"x\011?",	"x<=?")
	CMDstk(RARG_TEST_GT,	&cmdtest,				"x>?",		CNULL)
	CMDstk(RARG_TEST_GE,	&cmdtest,				"x\012?",	"x>=?")
	CMDcstk(RARG_TEST_ZEQ,	&cmdztest,				"\024x=?",	"cx=?")
	CMDcstk(RARG_TEST_ZNE,	&cmdztest,				"\024x\013?",	"cx!=?")
	CMDnoI(RARG_SKIP,	&cmdskip,	256,			"SKIP",		CNULL)
	CMDnoI(RARG_BACK,	&cmdback,	256,			"BACK",		CNULL)
	CMDnoI(RARG_BSF,	&cmdskip,	256,			"BSRF",		CNULL)
	CMDnoI(RARG_BSB,	&cmdback,	256,			"BSRB",		CNULL)

	CMDstk(RARG_DSE,	&cmdloop,				"DSE",		CNULL)
	CMDstk(RARG_ISG,	&cmdloop,				"ISG",		CNULL)
	CMDstk(RARG_DSL,	&cmdloop,				"DSL",		CNULL)
	CMDstk(RARG_ISE,	&cmdloop,				"ISE",		CNULL)
	CMDstk(RARG_DSZ,	&cmdloopz,				"DSZ",		CNULL)
	CMDstk(RARG_ISZ,	&cmdloopz,				"ISZ",		CNULL)
	CMDstk(RARG_DEC,	&cmdlincdec,				"DEC",		CNULL)
	CMDstk(RARG_INC,	&cmdlincdec,				"INC",		CNULL)

	CMDlblnI(RARG_LBL,	NOFN,					"LBL",		CNULL)
	CMDlbl(RARG_LBLP,	&cmdlblp,				"LBL?",		CNULL)
	CMDlbl(RARG_XEQ,	&cmdgto,				"XEQ",		CNULL)
	CMDlbl(RARG_GTO,	&cmdgto,				"GTO",		CNULL)

	CMDlbl(RARG_SUM,	XARG(SIGMA),				"\221",		"SUM")
	CMDlbl(RARG_PROD,	XARG(PRODUCT),				"\217",		"PROD")
	CMDlbl(RARG_SOLVE,	XARG(SOLVE),				"SLV",		CNULL)
	CMDlbl(RARG_DERIV,	XARG(DERIV),				"f'(x)",	CNULL)
	CMDlbl(RARG_2DERIV,	XARG(2DERIV),				"f\"(x)",	CNULL)
	CMDlbl(RARG_INTG,	XARG(INTEGRATE),			"\004",		"INTG")

	CMD(RARG_STD,		&cmddisp,	DISPLAY_DIGITS,		"ALL",		CNULL)
	CMD(RARG_FIX,		&cmddisp,	DISPLAY_DIGITS,		"FIX",		CNULL)
	CMD(RARG_SCI,		&cmddisp,	DISPLAY_DIGITS,		"SCI",		CNULL)
	CMD(RARG_ENG,		&cmddisp,	DISPLAY_DIGITS,		"ENG",		CNULL)
	CMD(RARG_DISP,		&cmddisp,	DISPLAY_DIGITS,		"DISP",		CNULL)

	CMDflg(RARG_SF,		&cmdflag,				"SF",		CNULL)
	CMDflg(RARG_CF,		&cmdflag,				"CF",		CNULL)
	CMDflg(RARG_FF,		&cmdflag,				"FF",		CNULL)
	CMDflg(RARG_FS,		&cmdflag,				"FS?",		CNULL)
	CMDflg(RARG_FC,		&cmdflag,				"FC?",		CNULL)
	CMDflg(RARG_FSC,	&cmdflag,				"FS?C",		CNULL)
	CMDflg(RARG_FSS,	&cmdflag,				"FS?S",		CNULL)
	CMDflg(RARG_FSF,	&cmdflag,				"FS?F",		CNULL)
	CMDflg(RARG_FCC,	&cmdflag,				"FC?C",		CNULL)
	CMDflg(RARG_FCS,	&cmdflag,				"FC?S",		CNULL)
	CMDflg(RARG_FCF,	&cmdflag,				"FC?F",		CNULL)
	CMD(RARG_WS,		&intws,		MAX_WORD_SIZE+1,	"WSIZE",	CNULL)
	CMD(RARG_RL,		&introt,	MAX_WORD_SIZE,		"RL",		CNULL)
	CMD(RARG_RR,		&introt,	MAX_WORD_SIZE,		"RR",		CNULL)
	CMD(RARG_RLC,		&introt,	MAX_WORD_SIZE+1,	"RLC",		CNULL)
	CMD(RARG_RRC,		&introt,	MAX_WORD_SIZE+1,	"RRC",		CNULL)
	CMD(RARG_SL,		&introt,	MAX_WORD_SIZE+1,	"SL",		CNULL)
	CMD(RARG_SR,		&introt,	MAX_WORD_SIZE+1,	"SR",		CNULL)
	CMD(RARG_ASR,		&introt,	MAX_WORD_SIZE+1,	"ASR",		CNULL)
	CMD(RARG_SB,		&intbits,	MAX_WORD_SIZE,		"SB",		CNULL)
	CMD(RARG_CB,		&intbits,	MAX_WORD_SIZE,		"CB",		CNULL)
	CMD(RARG_FB,		&intbits,	MAX_WORD_SIZE,		"FB",		CNULL)
	CMD(RARG_BS,		&intbits,	MAX_WORD_SIZE,		"BS?",		CNULL)
	CMD(RARG_BC,		&intbits,	MAX_WORD_SIZE,		"BC?",		CNULL)
	CMD(RARG_MASKL,		&intmsks,	MAX_WORD_SIZE+1,	"MASKL",	CNULL)
	CMD(RARG_MASKR,		&intmsks,	MAX_WORD_SIZE+1,	"MASKR",	CNULL)
	CMD(RARG_BASE,		&set_int_base,	17,			"BASE",		CNULL)

	CMDnoI(RARG_CONV,	&cmdconv,	NUM_CONSTS_CONV*2,	"conv",		CNULL)

	CMD(RARG_PAUSE,		&cmdpause,	100,			"PSE",		CNULL)
	CMDstk(RARG_KEY,	&cmdkeyp,				"KEY?",		CNULL)

	CMDstk(RARG_ALPHAXEQ,	&cmdalphagto,				"\240XEQ",	"aXEQ")
	CMDstk(RARG_ALPHAGTO,	&cmdalphagto,				"\240GTO",	"aGTO")

#ifdef INCLUDE_FLASH_RECALL
	CMDstknL(RARG_FLRCL, 	  &cmdflashrcl,				"RCF",		CNULL)
	CMDstknL(RARG_FLRCL_PL,   &cmdflashrcl,				"RCF+",		CNULL)
	CMDstknL(RARG_FLRCL_MI,   &cmdflashrcl,				"RCF-",		CNULL)
	CMDstknL(RARG_FLRCL_MU,   &cmdflashrcl,				"RCF\034",	"RCF*")
	CMDstknL(RARG_FLRCL_DV,   &cmdflashrcl,				"RCF/",		CNULL)
	CMDstknL(RARG_FLRCL_MIN,  &cmdflashrcl,				"RCF\017",	"RCFMIN")
	CMDstknL(RARG_FLRCL_MAX,  &cmdflashrcl,				"RCF\020",	"RCFMAX")
	CMDcstknL(RARG_FLCRCL, 	  &cmdflashcrcl,			"\024RCF",	"cRCF")
	CMDcstknL(RARG_FLCRCL_PL, &cmdflashcrcl,			"\024RCF+",	"cRCF+")
	CMDcstknL(RARG_FLCRCL_MI, &cmdflashcrcl,			"\024RCF-",	"cRCF-")
	CMDcstknL(RARG_FLCRCL_MU, &cmdflashcrcl,			"\024RCF\034",	"cRCF*")
	CMDcstknL(RARG_FLCRCL_DV, &cmdflashcrcl,			"\024RCF/",	"cRCF/")
#endif
	CMD(RARG_SLD,		&op_shift_digit, 256,			"SDL",		CNULL)
	CMD(RARG_SRD,		&op_shift_digit, 256,			"SDR",		CNULL)

	CMDstk(RARG_VIEW_REG,	&alpha_view_reg,			"VW\240+",	"VWa+")
	CMD(RARG_ROUNDING,	&rarg_roundingmode, DEC_ROUND_MAX,	"RM",		CNULL)
	CMD(RARG_ROUND,		&rarg_round,	35,			"RSD",		CNULL)
	CMD(RARG_ROUND_DEC,	&rarg_round,	100,			"RDP",		CNULL)

#ifdef INCLUDE_USER_MODE
	CMDstk(RARG_STOM,	&cmdsavem,				"STOM",		CNULL)
	CMDstk(RARG_RCLM,	&cmdrestm,				"RCLM",		CNULL)
#endif
	CMDstk(RARG_PUTKEY,	&cmdputkey,				"PUTK",		CNULL)
	CMDstk(RARG_KEYTYPE,	&cmdkeytype,				"KTP?",		CNULL)

	CMD(RARG_MESSAGE,	&cmdmsg,	MAX_MESSAGE,		"MSG",		CNULL)
	CMD(RARG_LOCR,		&cmdlocr,	MAX_LOCAL + 1,		"LocR",		CNULL)
	CMD(RARG_REGS,		&cmdregs,	TOPREALREG + 1,		"REGS",		CNULL)

	CMDstk(RARG_iRCL,	&cmdircl,				"iRCL",		CNULL)
	CMDstk(RARG_sRCL,	&cmdrrcl,				"sRCL",		CNULL)
	CMDstk(RARG_dRCL,	&cmdrrcl,				"dRCL",		CNULL)
	CMD(RARG_MODE_SET,	&cmdmode,	64,			"xMSET",	CNULL)
	CMD(RARG_MODE_CLEAR,	&cmdmode,	64,			"xMCLR",	CNULL)

	CMDnoI(RARG_XROM_IN,	&cmdxin,	256,			"xIN",		CNULL)
	CMDnoI(RARG_XROM_OUT,	&cmdxout,	256,			"xOUT",		CNULL)
#ifdef XROM_RARG_COMMANDS
	CMDstk(RARG_XROM_ARG,	&cmdxarg,				"xARG",		CNULL)
#endif
	CMDnoI(RARG_CONVERGED,	&cmdconverged,	32,			"CNVG?",	CNULL)
	CMDnoI(RARG_SHUFFLE,	&cmdshuffle,	256,			"\027",		"<>")

	CMDnoI(RARG_INTNUM,	  &cmdconst,	256,			"#",		CNULL)
	CMDnoI(RARG_INTNUM_CMPLX, &cmdconst,	256,			"\024#",	"c#")
#ifdef INCLUDE_INDIRECT_CONSTS
	CMD(RARG_IND_CONST,	  &cmdconst,	NUM_CONSTS,		"CNST",		CNULL)
	CMD(RARG_IND_CONST_CMPLX, &cmdconst,	NUM_CONSTS,		"\024CNST",	"cCNST")
#endif
	/* INFRARED commands */
	CMDstk(RARG_PRINT_REG,	IRA(cmdprintreg),			"\222r",	"P.r")
	CMD(RARG_PRINT_BYTE,	IRA(cmdprint),	256,			"\222#",	"P.#")
	CMD(RARG_PRINT_CHAR,	IRA(cmdprint),	256,			"\222CHR",	"P.CHR")
	CMD(RARG_PRINT_TAB,	IRA(cmdprint),	166,			"\222TAB",	"P.TAB")
	CMD(RARG_PMODE,		IRA(cmdprintmode),  4,			"\222MODE",	"P.MODE")
	CMD(RARG_PDELAY,	IRA(cmdprintmode),  32,			"\222DLAY",	"P.DLAY")
	CMDcstk(RARG_PRINT_CMPLX, IRA(cmdprintcmplxreg),		"\222\024r\213\214",	"P.crect")
	/* end of INFRARED commands */
#ifdef INCLUDE_PLOTTING
	CMDplt(RARG_PLOT_INIT,    &cmdplotinit,				"gDIM",		CNULL)
	CMDplt(RARG_PLOT_DIM,     &cmdplotdim,				"gDIM?",	CNULL)
	CMDplt(RARG_PLOT_SETPIX,  &cmdplotpixel,			"gSET",		CNULL)
	CMDplt(RARG_PLOT_CLRPIX,  &cmdplotpixel,			"gCLR",		CNULL)
	CMDplt(RARG_PLOT_FLIPPIX, &cmdplotpixel,			"gFLP",		CNULL)
	CMDplt(RARG_PLOT_ISSET,   &cmdplotpixel,			"gPIX?",	CNULL)
	CMDplt(RARG_PLOT_DISPLAY, &cmdplotdisplay,			"gPLOT",	CNULL)
	CMDplt(RARG_PLOT_PRINT,   IRA(cmdplotprint),			"\222PLOT",	"P.PLOT")	/* INFRARED command */
#endif

	// Only the first of this group is used in XROM
	CMDstk(RARG_CASE,	&cmdskip,				"CASE",		CNULL)
#ifdef INCLUDE_INDIRECT_BRANCHES
	CMD(RARG_iBACK,		&cmdback,				"iBACK",	CNULL)
	CMD(RARG_iBSF,		&cmdskip,				"iBSRF",	CNULL)
	CMD(RARG_iBSB,		&cmdback,				"iBSRB",	CNULL)
#endif
	CMDcstk(RARG_CVIEW,	&cmdview,				"\024VIEW",	"cVIEW")

#undef CMDlbl
#undef CMDlblnI
#undef CMDnoI
#undef CMDstk
#undef CMD
#undef allCMD
};


#ifdef COMPILE_CATALOGUES
#define CMD(name, func, nm, alias)			\
	{ #func, nm, alias },
#elif DEBUG
#define CMD(name, func, nm, alias)			\
	{ name, func, nm, alias },
#elif COMMANDS_PASS == 1
#define CMD(name, func, nm, alias)			\
	{ 0xaa55, nm },
#elif defined(REALBUILD)
#define CMD(name, func, nm, alias)			\
	{ func, nm },
#else
#define CMD(name, func, nm, alias)			\
	{ func, nm, alias },
#endif

#if COMMANDS_PASS == 2
CMDTAB const struct multicmd_cmdtab multicmds_ct[ NUM_MULTI ] = {
#else
const struct multicmd multicmds[ NUM_MULTI ] = {
#endif
	CMD(DBL_LBL,	NOFN,				"LBL",		CNULL)
	CMD(DBL_LBLP,	&cmdmultilblp,			"LBL?",		CNULL)
	CMD(DBL_XEQ,	&cmdmultigto,			"XEQ",		CNULL)
	CMD(DBL_GTO,	&cmdmultigto,			"GTO",		CNULL)
	CMD(DBL_SUM,	XMULTI(SIGMA),			"\221",		"SUM")
	CMD(DBL_PROD,	XMULTI(PRODUCT),		"\217",		"PROD")
	CMD(DBL_SOLVE,	XMULTI(SOLVE),			"SLV",		CNULL)
	CMD(DBL_DERIV,	XMULTI(DERIV),			"f'(x)",	CNULL)
	CMD(DBL_2DERIV,	XMULTI(2DERIV),			"f\"(x)",	CNULL)
	CMD(DBL_INTG,	XMULTI(INTEGRATE),		"\004",		"INTG")
	CMD(DBL_ALPHA,	&multialpha,			"\240",		"a")
#undef CMD
};


/*
 *  We need to include the same file a second time with updated #defines.
 *  This will create the structures in the CMDTAB segment with all pointers filled in.
 */
#if COMMANDS_PASS == 1
#undef COMMANDS_PASS
#define COMMANDS_PASS 2
#include "commands.c"
#undef COMMANDS_PASS
#endif

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
 *
 */

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0000, "(null)",              0, 0x0000,     0x00, 0x00,    CNULL,
		{
			     _ ,      _ ,
			     _ ,      _ ,
			     _ ,      _ ,
			     _ ,      _ ,
			     _ ,      _ ,
			     _ ,      _ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0001, "x-bar",            7500, 0x1e80,     0x00, 0x00,    CNULL,
		{
			  OOO_ ,   OOO_ ,
			  ____ ,   ____ ,
			  O_O_ ,   O_O_ ,
			  _O__ ,   _O__ ,
			  _O__ ,   O_O_ ,
			  O_O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0002, "y-bar",            7900, 0x0233,     0x00, 0x00,    CNULL,
		{
			 OOOO_ ,   OOO_ ,
			 _____ ,   ____ ,
			 O__O_ ,   O_O_ ,
			 _O_O_ ,   OOO_ ,
			 __O__ ,   __O_ ,
			 OO___ ,   OO__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0003, "sqrt",            17100, 0x221a,     0x00, 0x83,    CNULL,
		{
			__OOO_ ,  _OOO_ ,
			__O___ ,  _O___ ,
			__O___ ,  _O___ ,
			O_O___ ,  OO___ ,
			_OO___ ,  _O___ ,
			__O___ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0004, "integral",        17200, 0x222b,     0x00, 0x84,    CNULL,
		{
			___O__ ,   _OO_ ,
			__O_O_ ,   _O__ ,
			__O___ ,   _O__ ,
			__O___ ,   _O__ ,
			O_O___ ,   _O__ ,
			_O____ ,   OO__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0005, "degree",           1000, 0x00b0,     0xb0, 0xB3,    CNULL,
		{
			  OOO_ ,   OOO_ ,
			  O_O_ ,   O_O_ ,
			  OOO_ ,   OOO_ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0006, "narrow-space",        0, 0x00a0,     0xa0, 0x20,    CNULL,
		{
			     _ ,      _ ,
			     _ ,      _ ,
			     _ ,      _ ,
			     _ ,      _ ,
			     _ ,      _ ,
			     _ ,      _ ,
		}
	},

/* The grad symbol is also used as an escape character, see set_status_sized() in display.c */
/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0007, "grad",            18000, 0x24bc,     0x00, 0x00,    CNULL,
		{
			_OOO__ ,  _OOO_ ,
			O_____ ,  O____ ,
			O_OO__ ,  O_OO_ ,
			O__O__ ,  O__O_ ,
			_OOO__ ,  _OOO_ ,
			______ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0010, "+/-",             12700, 0x00b1,     0xb1, 0xFE,    "plus-minus",
		{
			______ ,   _O__ ,
			__O___ ,   OOO_ ,
			OOOOO_ ,   _O__ ,
			__O___ ,   ____ ,
			______ ,   OOO_ ,
			OOOOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0011, "<=",              15600, 0x2264,     0xab, 0x89,    "le",
		{
			 ___O_ ,   __O_ ,
			 __O__ ,   _O__ ,
			 _O___ ,   OOO_ ,
			 OOOO_ ,   ____ ,
			 _____ ,   OOO_ ,
			 OOOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0012, ">=",              16000, 0x2265,     0xbb, 0x8A,    "ge",
		{
			 O____ ,   O___ ,
			 _O___ ,   _O__ ,
			 __O__ ,   OOO_ ,
			 OOOO_ ,   ____ ,
			 _____ ,   OOO_ ,
			 OOOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0013, "!=",              15900, 0x2260,     0x87, 0x8B,    "ne",
		{
			______ ,   _O__ ,
			___O__ ,   OOO_ ,
			OOOOO_ ,   _O__ ,
			__O___ ,   OOO_ ,
			OOOOO_ ,   _O__ ,
			_O____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0014, "euro",            16400, 0x20ac,     0x80, 0x00,    CNULL,
		{
			__OOO_ ,  _OOO_ ,
			_O____ ,  O____ ,
			_OOO__ ,  OOO__ ,
			OOO___ ,  O____ ,
			_O____ ,  _OOO_ ,
			__OOO_ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0015, "->",              14600, 0x2192,     0x00, 0x8D,    "right",
		{
			______ ,  _O___ ,
			__O___ ,  __O__ ,
			___O__ ,  OOOO_ ,
			OOOOO_ ,  __O__ ,
			___O__ ,  _O___ ,
			__O___ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0016, "<-",              14700, 0x2190,     0x00, 0x8E,    "left",
		{
			______ ,  __O__ ,
			__O___ ,  _O___ ,
			_O____ ,  OOOO_ ,
			OOOOO_ ,  _O___ ,
			_O____ ,  __O__ ,
			__O___ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0017, "v",               14900, 0x2193,     0x00, 0x00,    "down",
		{
			______ ,   _O__ ,
			__O___ ,   _O__ ,
			__O___ ,   _O__ ,
			O_O_O_ ,   OOO_ ,
			_OOO__ ,   _O__ ,
			__O___ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0020, "^",               14800, 0x2191,     0x00, 0x00,    "up",
		{
			______ ,   _O__ ,
			__O___ ,   OOO_ ,
			_OOO__ ,   _O__ ,
			O_O_O_ ,   _O__ ,
			__O___ ,   _O__ ,
			__O___ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0021, "f-shift",           100, 0x24d5,     0x00, 0x00,    CNULL,
		{
			 __OO_ ,   _OO_ ,	/* do NOT change the width */
			 __O__ ,   _O__ ,	/* of this character	   */
			 _OOO_ ,   OOO_ ,
			 __O__ ,   _O__ ,
			 __O__ ,   ____ ,
			 _____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0022, "g-shift",           200, 0x24d6,     0x00, 0x00,    CNULL,
		{
			 __OO_ ,   _OO_ ,	/* do NOT change the width */
			 _O_O_ ,   OOO_ ,	/* of this character	   */
			 _OOO_ ,   __O_ ,
			 ___O_ ,   OO__ ,
			 _OO__ ,   ____ ,
			 _____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0023, "h-shift",           300, 0x24d7,     0x00, 0x00,    CNULL,
		{
			 _O___ ,   O___ ,	/* do NOT change the width */
			 _O___ ,   OOO_ ,	/* of this character	   */
			 _OOO_ ,   O_O_ ,
			 _O_O_ ,   ____ ,
			 _____ ,   ____ ,
			 _____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0024, "cmplx",            2700, 0x00a9,     0xa9, 0x00,    CNULL,
		{
			   OO_ ,    OO_ ,
			   O__ ,    O__ ,
			   OO_ ,    OO_ ,
			   ___ ,    ___ ,
			   ___ ,    ___ ,
			   ___ ,    ___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0025, "O-slash",          5800, 0x00d8,     0xd8, 0xD2,    CNULL,
		{
			_OOOO_ , _OOOO_ ,
			O__OO_ , O__OO_ ,
			O_O_O_ , O_O_O_ ,
			O_O_O_ , OO__O_ ,
			OO__O_ , OOOO__ ,
			OOOO__ , ______ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0026, "o-slash",          5800, 0x00f8,     0xf8, 0xD6,    CNULL,
		{
			 _____ ,  _____ ,
			 _____ ,  _OOO_ ,
			 _OOO_ ,  O_OO_ ,
			 O_OO_ ,  OO_O_ ,
			 OO_O_ ,  OOO__ ,
			 OOO__ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0027, "<->",             15200, 0x21c6,     0x00, 0x00,    "exchange",
		{
			_O____ , _O____ ,
			OOOOO_ , OOOOO_ ,
			_O____ , _O____ ,
			___O__ , ___O__ ,
			OOOOO_ , OOOOO_ ,
			___O__ , ___O__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0030, "sz",               6400, 0x00df,     0xdf, 0xDE,    CNULL,
		{
			 _OO__ ,   OO__ ,
			 O__O_ ,   O_O_ ,
			 OOO__ ,   OO__ ,
			 O__O_ ,   O_O_ ,
			 OOOO_ ,   OOO_ ,
			 O____ ,   O___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0031, "x-hat",            7600, 0x1e8b,     0x00, 0x00,    CNULL,
		{
			  _O__ ,   _O__ ,
			  O_O_ ,   O_O_ ,
			  ____ ,   ____ ,
			  O_O_ ,   O_O_ ,
			  _O__ ,   _O__ ,
			  O_O_ ,   O_O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0032, "y-hat",            8100, 0x0177,     0x00, 0x00,    CNULL,
		{
			  _O__ ,   _O__ ,
			  O_O_ ,   O_O_ ,
			  ____ ,   ____ ,
			  O_O_ ,   O_O_ ,
			  _O__ ,   _O__ ,
			  _O__ ,   _O__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0033, "sub-m",            5000, 0x2098,     0x00, 0x00,    CNULL,
		{
			______ , ______ ,
			______ , ______ ,
			______ , ______ ,
			______ , OOOO__ ,
			OOOO__ , O_O_O_ ,
			O_O_O_ , O_O_O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0034, "times",           12500, 0x00d7,     0xd7, 0x82,    CNULL,
		{
			  ____ ,   ____ ,
			  ____ ,   O_O_ ,
			  O_O_ ,   _O__ ,
			  _O__ ,   O_O_ ,
			  O_O_ ,   ____ ,
			  ____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0035, "approx",          15800, 0x2248,     0x00, 0x00,    CNULL,
		{
			 _____ ,  _O_O_ ,
			 _O_O_ ,  O_O__ ,
			 O_O__ ,  _____ ,
			 _____ ,  _O_O_ ,
			 _O_O_ ,  O_O__ ,
			 O_O__ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0036, "pound",           16500, 0x00a3,     0xa3, 0xBB,    CNULL,
		{
			 __O__ ,  _OO__ ,
			 _O_O_ ,  _O___ ,
			 _O___ ,  OOO__ ,
			 OOO__ ,  _O___ ,
			 _O___ ,  OOOO_ ,
			 OOOO_ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0037, "yen",             16600, 0x00a5,     0xa5, 0xBC,    CNULL,
		{
			O___O_ , O___O_ ,
			_O_O__ , _OOO__ ,
			_OOO__ , __O___ ,
			__O___ , _OOO__ ,
			_OOO__ , __O___ ,
			__O___ , ______ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0040, "space",               0, 0x0020,     0x20, 0x20,    CNULL,
		{
			 _____ ,     __ ,
			 _____ ,     __ ,
			 _____ ,     __ ,
			 _____ ,     __ ,
			 _____ ,     __ ,
			 _____ ,     __ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0041, "!",               13000, 0x0021,     0x21, 0x21,    "exclam",
		{
			    O_ ,     O_ ,
			    O_ ,     O_ ,
			    O_ ,     O_ ,
			    O_ ,     __ ,
			    __ ,     O_ ,
			    O_ ,     __ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0042, "dbl-quote",       13500, 0x0022,     0x22, 0x22,    "quotedbl",
		{
			  O_O_ ,   O_O_ ,
			  O_O_ ,   O_O_ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0043, "#",               25500, 0x0023,     0x23, 0x23,    "numbersign",
		{
			______ , _O_O__ ,
			_O_O__ , OOOOO_ ,
			OOOOO_ , _O_O__ ,
			_O_O__ , OOOOO_ ,
			OOOOO_ , _O_O__ ,
			_O_O__ , ______ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0044, "$",               16300, 0x0024,     0x24, 0x24,    "dollar",
		{
			__O___ , _OOOO_ ,
			_OOOO_ , O_O___ ,
			O_O___ , _OOO__ ,
			_OOO__ , __O_O_ ,
			__O_O_ , OOOO__ ,
			OOOO__ , ______ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0045, "%",               16200, 0x0025,     0x25, 0x25,    "percent",
		{
			______ ,   O___ ,
			OO__O_ ,   __O_ ,
			OO_O__ ,   _O__ ,
			__O___ ,   O___ ,
			_O_OO_ ,   __O_ ,
			O__OO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0046, "&",               17600, 0x0026,     0x26, 0x26,    "ampersand",
		{
			_OO___ ,  OOO__ ,
			O__O__ ,  O_O__ ,
			_OO___ ,  _O___ ,
			O__OO_ ,  O_OO_ ,
			O__O__ ,  OOO__ ,
			_OO_O_ ,  ___O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0047, "'",               13400, 0x0027,     0x27, 0x27,    "quotesingle",
		{
			    O_ ,     O_ ,
			    O_ ,     O_ ,
			    __ ,     __ ,
			    __ ,     __ ,
			    __ ,     __ ,
			    __ ,     __ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0050, "(",               12100, 0x0028,     0x28, 0x28,    "parenleft",
		{
			   _O_ ,    _O_ ,
			   O__ ,    O__ ,
			   O__ ,    O__ ,
			   O__ ,    O__ ,
			   O__ ,    _O_ ,
			   _O_ ,    ___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0051, ")",               12200, 0x0029,     0x29, 0x29,    "parenright",
		{
			   O__ ,    O__ ,
			   _O_ ,    _O_ ,
			   _O_ ,    _O_ ,
			   _O_ ,    _O_ ,
			   _O_ ,    O__ ,
			   O__ ,    ___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0052, "*",               13700, 0x002a,     0x2a, 0x2A,    "asterisk",
		{
			_O_O__ , _O_O__ ,
			__O___ , __O___ ,
			OOOOO_ , OOOOO_ ,
			__O___ , __O___ ,
			_O_O__ , _O_O__ ,
			______ , ______ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0053, "+",               12300, 0x002b,     0x2b, 0x2B,    "plus",
		{
			______ ,   ____ ,
			__O___ ,   _O__ ,
			__O___ ,   OOO_ ,
			OOOOO_ ,   _O__ ,
			__O___ ,   ____ ,
			__O___ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0054, ",",               12800, 0x002c,     0x2c, 0x2C,    "comma",
		{
			   ___ ,    ___ ,
			   ___ ,    ___ ,
			   ___ ,    ___ ,
			   OO_ ,    ___ ,
			   _O_ ,    _O_ ,
			   O__ ,    O__ ,
		}
	},

#if defined(INCLUDE_YREG_CODE)
/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
 	{ 0055, "-",               12400, 0x002d,     0x2d, 0x2D,    "hyphen",
 		{
			____ ,   ___ ,
			____ ,   ___ ,
			____ ,   OO_ ,
			OOO_ ,   ___ ,
			____ ,   ___ ,
			____ ,   ___ ,
 		}
 	},
#else
/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0055, "-",               12400, 0x002d,     0x2d, 0x2D,    "hyphen",
		{
			______ ,   ____ ,
			______ ,   ____ ,
			______ ,   OOO_ ,
			OOOOO_ ,   ____ ,
			______ ,   ____ ,
			______ ,   ____ ,
		}
	},
#endif

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0056, ".",               12900, 0x002e,     0x2e, 0x2E,    "period",
		{
			   ___ ,     __ ,
			   ___ ,     __ ,
			   ___ ,     __ ,
			   ___ ,     __ ,
			   OO_ ,     O_ ,
			   OO_ ,     __ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0057, "/",               12600, 0x002f,     0x2f, 0x2F,    "slash",
		{
			  __O_ ,   __O_ ,
			  __O_ ,   __O_ ,
			  _O__ ,   _O__ ,
			  _O__ ,   _O__ ,
			  O___ ,   O___ ,
			  O___ ,   O___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0060, "0",                1000, 0x0030,     0x30, 0x30,    "zero",
		{
			 _OO__ ,   _OO_ ,
			 O__O_ ,   O_O_ ,
			 O_OO_ ,   O_O_ ,
			 OO_O_ ,   O_O_ ,
			 O__O_ ,   OO__ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0061, "1",                1100, 0x0031,     0x31, 0x31,    "one",
		{
			   _O_ ,    _O_ ,
			   OO_ ,    OO_ ,
			   _O_ ,    _O_ ,
			   _O_ ,    _O_ ,
			   _O_ ,    _O_ ,
			   _O_ ,    ___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0062, "2",                1200, 0x0032,     0x32, 0x32,    "two",
		{
			 _OO__ ,   OOO_ ,
			 O__O_ ,   __O_ ,
			 ___O_ ,   _OO_ ,
			 __O__ ,   O___ ,
			 _O___ ,   OOO_ ,
			 OOOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0063, "3",                1300, 0x0033,     0x33, 0x33,    "three",
		{
			 OOOO_ ,   OOO_ ,
			 ___O_ ,   __O_ ,
			 __O__ ,   _OO_ ,
			 ___O_ ,   __O_ ,
			 O__O_ ,   OOO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0064, "4",                1400, 0x0034,     0x34, 0x34,    "four",
		{
			 O_O__ ,   O_O_ ,
			 O_O__ ,   O_O_ ,
			 O_O__ ,   OOO_ ,
			 OOOO_ ,   __O_ ,
			 __O__ ,   __O_ ,
			 __O__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0065, "5",                1500, 0x0035,     0x35, 0x35,    "five",
		{
			 OOOO_ ,   OOO_ ,
			 O____ ,   O___ ,
			 OOO__ ,   OOO_ ,
			 ___O_ ,   __O_ ,
			 ___O_ ,   OOO_ ,
			 OOO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0066, "6",                1600, 0x0036,     0x36, 0x36,    "six",
		{
			 _OO__ ,   _OO_ ,
			 O____ ,   O___ ,
			 OOO__ ,   OOO_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   OOO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0067, "7",                1700, 0x0037,     0x37, 0x37,    "seven",
		{
			 OOOO_ ,   OOO_ ,
			 ___O_ ,   __O_ ,
			 __O__ ,   __O_ ,
			 __O__ ,   _O__ ,
			 _O___ ,   _O__ ,
			 _O___ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0070, "8",                1800, 0x0038,     0x38, 0x38,    "eight",
		{
			 _OO__ ,   OOO_ ,
			 O__O_ ,   O_O_ ,
			 _OO__ ,   OOO_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   OOO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0071, "9",                1900, 0x0039,     0x39, 0x39,    "nine",
		{
			 _OO__ ,   OOO_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   OOO_ ,
			 _OOO_ ,   __O_ ,
			 ___O_ ,   OO__ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0072, ":",               13200, 0x003a,     0x3a, 0x3A,    "colon",
		{
			   ___ ,     __ ,
			   OO_ ,     __ ,
			   OO_ ,     O_ ,
			   ___ ,     __ ,
			   OO_ ,     O_ ,
			   OO_ ,     __ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0073, ";",               13300, 0x003b,     0x3b, 0x3B,    "semicolon",
		{
			   ___ ,    ___ ,
			   OO_ ,    ___ ,
			   ___ ,    _O_ ,
			   OO_ ,    ___ ,
			   _O_ ,    _O_ ,
			   O__ ,    O__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0074, "<",               15500, 0x003c,     0x3c, 0x3C,    "less",
		{
			 _____ ,   __O_ ,
			 ___O_ ,   _O__ ,
			 __O__ ,   O___ ,
			 _O___ ,   _O__ ,
			 OOOO_ ,   __O_ ,
			 _____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0075, "=",               15700, 0x003d,     0x3d, 0x3D,    "equal",
		{
			______ ,   ____ ,
			______ ,   OOO_ ,
			OOOOO_ ,   ____ ,
			______ ,   OOO_ ,
			OOOOO_ ,   ____ ,
			______ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0076, ">",               16100, 0x003e,     0x3e, 0x3E,    "greater",
		{
			 _____ ,   O___ ,
			 O____ ,   _O__ ,
			 _O___ ,   __O_ ,
			 __O__ ,   _O__ ,
			 OOOO_ ,   O___ ,
			 _____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0077, "?",               13100, 0x003f,     0x3f, 0x3F,    "question",
		{
			  OO__ ,   OOO_ ,
			  __O_ ,   __O_ ,
			  __O_ ,   _OO_ ,
			  _O__ ,   ____ ,
			  ____ ,   _O__ ,
			  _O__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0100, "@",               13800, 0x0040,     0x40, 0x40,    "at",
		{
			_OOO__ ,  _OO__ ,
			O___O_ ,  O__O_ ,
			O_O_O_ ,  O_OO_ ,
			O_OOO_ ,  O____ ,
			O_____ ,  _OOO_ ,
			_OOO__ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0101, "A",                2000, 0x0041,     0x41, 0x41,    CNULL,
		{
			_OO__ ,   _O__ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   OOO_ ,
			OOOO_ ,   O_O_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0102, "B",                2600, 0x0042,     0x42, 0x42,    CNULL,
		{
			OOO__ ,   OO__ ,
			O__O_ ,   O_O_ ,
			OOO__ ,   OO__ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   OO__ ,
			OOO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0103, "C",                2700, 0x0043,     0x43, 0x43,    CNULL,
		{
			_OO__ ,   _OO_ ,
			O__O_ ,   O___ ,
			O____ ,   O___ ,
			O____ ,   O___ ,
			O__O_ ,   _OO_ ,
			_OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0104, "D",                3100, 0x0044,     0x44, 0x44,    CNULL,
		{
			OOOO__ ,   OO__ ,
			_O__O_ ,   O_O_ ,
			_O__O_ ,   O_O_ ,
			_O__O_ ,   O_O_ ,
			_O__O_ ,   OO__ ,
			OOOO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0105, "E",                3300, 0x0045,     0x45, 0x45,    CNULL,
		{
			OOOO_ ,   OOO_ ,
			O____ ,   O___ ,
			OOO__ ,   OO__ ,
			O____ ,   O___ ,
			O____ ,   OOO_ ,
			OOOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0106, "F",                3800, 0x0046,     0x46, 0x46,    CNULL,
		{
			OOOO_ ,   OOO_ ,
			O____ ,   O___ ,
			OOO__ ,   OO__ ,
			O____ ,   O___ ,
			O____ ,   O___ ,
			O____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0107, "G",                3900, 0x0047,     0x47, 0x47,    CNULL,
		{
			_OOO__ ,   _OO_ ,
			O___O_ ,   O___ ,
			O_____ ,   O_O_ ,
			O__OO_ ,   O_O_ ,
			O___O_ ,   OOO_ ,
			_OOO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0110, "H",                4000, 0x0048,     0x48, 0x48,    CNULL,
		{
			O__O_ ,   O_O_ ,
			O__O_ ,   O_O_ ,
			OOOO_ ,   OOO_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0111, "I",                4200, 0x0049,     0x49, 0x49,    CNULL,
		{
			  OOO_ ,     O_ ,
			  _O__ ,     O_ ,
			  _O__ ,     O_ ,
			  _O__ ,     O_ ,
			  _O__ ,     O_ ,
			  OOO_ ,     __ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0112, "J",                4700, 0x004a,     0x4a, 0x4A,    CNULL,
		{
			 ___O_ ,   __O_ ,
			 ___O_ ,   __O_ ,
			 ___O_ ,   __O_ ,
			 ___O_ ,   O_O_ ,
			 O__O_ ,   OOO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0113, "K",                4800, 0x004b,     0x4b, 0x4B,    CNULL,
		{
			O__O_ ,  O__O_ ,
			O_O__ ,  O_O__ ,
			OO___ ,  OO___ ,
			OO___ ,  O_O__ ,
			O_O__ ,  O__O_ ,
			O__O_ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0114, "L",                4900, 0x004c,     0x4c, 0x4C,    CNULL,
		{
			 O____ ,   O___ ,
			 O____ ,   O___ ,
			 O____ ,   O___ ,
			 O____ ,   O___ ,
			 O____ ,   OOO_ ,
			 OOOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0115, "M",                5000, 0x004d,     0x4d, 0x4D,    CNULL,
		{
			O___O_ , O___O_ ,
			OO_OO_ , OO_OO_ ,
			O_O_O_ , O_O_O_ ,
			O_O_O_ , O___O_ ,
			O___O_ , O___O_ ,
			O___O_ , ______ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0116, "N",                5100, 0x004e,     0x4e, 0x4E,    CNULL,
		{
			O___O_ ,  O__O_ ,
			OO__O_ ,  OO_O_ ,
			O_O_O_ ,  O_OO_ ,
			O__OO_ ,  O__O_ ,
			O___O_ ,  O__O_ ,
			O___O_ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0117, "O",                5300, 0x004f,     0x4f, 0x4F,    CNULL,
		{
			_OOO__ ,   OOO_ ,
			O___O_ ,   O_O_ ,
			O___O_ ,   O_O_ ,
			O___O_ ,   O_O_ ,
			O___O_ ,   OOO_ ,
			_OOO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0120, "P",                5900, 0x0050,     0x50, 0x50,    CNULL,
		{
			OOO__ ,   OOO_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   OOO_ ,
			OOO__ ,   O___ ,
			O____ ,   O___ ,
			O____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0121, "Q",                6000, 0x0051,     0x51, 0x51,    CNULL,
		{
			_OOO__ ,  _OO__ ,
			O___O_ ,  O__O_ ,
			O___O_ ,  O__O_ ,
			O_O_O_ ,  O_O__ ,
			O__O__ ,  _O_O_ ,
			_OO_O_ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0122, "R",                6100, 0x0052,     0x52, 0x52,    CNULL,
		{
			OOO__ ,   OOO__ ,
			O__O_ ,   O_O__ ,
			O__O_ ,   OO___ ,
			OOO__ ,   O_O__ ,
			O_O__ ,   O__O_ ,
			O__O_ ,   _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0123, "S",                6300, 0x0053,     0x53, 0x53,    CNULL,
		{
			 _OO__ ,   _OO_ ,
			 O__O_ ,   O___ ,
			 _O___ ,   _O__ ,
			 __O__ ,   __O_ ,
			 O__O_ ,   OO__ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0124, "T",                6600, 0x0054,     0x54, 0x54,   CNULL,
		{
			OOOOO_ ,   OOO_ ,
			__O___ ,   _O__ ,
			__O___ ,   _O__ ,
			__O___ ,   _O__ ,
			__O___ ,   _O__ ,
			__O___ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0125, "U",                6700, 0x0055,     0x55, 0x55,    CNULL,
		{
			O__O_ ,   O_O_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   OOO_ ,
			_OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0126, "V",                7300, 0x0056,     0x56, 0x56,    CNULL,
		{
			O___O_ ,   O_O_ ,
			O___O_ ,   O_O_ ,
			_O_O__ ,   O_O_ ,
			_O_O__ ,   _O__ ,
			__O___ ,   _O__ ,
			__O___ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0127, "W",                7400, 0x0057,     0x57, 0x57,    CNULL,
		{
			O___O_ , O__O_ ,
			O___O_ , O__O_ ,
			O_O_O_ , O__O_ ,
			O_O_O_ , OOOO_ ,
			OO_OO_ , O__O_ ,
			O___O_ , _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0130, "X",                7500, 0x0058,     0x58, 0x58,    CNULL,
		{
			O___O_ ,   O_O_ ,
			_O_O__ ,   O_O_ ,
			__O___ ,   _O__ ,
			__O___ ,   O_O_ ,
			_O_O__ ,   O_O_ ,
			O___O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0131, "Y",                7800, 0x0059,     0x59, 0x59,    CNULL,
		{
			O___O_ ,   O_O_ ,
			_O_O__ ,   O_O_ ,
			__O___ ,   _O__ ,
			__O___ ,   _O__ ,
			__O___ ,   _O__ ,
			__O___ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0132, "Z",                8300, 0x005a,     0x5a, 0x5A,    CNULL,
		{
			 OOOO_ ,   OOO_ ,
			 ___O_ ,   __O_ ,
			 __O__ ,   _O__ ,
			 _O___ ,   O___ ,
			 O____ ,   OOO_ ,
			 OOOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0133, "[",               18100, 0x005b,     0x5b, 0x5B,    "bracketleft",
		{
			  OOO_ ,    OO_ ,
			  O___ ,    O__ ,
			  O___ ,    O__ ,
			  O___ ,    O__ ,
			  O___ ,    OO_ ,
			  OOO_ ,    ___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0134, "backslash",       17700, 0x005c,     0x5c, 0x5C,    CNULL,
		{
			  O___ ,   O___ ,
			  O___ ,   O___ ,
			  _O__ ,   _O__ ,
			  _O__ ,   _O__ ,
			  __O_ ,   __O_ ,
			  __O_ ,   __O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0135, "]",               18200, 0x005d,     0x5d, 0x5D,    "bracketright",
		{
			  OOO_ ,    OO_ ,
			  __O_ ,    _O_ ,
			  __O_ ,    _O_ ,
			  __O_ ,    _O_ ,
			  __O_ ,    OO_ ,
			  OOO_ ,    ___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0136, "^",               17800, 0x005e,     0x5e, 0x5E,    "asciicircum",
		{
			  _O__ ,   _O__ ,
			  O_O_ ,   O_O_ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0137, "_",               13900, 0x005f,     0x5f, 0x5F,    "underscore",
		{
			 _____ ,   ____ ,
			 _____ ,   ____ ,
			 _____ ,   ____ ,
			 _____ ,   ____ ,
			 _____ ,   ____ ,
			 OOOO_ ,   OOO_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0140, "back-quote",      25400, 0x0060,     0x60, 0x60,    "accentgrave",
		{
			    O__ ,     O__ ,
			    _O_ ,     _O_ ,
			    ___ ,     ___ ,
			    ___ ,     ___ ,
			    ___ ,     ___ ,
			    ___ ,     ___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0141, "a",                2000, 0x0061,     0x61, 0x61,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   _OO_ ,
			 _OOO_ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   _OO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0142, "b",                2600, 0x0062,     0x62, 0x62,    CNULL,
		{
			 O____ ,   O___ ,
			 O____ ,   O___ ,
			 OOO__ ,   OOO_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   OOO_ ,
			 OOO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0143, "c",                2700, 0x0063,     0x63, 0x63,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   _OO_ ,
			 _OO__ ,   O___ ,
			 O____ ,   O___ ,
			 O____ ,   _OO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0144, "d",                3100, 0x0064,     0x64, 0x64,    CNULL,
		{
			 ___O_ ,   __O_ ,
			 ___O_ ,   __O_ ,
			 _OOO_ ,   OOO_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   OOO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0145, "e",                3300, 0x0065,     0x65, 0x65,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   _OO_ ,
			 _OO__ ,   O_O_ ,
			 O_OO_ ,   OO__ ,
			 OO___ ,   _OO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0146, "f",                3800, 0x0066,     0x66, 0x66,    CNULL,
		{
			 __OO_ ,   _OO_ ,
			 _O___ ,   _O__ ,
			 _O___ ,   OOO_ ,
			 OOO__ ,   _O__ ,
			 _O___ ,   _O__ ,
			 _O___ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0147, "g",                3900, 0x0067,     0x67, 0x67,    CNULL,
		{
			 _____ ,   ____ ,
			 _OOO_ ,   _OO_ ,
			 O__O_ ,   O_O_ ,
			 _OOO_ ,   OOO_ ,
			 ___O_ ,   __O_ ,
			 OOO__ ,   OO__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0150, "h",                4000, 0x0068,     0x68, 0x68,    CNULL,
		{
			 O____ ,   O___ ,
			 O____ ,   O___ ,
			 OOO__ ,   OOO_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0151, "i",                4200, 0x0069,     0x69, 0x69,    CNULL,
		{
			  _O__ ,    O__ ,
			  ____ ,    ___ ,
			  OO__ ,    O__ ,
			  _O__ ,    O__ ,
			  _O__ ,    OO_ ,
			  OOO_ ,    ___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0152, "j",                4700, 0x006a,     0x6a, 0x6A,    CNULL,
		{
			  __O_ ,    _O_ ,
			  ____ ,    ___ ,
			  _OO_ ,    _O_ ,
			  __O_ ,    _O_ ,
			  __O_ ,    _O_ ,
			  OO__ ,    OO_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0153, "k",                4800, 0x006b,     0x6b, 0x6B,    CNULL,
		{
			 O____ ,   O___ ,
			 O____ ,   O_O_ ,
			 O__O_ ,   OO__ ,
			 OOO__ ,   O_O_ ,
			 O_O__ ,   O_O_ ,
			 O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0154, "l",                4900, 0x006c,     0x6c, 0x6C,    CNULL,
		{
			  OO__ ,    O__ ,
			  _O__ ,    O__ ,
			  _O__ ,    O__ ,
			  _O__ ,    O__ ,
			  _O__ ,    OO_ ,
			  OOO_ ,    ___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0155, "m",                5000, 0x006d,     0x6d, 0x6D,    CNULL,
		{
			______ , ______ ,
			______ , OOOO__ ,
			OOOO__ , O_O_O_ ,
			O_O_O_ , O_O_O_ ,
			O_O_O_ , O_O_O_ ,
			O_O_O_ , ______ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0156, "n",                5100, 0x006e,     0x6e, 0x6E,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   OO__ ,
			 OOO__ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0157, "o",                5300, 0x006f,     0x6f, 0x6F,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   OO__ ,
			 _OO__ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   _OO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0160, "p",                5900, 0x0070,     0x70, 0x70,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   OOO_ ,
			 OOO__ ,   O_O_ ,
			 O__O_ ,   OOO_ ,
			 OOO__ ,   O___ ,
			 O____ ,   O___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0161, "q",                6000, 0x0071,     0x71, 0x71,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   OOO_ ,
			 _OOO_ ,   O_O_ ,
			 O__O_ ,   OOO_ ,
			 _OOO_ ,   __O_ ,
			 ___O_ ,   __O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0162, "r",                6100, 0x0072,     0x72, 0x72,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   _OO_ ,
			 O_OO_ ,   O___ ,
			 OO___ ,   O___ ,
			 O____ ,   O___ ,
			 O____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0163, "s",                6300, 0x0073,     0x73, 0x73,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   OOO_ ,
			 _OOO_ ,   OO__ ,
			 _O___ ,   __O_ ,
			 __O__ ,   OOO_ ,
			 OOO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0164, "t",                6600, 0x0074,     0x74, 0x74,    CNULL,
		{
			 _O___ ,   _O__ ,
			 _O___ ,   OOO_ ,
			 OOOO_ ,   _O__ ,
			 _O___ ,   _O__ ,
			 _O___ ,   _OO_ ,
			 __OO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0165, "u",                6700, 0x0075,     0x75, 0x75,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   _OO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0166, "v",                7300, 0x0076,     0x76, 0x76,    CNULL,
		{
			______ ,   ____ ,
			______ ,   O_O_ ,
			O___O_ ,   O_O_ ,
			O___O_ ,   OOO_ ,
			_O_O__ ,   _O__ ,
			__O___ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0167, "w",                7400, 0x0077,     0x77, 0x77,    CNULL,
		{
			______ , ______ ,
			______ , O___O_ ,
			O___O_ , O_O_O_ ,
			O_O_O_ , O_O_O_ ,
			O_O_O_ , _O_O__ ,
			_O_O__ , ______ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0170, "x",                7500, 0x0078,     0x78, 0x78,    CNULL,
		{
			  ____ ,   ____ ,
			  ____ ,   O_O_ ,
			  O_O_ ,   _O__ ,
			  _O__ ,   _O__ ,
			  _O__ ,   O_O_ ,
			  O_O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0171, "y",                7800, 0x0079,     0x79, 0x7A,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 _O_O_ ,   _OO_ ,
			 __O__ ,   __O_ ,
			 OO___ ,   OO__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0172, "z",                8300, 0x007a,     0x7a, 0x7B,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   OOO_ ,
			 OOO__ ,   _OO_ ,
			 __O__ ,   O___ ,
			 _O___ ,   OOO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0173, "{",               18300, 0x007b,     0x7b, 0x7C,    "braceleft",
		{
			  _OO_ ,   _OO_ ,
			  _O__ ,   _O__ ,
			  O___ ,   OO__ ,
			  O___ ,   _O__ ,
			  _O__ ,   _OO_ ,
			  _OO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0174, "|",               17900, 0x007c,     0x7c, 0x7D,    "bar",
		{
			    O_ ,     O_ ,
			    O_ ,     O_ ,
			    O_ ,     O_ ,
			    O_ ,     O_ ,
			    O_ ,     O_ ,
			    O_ ,     O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0175, "}",               18400, 0x007d,     0x7d, 0x7E,    "braceright",
		{
			  OO__ ,   OO__ ,
			  _O__ ,   _O__ ,
			  __O_ ,   _OO_ ,
			  __O_ ,   _O__ ,
			  _O__ ,   OO__ ,
			  OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0176, "~",               14000, 0x007e,     0x7e, 0x7E,    "asciitilde",
		{
			______ ,  _____ ,
			_O____ ,  _O_O_ ,
			O_O_O_ ,  O_O__ ,
			___O__ ,  _____ ,
			______ ,  _____ ,
			______ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0177, "^v",              19100, 0x2195,     0x7f, 0x00,    "up-down",
		{
			  _O__ ,   _O__ ,
			  OOO_ ,   OOO_ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  OOO_ ,   OOO_ ,
			  _O__ ,   _O__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0200, "^3",               1300, 0x00b3,     0xb3, 0x98,    "super-3",
		{
			  OO__ ,   OO__ ,
			  _O__ ,   _O__ ,
			  __O_ ,   __O_ ,
			  OOO_ ,   OOO_ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0201, "sub-w",            7400, 0x24b2,     0x00, 0x00,    CNULL,
		{
			______ , ______ ,
			______ , ______ ,
			______ , ______ ,
			______ , O_O_O_ ,
			O_O_O_ , O_O_O_ ,
			_O_O__ , _O_O__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0202, "GAMMA",            9300, 0x0393,     0x00, 0x00,    CNULL,
		{
			OOOO_ ,   OOO_ ,
			O__O_ ,   O___ ,
			O____ ,   O___ ,
			O____ ,   O___ ,
			O____ ,   O___ ,
			O____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0203, "DELTA",            9400, 0x0394,     0x00, 0x00,    CNULL,
		{
			__O___ ,  ___O_ ,
			__O___ ,  __OO_ ,
			_O_O__ ,  _O_O_ ,
			_O_O__ ,  O__O_ ,
			O___O_ ,  OOOO_ ,
			OOOOO_ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0204, "D-bar",            3200, 0x00d0,     0xd0, 0xE3,   CNULL,
		{
			OOOO__ ,  _OO__ ,
			_O__O_ ,  _O_O_ ,
			OOO_O_ ,  OO_O_ ,
			_O__O_ ,  _O_O_ ,
			_O__O_ ,  _OO__ ,
			OOOO__ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0205, "d-bar",            3200, 0x00f0,     0xf0, 0xE4,    CNULL,
		{
			___O__ ,  __O__ ,
			__OOO_ ,  _OOO_ ,
			___O__ ,  O_O__ ,
			_OOO__ ,  O_O__ ,
			O__O__ ,  _OO__ ,
			_OOO__ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0206, "sub-d",            3100, 0x249f,     0x0e, 0x00,    CNULL,
		{
			  ____ ,    ____ ,
			  ____ ,    ____ ,
			  ____ ,    ____ ,
			  __O_ ,    __O_ ,
			  OOO_ ,    OOO_ ,
			  OOO_ ,    OOO_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0207, "THETA",            9800, 0x0398,     0x00, 0x00,    CNULL,
		{
			_OOO__ ,  _OO__ ,
			O___O_ ,  O__O_ ,
			O___O_ ,  OOOO_ ,
			OOOOO_ ,  O__O_ ,
			O___O_ ,  _OO__ ,
			_OOO__ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0210, "AE",               2500, 0x00c6,     0xc6, 0xD3,    CNULL,
		{
			_OOOO_ ,  _OOO_ ,
			O_O___ ,  O_O__ ,
			O_OOO_ ,  OOOO_ ,
			OOO___ ,  O_O__ ,
			O_O___ ,  O_OO_ ,
			O_OOO_ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0211, "ae",               2500, 0x00e6,     0xe6, 0xD7,    CNULL,
		{
			______ , ______ ,
			______ , _OOOO_ ,
			_OOOO_ , O_O_O_ ,
			O_O_O_ , O_OO__ ,
			O_OO__ , _OOOO_ ,
			_OOOO_ , ______ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0212, "LAMBDA",          10100, 0x039b,     0x00, 0x00,    CNULL,
		{
			__O___ ,   _O__ ,
			__O___ ,   _O__ ,
			_O_O__ ,   O_O_ ,
			_O_O__ ,   O_O_ ,
			O___O_ ,   O_O_ ,
			O___O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0213, "sub-x",            7500, 0x2093,     0x08, 0x00,    CNULL,
		{
			 ____ ,    ____ ,
			 ____ ,    ____ ,
			 ____ ,    ____ ,
			 O_O_ ,    O_O_ ,
			 _O__ ,    _O__ ,
			 O_O_ ,    O_O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0214, "sub-y",            7800, 0x1d67,     0x09, 0x00,    CNULL,
		{
			 ____ ,    ____ ,
			 ____ ,    ____ ,
			 ____ ,    ____ ,
			 O_O_ ,    O_O_ ,
			 _O__ ,    _O__ ,
			 _O__ ,    _O__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0215, "XI",              10400, 0x039e,     0x00, 0x00,    CNULL,
		{
			OOOO_ ,  OOOO_ ,
			_____ ,  _____ ,
			_OO__ ,  _OO__ ,
			_____ ,  _____ ,
			_____ ,  OOOO_ ,
			OOOO_ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0216, "sol",             17400, 0x2609,     0x00, 0x00,    CNULL,
		{
			______ , ______ ,
			_OOO__ , _OOO__ ,
			O___O_ , O___O_ ,
			O_O_O_ , O_O_O_ ,
			O___O_ , O___O_ ,
			_OOO__ , _OOO__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0217, "PI",              10600, 0x03a0,     0x00, 0x00,    CNULL,
		{
			OOOO_ ,   OOO_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0220, "super-star",      13700, 0x207a,     0xa4, 0x00,    CNULL,
		{
			O_O_O_ , _O__ ,
			_OOO__ , O_O_ ,
			O_O_O_ , _O__ ,
			______ , ____ ,
			______ , ____ ,
			______ , ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0221, "SIGMA",           10800, 0x03a3,     0x00, 0x85,    CNULL,
		{
			OOOOO_ ,  OOOO_ ,
			_O____ ,  _O___ ,
			__O___ ,  __O__ ,
			__O___ ,  _O___ ,
			_O____ ,  OOOO_ ,
			OOOOO_ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0222, "print",           19000, 0x2399,     0x0c, 0x00,    "printscreensymbol",
		{
			     _OOO__ ,      _OO__ ,
			     _O_O__ ,      _OO__ ,
			     OO_OO_ ,      OOOO_ ,
			     OOOOO_ ,      O__O_ ,
			     O___O_ ,      OOOO_ ,
			     OOOOO_ ,      _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0223, "sub-q",            6000, 0x209b,     0x0a, 0x00,    CNULL,
		{
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  OOO_ ,   OOO_ ,
			  OOO_ ,   OOO_ ,
			  __O_ ,   __O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0224, "PHI",             11100, 0x03a6,     0x00, 0x00,    CNULL,
		{
			__O___ , _OOO__ ,
			_OOO__ , O_O_O_ ,
			O_O_O_ , O_O_O_ ,
			O_O_O_ , _OOO__ ,
			_OOO__ , __O___ ,
			__O___ , ______ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0225, "not",             14100, 0x00ac,     0xac, 0x00,    CNULL,
		{
			______ ,   ____ ,
			______ ,   ____ ,
			______ ,   OOO_ ,
			OOOOO_ ,   __O_ ,
			____O_ ,   ____ ,
			______ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0226, "PSI",             11300, 0x03a8,     0x00, 0x00,    CNULL,
		{
			__O___ , O_O_O_ ,
			O_O_O_ , O_O_O_ ,
			O_O_O_ , _OOO__ ,
			_OOO__ , __O___ ,
			__O___ , __O___ ,
			__O___ , ______ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0227, "OMEGA",           11400, 0x03a9,     0x00, 0x00,    CNULL,
		{
			_OOO__ ,  _OO__ ,
			O___O_ ,  O__O_ ,
			O___O_ ,  O__O_ ,
			O___O_ ,  _OO__ ,
			_O_O__ ,  OOOO_ ,
			OO_OO_ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0230, "sub-B",            2600, 0x249d,     0x00, 0x00,    CNULL,
		{
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  OO__ ,   OO__ ,
			  OOO_ ,   OOO_ ,
			  OOO_ ,   OOO_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0231, "sub-mu",          10200, 0x00b6,     0x00, 0x00,    CNULL,
		{
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  O_O_ ,   O_O_ ,
			  OOO_ ,   OOO_ ,
			  O___ ,   O___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0232, "^2",               1200, 0x00b2,     0xb2, 0x97,    "super-2",
		{
			  OOO_ ,    OO_ ,
			  __O_ ,    _O_ ,
			  _O__ ,    O__ ,
			  OOO_ ,    OO_ ,
			  ____ ,    ___ ,
			  ____ ,    ___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0233, "sub-infinity",    17300, 0x2088,     0x00, 0x00,    CNULL,
		{
			______ , ______ ,
			______ , ______ ,
			______ , ______ ,
			_O_O__ , _O_O__ ,
			O_O_O_ , O_O_O_ ,
			_O_O__ , _O_O__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0234, "^x",               7500, 0x1d61,     0x00, 0x00,    "super-x",
		{
			  O_O_ ,   O_O_ ,
			  _O__ ,   _O__ ,
			  O_O_ ,   O_O_ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0235, "^-1",             12400, 0x2078,     0x00, 0x00,    "reciprocal",
		{
			____O_ ,  ___O_ ,
			___OO_ ,  OO_O_ ,
			OO__O_ ,  ___O_ ,
			____O_ ,  _____ ,
			______ ,  _____ ,
			______ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0236, "h-bar",            4100, 0x0127,     0x00, 0x00,    CNULL,
		{
			_O____ ,  _O___ ,
			OOOO__ ,  OOO__ ,
			_O____ ,  _O___ ,
			_OOO__ ,  _OOO_ ,
			_O__O_ ,  _O_O_ ,
			_O__O_ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0237, "infinity",        17300, 0x221e,     0x00, 0x00,    CNULL,
		{
			______ , ______ ,
			______ , _O_O__ ,
			_O_O__ , O_O_O_ ,
			O_O_O_ , O_O_O_ ,
			O_O_O_ , _O_O__ ,
			_O_O__ , ______ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0240, "alpha",            9100, 0x03b1,     0x00, 0x8C,    CNULL,
		{
			______ ,  _____ ,
			______ ,  _O_O_ ,
			_OO_O_ ,  O_O__ ,
			O__O__ ,  O_O__ ,
			O__O__ ,  _O_O_ ,
			_OO_O_ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0241, "beta",             9200, 0x03b2,     0x00, 0x00,    CNULL,
		{
			 _O___ ,   _O__ ,
			 O_O__ ,   O_O_ ,
			 OOO__ ,   OO__ ,
			 O__O_ ,   O_O_ ,
			 OOO__ ,   OO__ ,
			 O____ ,   O___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0242, "gamma",            9300, 0x03b3,     0x00, 0x00,    CNULL,
		{
			 _____ ,   _____ ,
			 _O_O_ ,   _O_O_ ,
			 O_O__ ,   O_O__ ,
			 __O__ ,   __O__ ,
			 __O__ ,   __O__ ,
			 __O__ ,   __O__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0243, "delta",            9400, 0x03b4,     0x00, 0x88,    CNULL,
		{
			 __O__ ,   _O__ ,
			 _O___ ,   O___ ,
			 __O__ ,   _O__ ,
			 _OOO_ ,   O_O_ ,
			 O__O_ ,   OOO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0244, "epsilon",          9500, 0x03b5,     0x00, 0x00,    CNULL,
		{
			 _____ ,   _OO_ ,
			 _OO__ ,   O___ ,
			 O____ ,   _OO_ ,
			 _O___ ,   O___ ,
			 O__O_ ,   _OO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0245, "zeta",             9600, 0x03b6,     0x00, 0x00,    CNULL,
		{
			 OOOO_ ,   OOO_ ,
			 ___O_ ,   __O_ ,
			 __O__ ,   _O__ ,
			 _O___ ,   O___ ,
			 _OOO_ ,   OOO_ ,
			 ___O_ ,   __O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0246, "eta",              9700, 0x03b7,     0x00, 0x00,    CNULL,
		{
			______ ,  _____ ,
			______ ,  OOO__ ,
			O_OO__ ,  _O_O_ ,
			_O__O_ ,  _O_O_ ,
			_O__O_ ,  ___O_ ,
			____O_ ,  ___O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0247, "theta",            9800, 0x03b8,     0x00, 0x00,    CNULL,
		{
			_OO___ ,  OOO__ ,
			O__O__ ,  O_O__ ,
			O__O__ ,  _OOO_ ,
			_OOOO_ ,  O_O__ ,
			O__O__ ,  OOO__ ,
			_OO___ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0250, "iota",             9900, 0x03b9,     0x00, 0x00,    CNULL,
		{
			   ___ ,    ___ ,
			   ___ ,    O__ ,
			   O__ ,    O__ ,
			   O__ ,    O__ ,
			   O__ ,    OO_ ,
			   OO_ ,    ___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0251, "kappa",           10000, 0x03ba,     0x00, 0x00,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   O_O_ ,
			 O_OO_ ,   OO__ ,
			 OO___ ,   O_O_ ,
			 O_O__ ,   O_O_ ,
			 O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0252, "lambda",          10100, 0x03bb,     0x00, 0x00,    CNULL,
		{
			 O____ ,   OO__ ,
			 _O___ ,   __O_ ,
			 __O__ ,   __O_ ,
			 __O__ ,   _OO_ ,
			 _O_O_ ,   O_O_ ,
			 O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0253, "mu",              10200, 0x03bc,     0xb5, 0x8F,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 OOOO_ ,   OOO_ ,
			 O____ ,   O___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0254, "nu",              10300, 0x03bd,     0x00, 0x00,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   OO__ ,
			 O_O__ ,   OO__ ,
			 OO___ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0255, "xi",              10400, 0x03be,     0x00, 0x00,    CNULL,
		{
			 OOOO_ ,   OOO_ ,
			 __O__ ,   _O__ ,
			 _OOO_ ,   OOO_ ,
			 __O__ ,   _O__ ,
			 _OOO_ ,   OOO_ ,
			 ___O_ ,   __O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0256, "terra",           17500, 0x2641,     0x00, 0x00,    CNULL,
		{
			______ , ______ ,
			_OOO__ , _OOO__ ,
			O_O_O_ , O_O_O_ ,
			OOOOO_ , OOOOO_ ,
			O_O_O_ , O_O_O_ ,
			_OOO__ , _OOO__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0257, "pi",              10600, 0x03c0,     0x00, 0x87,    CNULL,
		{
			______ ,  _____ ,
			______ ,  OOOO_ ,
			OOOOO_ ,  O_O__ ,
			_O_O__ ,  O_O__ ,
			_O_O__ ,  O_OO_ ,
			_O_OO_ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0260, "rho",             10700, 0x03c1,     0x00, 0x00,    CNULL,
		{
			______ ,  _____ ,
			______ ,  __OO_ ,
			__OOO_ ,  _O_O_ ,
			_O__O_ ,  _OOO_ ,
			_OOO__ ,  _O___ ,
			OO____ ,  OO___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0261, "sigma",           10800, 0x03c3,     0x00, 0x00,    CNULL,
		{
			______ ,  _____ ,
			______ ,  _OOO_ ,
			_OOOO_ ,  O_O__ ,
			O__O__ ,  O_O__ ,
			O__O__ ,  _O___ ,
			_OO___ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0262, "tau",             10900, 0x03c4,     0x00, 0x00,    CNULL,
		{
			 _____ ,   ____ ,
			 _____ ,   OOO_ ,
			 OOOO_ ,   _O__ ,
			 _O___ ,   _O__ ,
			 _O___ ,   _OO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0263, "upsilon",         11000, 0x03c5,     0x00, 0x00,    CNULL,
		{
			______ ,  _____ ,
			______ ,  OO_O_ ,
			OO__O_ ,  _O_O_ ,
			_O__O_ ,  _O_O_ ,
			_O__O_ ,  _OO__ ,
			_OOO__ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0264, "phi",             11100, 0x03c6,     0x00, 0x00,    CNULL,
		{
			______ , ______ ,
			_OOO__ , _OOO__ ,
			O_O_O_ , O_O_O_ ,
			O_O_O_ , O_O_O_ ,
			_OOO__ , _OOO__ ,
			__O___ , __O___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0265, "chi",             11200, 0x03c7,     0x00, 0x00,    CNULL,
		{
			______ ,   ____ ,
			_OO_O_ ,   O_O_ ,
			__OO__ ,   O_O_ ,
			__O___ ,   _O__ ,
			_OO___ ,   O_O_ ,
			O_OO__ ,   O_O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0266, "psi",             11300, 0x03c8,     0x00, 0x00,    CNULL,
		{
			______ , ______ ,
			______ , O_O_O_ ,
			O_O_O_ , O_O_O_ ,
			O_O_O_ , _OOO__ ,
			_OOO__ , __O___ ,
			__O___ , __O___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0267, "omega",           11400, 0x03c9,     0x00, 0x00,    CNULL,
		{
			______ , ______ ,
			______ , _O_O__ ,
			_O_O__ , O___O_ ,
			O___O_ , O_O_O_ ,
			O_O_O_ , _O_O__ ,
			_O_O__ , ______ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0270, "sub-0",            1000, 0x2080,     0x00, 0x00,    CNULL,
		{
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  OOO_ ,   OOO_ ,
			  O_O_ ,   O_O_ ,
			  OOO_ ,   OOO_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0271, "sub-1",            1100, 0x2081,     0x00, 0x95,    CNULL,
		{
			   ___ ,    ___ ,
			   ___ ,    ___ ,
			   _O_ ,    _O_ ,
			   OO_ ,    OO_ ,
			   _O_ ,    _O_ ,
			   _O_ ,    _O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0272, "sub-2",            1200, 0x2082,     0x00, 0x96,    CNULL,
		{
			  ____ ,    ___ ,
			  ____ ,    ___ ,
			  OOO_ ,    OO_ ,
			  __O_ ,    _O_ ,
			  _O__ ,    O__ ,
			  OOO_ ,    OO_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0273, "sub-c",            2700, 0x249e,     0x00, 0x00,    CNULL,
		{
			  ____ ,    ___ ,
			  ____ ,    ___ ,
			  ____ ,    ___ ,
			  OOO_ ,    OO_ ,
			  O___ ,    O__ ,
			  OOO_ ,    OO_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0274, "sub-e",            3300, 0x2091,     0x00, 0x00,    CNULL,
		{
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   _OO_ ,
			  OOO_ ,   O_O_ ,
			  OO__ ,   OO__ ,
			  _OO_ ,   _OO_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0275, "sub-n",            5100, 0x2099,     0x00, 0x00,    CNULL,
		{
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   OO__ ,
			  OOO_ ,   O_O_ ,
			  O_O_ ,   O_O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0276, "sub-p",            5900, 0x209a,     0x00, 0x00,    CNULL,
		{
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  OOO_ ,   OOO_ ,
			  OOO_ ,   OOO_ ,
			  O___ ,   O___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0277, "sub-u",            6700, 0x1d64,     0x00, 0x00,    CNULL,
		{
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   O_O_ ,
			  O_O_ ,   O_O_ ,
			  OOO_ ,   _OO_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0300, "A-grave",          2100, 0x00c0,     0xc0, 0xA1,    CNULL,
		{
			OO___ ,   OO__ ,
			_____ ,   ____ ,
			_OO__ ,   _O__ ,
			O__O_ ,   OOO_ ,
			OOOO_ ,   O_O_ ,
			O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0301, "A-acute",          2200, 0x00c1,     0xc1, 0xE0,    CNULL,
		{
			__OO_ ,   _OO_ ,
			_____ ,   ____ ,
			_OO__ ,   _O__ ,
			O__O_ ,   OOO_ ,
			OOOO_ ,   O_O_ ,
			O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0302, "A-circumflex",     2300, 0x00c2,     0xc2, 0xA2,    CNULL,
		{
			OOOO_ ,   OOO_ ,
			_____ ,   ____ ,
			_OO__ ,   _O__ ,
			O__O_ ,   OOO_ ,
			OOOO_ ,   O_O_ ,
			O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0303, "A-umlaut",         2400, 0x00c4,     0xc4, 0xD8,    CNULL,
		{
			O__O_ ,   O_O_ ,
			_____ ,   ____ ,
			_OO__ ,   _O__ ,
			O__O_ ,   OOO_ ,
			OOOO_ ,   O_O_ ,
			O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0304, "A-dot",            2600, 0x00c5,     0xc5, 0xD0,    CNULL,
		{
			_OO__ ,   _O__ ,
			_____ ,   ____ ,
			_OO__ ,   _O__ ,
			O__O_ ,   OOO_ ,
			OOOO_ ,   O_O_ ,
			O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0305, "C-acute",          2800, 0x0106,     0x00, 0x00,    CNULL,
		{
			__OO_ ,   _OO_ ,
			_____ ,   ____ ,
			_OOO_ ,   OOO_ ,
			O____ ,   O___ ,
			O____ ,   OOO_ ,
			_OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0306, "C-hook",           2900, 0x010c,     0x00, 0x00,    CNULL,
		{
			OOOO_ ,   OOO_ ,
			_____ ,   ____ ,
			_OOO_ ,   OOO_ ,
			O____ ,   O___ ,
			O____ ,   OOO_ ,
			_OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0307, "C-cedilla",        3000, 0x00c7,     0xc7, 0xB4,    CNULL,
		{
			_OO__ ,   _OO_ ,
			O__O_ ,   O___ ,
			O____ ,   O___ ,
			O____ ,   O___ ,
			_OOO_ ,   _OO_ ,
			OO___ ,   OO__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0310, "E-grave",          3400, 0x00c8,     0xc8, 0xA4,    CNULL,
		{
			OO___ ,   OO__ ,
			_____ ,   ____ ,
			OOOO_ ,   OOO_ ,
			OOO__ ,   OO__ ,
			O____ ,   OOO_ ,
			OOOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0311, "E-acute",          3500, 0x00c9,     0xc9, 0xDC,    CNULL,
		{
			__OO_ ,   _OO_ ,
			_____ ,   ____ ,
			OOOO_ ,   OOO_ ,
			OOO__ ,   OO__ ,
			O____ ,   OOO_ ,
			OOOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0312, "E-circumflex",     3600, 0x00ca,     0xca, 0xA4,    CNULL,
		{
			OOOO_ ,   OOO_ ,
			_____ ,   ____ ,
			OOOO_ ,   OOO_ ,
			OOO__ ,   OO__ ,
			O____ ,   OOO_ ,
			OOOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0313, "E-trema",          3700, 0x00cb,     0xcb, 0xA5,    CNULL,
		{
			O__O_ ,   O_O_ ,
			_____ ,   ____ ,
			OOOO_ ,   OOO_ ,
			OOO__ ,   OO__ ,
			O____ ,   OOO_ ,
			OOOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0314, "I-grave",          4300, 0x00cc,     0xcc, 0xE6,    CNULL,
		{
			  OO__ ,    OO_ ,
			  ____ ,    ___ ,
			  OOO_ ,    _O_ ,
			  _O__ ,    _O_ ,
			  _O__ ,    _O_ ,
			  OOO_ ,    ___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0315, "I-acute",          4400, 0x00cd,     0xcd, 0xE5,    CNULL,
		{
			  _OO_ ,    OO_ ,
			  ____ ,    ___ ,
			  OOO_ ,    O__ ,
			  _O__ ,    O__ ,
			  _O__ ,    O__ ,
			  OOO_ ,    ___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0316, "I-circumflex",     4500, 0x00ce,     0xce, 0xA6,    CNULL,
		{
			  OOO_ ,   OOO_ ,
			  ____ ,   ____ ,
			  OOO_ ,   _O__ ,
			  _O__ ,   _O__ ,
			  _O__ ,   _O__ ,
			  OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0317, "I-trema",          4600, 0x00cf,     0xcf, 0xA7,    CNULL,
		{
			  O_O_ ,   O_O_ ,
			  ____ ,   ____ ,
			  OOO_ ,   _O__ ,
			  _O__ ,   _O__ ,
			  _O__ ,   _O__ ,
			  OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0320, "N-tilde",          5200, 0x00d1,     0xd1, 0xB6,    CNULL,
		{
			OOOO_ ,  OOOO_ ,
			_____ ,  _____ ,
			O__O_ ,  OO_O_ ,
			OO_O_ ,  O_OO_ ,
			O_OO_ ,  O__O_ ,
			O__O_ ,  _____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0321, "O-grave",          5400, 0x00d2,     0xd2, 0xE8,    CNULL,
		{
			OO____ ,   OO__ ,
			______ ,   ____ ,
			_OOO__ ,   OOO_ ,
			O___O_ ,   O_O_ ,
			O___O_ ,   OOO_ ,
			_OOO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0322, "O-acute",          5500, 0x00d3,     0xd3, 0xE7,    CNULL,
		{
			___OO_ ,   _OO_ ,
			______ ,   ____ ,
			_OOO__ ,   OOO_ ,
			O___O_ ,   O_O_ ,
			O___O_ ,   OOO_ ,
			_OOO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0323, "O-circumflex",     5600, 0x00d4,     0xd4, 0x00,    CNULL,
		{
			_OOO__ ,   OOO_ ,
			______ ,   ____ ,
			_OOO__ ,   OOO_ ,
			O___O_ ,   O_O_ ,
			O___O_ ,   OOO_ ,
			_OOO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0324, "O-umlaut",         5700, 0x00d6,     0xd6, 0xDA,    CNULL,
		{
			_O_O__ ,   O_O_ ,
			______ ,   ____ ,
			_OOO__ ,   OOO_ ,
			O___O_ ,   O_O_ ,
			O___O_ ,   OOO_ ,
			_OOO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0325, "R-hook",           6200, 0x0158,     0x00, 0x00,    CNULL,
		{
			OOOO_ ,   OOO_ ,
			_____ ,   ____ ,
			OOO__ ,   OO__ ,
			O__O_ ,   OO__ ,
			OOO__ ,   O_O_ ,
			O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0326, "S-hook",           6400, 0x0160,     0x8a, 0xEB,    CNULL,
		{
			 OOOO_ ,   OOO_ ,
			 _____ ,   ____ ,
			 _OOO_ ,   _OO_ ,
			 OO___ ,   _O__ ,
			 __OO_ ,   OO__ ,
			 OOO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0327, "sub-A",            2000, 0x2090,     0x00, 0x00,    CNULL,
		{
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  _O__ ,   _O__ ,
			  OOO_ ,   OOO_ ,
			  O_O_ ,   O_O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0330, "U-grave",          6800, 0x00d9,     0xd9, 0xAD,    CNULL,
		{
			OO___ ,   OO__ ,
			_____ ,   ____ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   OOO_ ,
			_OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0331, "U-acute",          6900, 0x00da,     0xda, 0xED,    CNULL,
		{
			__OO_ ,   _OO_ ,
			_____ ,   ____ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   OOO_ ,
			_OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0332, "U-circumflex",     7000, 0x00db,     0xdb, 0xAE,    CNULL,
		{
			OOOO_ ,   OOO_ ,
			_____ ,   ____ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   OOO_ ,
			_OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0333, "U-umlaut",         7100, 0x00dc,     0xdc, 0xDA,    CNULL,
		{
			O__O_ ,   O_O_ ,
			_____ ,   ____ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   OOO_ ,
			_OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0334, "U-dot",            7200, 0x016e,     0x00, 0x00,    CNULL,
		{
			_OO__ ,   _O__ ,
			_____ ,   ____ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   O_O_ ,
			O__O_ ,   OOO_ ,
			_OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0335, "Y-acute",          7900, 0x00dd,     0xdd, 0xB1,    CNULL,
		{
			___O__ ,   _OO_ ,
			__O___ ,   ____ ,
			O___O_ ,   O_O_ ,
			_O_O__ ,   _O__ ,
			__O___ ,   _O__ ,
			__O___ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0336, "Y-trema",          8100, 0x0178,     0x9f, 0xEE,    CNULL,
		{
			_O_O__ ,   O_O_ ,
			______ ,   ____ ,
			O___O_ ,   O_O_ ,
			_O_O__ ,   _O__ ,
			__O___ ,   _O__ ,
			__O___ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0337, "Z-hook",           8400, 0x017d,     0x8e, 0x00,    CNULL,
		{
			 OOOO_ ,   OOO_ ,
			 _____ ,   ____ ,
			 OOOO_ ,   OO__ ,
			 __O__ ,   _O__ ,
			 _O___ ,   _OO_ ,
			 OOOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0340, "a-grave",          2100, 0x00e0,     0xe0, 0xC8,    CNULL,
		{
			 OO___ ,   OO__ ,
			 _____ ,   ____ ,
			 _OOO_ ,   _OO_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   OOO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0341, "a-acute",          2200, 0x00e1,     0xe1, 0xC4,    CNULL,
		{
			 __OO_ ,   _OO_ ,
			 _____ ,   ____ ,
			 _OOO_ ,   _OO_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   OOO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0342, "a-circumflex",     2300, 0x00e2,     0xe2, 0xC0,    CNULL,
		{
			 OOOO_ ,   OOO_ ,
			 _____ ,   ____ ,
			 _OOO_ ,   _OO_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   OOO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0343, "a-umlaut",         2400, 0x00e4,     0xe4, 0xCC,    CNULL,
		{
			 _O_O_ ,   O_O_ ,
			 _____ ,   ____ ,
			 _OOO_ ,   _OO_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   OOO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0344, "a-dot",            2600, 0x00e5,     0xe5, 0xD4,    CNULL,
		{
			 __O__ ,   _O__ ,
			 _____ ,   ____ ,
			 _OOO_ ,   _OO_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   OOO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0345, "c-acute",          2800, 0x0107,     0x00, 0x00,    CNULL,
		{
			 __OO_ ,   _OO_ ,
			 _____ ,   ____ ,
			 _OO__ ,   OOO_ ,
			 O____ ,   O___ ,
			 O____ ,   _OO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0346, "c-hook",           2900, 0x010d,     0x00, 0x00,    CNULL,
		{
			 OOOO_ ,   OOO_ ,
			 _____ ,   ____ ,
			 _OO__ ,   OOO_ ,
			 O____ ,   O___ ,
			 O____ ,   _OO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0347, "c-cedilla",        3000, 0x00e7,     0xe7, 0xB5,    CNULL,
		{
			 _____ ,   ____ ,
			 _OO__ ,   _OO_ ,
			 O____ ,   O___ ,
			 O____ ,   O___ ,
			 _OOO_ ,   _OO_ ,
			 OO___ ,   OO__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0350, "e-grave",          3400, 0x00e8,     0xe8, 0xC9,    CNULL,
		{
			 OO___ ,   OO__ ,
			 _____ ,   ____ ,
			 _OO__ ,   OOO_ ,
			 O_OO_ ,   OO__ ,
			 OO___ ,   _OO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0351, "e-acute",          3500, 0x00e9,     0xe9, 0xC5,    CNULL,
		{
			 __OO_ ,   _OO_ ,
			 _____ ,   ____ ,
			 _OO__ ,   OOO_ ,
			 O_OO_ ,   OO__ ,
			 OO___ ,   _OO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0352, "e-circumflex",     3600, 0x00ea,     0xea, 0xC1,    CNULL,
		{
			 OOOO_ ,   OOO_ ,
			 _____ ,   ____ ,
			 _OO__ ,   OOO_ ,
			 O_OO_ ,   OO__ ,
			 OO___ ,   _OO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0353, "e-trema",          3700, 0x00eb,     0xeb, 0xCD,    CNULL,
		{
			 O_O__ ,   O_O_ ,
			 _____ ,   ____ ,
			 _OO__ ,   OOO_ ,
			 O_OO_ ,   OO__ ,
			 OO___ ,   _OO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0354, "i-grave",          4300, 0x00ec,     0xec, 0xD9,    CNULL,
		{
			  OO__ ,   OO__ ,
			  ____ ,   ____ ,
			  OO__ ,   _O__ ,
			  _O__ ,   _O__ ,
			  _O__ ,   _OO_ ,
			  OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0355, "i-acute",          4400, 0x00ed,     0xed, 0xD5,    CNULL,
		{
			  _OO_ ,    OO_ ,
			  ____ ,    ___ ,
			  OO__ ,    O__ ,
			  _O__ ,    O__ ,
			  _O__ ,    OO_ ,
			  OOO_ ,    ___ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0356, "i-circumflex",     4500, 0x00ee,     0xee, 0xD1,    CNULL,
		{
			  OOO_ ,   OOO_ ,
			  ____ ,   ____ ,
			  OO__ ,   _O__ ,
			  _O__ ,   _O__ ,
			  _O__ ,   _OO_ ,
			  OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0357, "i-trema",          4600, 0x00ef,     0xef, 0xDD,    CNULL,
		{
			  O_O_ ,   O_O_ ,
			  ____ ,   ____ ,
			  OO__ ,   _O__ ,
			  _O__ ,   _O__ ,
			  _O__ ,   _OO_ ,
			  OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0360, "n-tilde",          5200, 0x00f1,     0xf1, 0xB7,    CNULL,
		{
			 OOOO_ ,   OOO_ ,
			 _____ ,   ____ ,
			 OOO__ ,   OO__ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0361, "o-grave",          5400, 0x00f2,     0xf2, 0xCA,    CNULL,
		{
			 OO___ ,   OO__ ,
			 _____ ,   ____ ,
			 _OO__ ,   OO__ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   _OO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0362, "o-acute",          5500, 0x00f3,     0xf3, 0xC6,    CNULL,
		{
			 __OO_ ,   _OO_ ,
			 _____ ,   ____ ,
			 _OO__ ,   OO__ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   _OO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0363, "o-circumflex",     5600, 0x00f4,     0xf4, 0xC2,    CNULL,
		{
			 OOOO_ ,   OOO_ ,
			 _____ ,   ____ ,
			 _OO__ ,   OO__ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   _OO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0364, "o-umlaut",         5700, 0x00f6,     0xf6, 0xCE,    CNULL,
		{
			 O__O_ ,   O_O_ ,
			 _____ ,   ____ ,
			 _OO__ ,   OO__ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   _OO_ ,
			 _OO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0365, "r-hook",           6200, 0x0159,     0x00, 0x00,    CNULL,
		{
			 OOOO_ ,   OOO_ ,
			 _____ ,   ____ ,
			 O_OO_ ,   _OO_ ,
			 OO___ ,   O___ ,
			 O____ ,   O___ ,
			 O____ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0366, "s-hook",           6400, 0x0161,     0x9a, 0xEC,    CNULL,
		{
			 _OOO_ ,   OOO_ ,
			 _____ ,   ____ ,
			 _OOO_ ,   _OO_ ,
			 _O___ ,   _O__ ,
			 __O__ ,   OO__ ,
			 OOO__ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0367, "sub-k",            4800, 0x2096,     0x00, 0x00,    CNULL,
		{
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  ____ ,   ____ ,
			  O_O_ ,   O_O_ ,
			  OO__ ,   OO__ ,
			  O_O_ ,   O_O_ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0370, "u-grave",          6800, 0x00f9,     0xf9, 0xCB,    CNULL,
		{
			 OO___ ,   OO__ ,
			 _____ ,   ____ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   _OO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0371, "u-acute",          6900, 0x00fa,     0xfa, 0xC7,    CNULL,
		{
			 __OO_ ,   _OO_ ,
			 _____ ,   ____ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   _OO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0372, "u-circumflex",     7000, 0x00fb,     0xfb, 0xC4,    CNULL,
		{
			 OOOO_ ,   OOO_ ,
			 _____ ,   ____ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   _OO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0373, "u-umlaut",         7100, 0x00fc,     0xfc, 0xCF,    CNULL,
		{
			 O__O_ ,   O_O_ ,
			 _____ ,   ____ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   _OO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0374, "u-dot",            7200, 0x016f,     0x00, 0x00,    CNULL,
		{
			 __O__ ,   _O__ ,
			 _____ ,   ____ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   O_O_ ,
			 O__O_ ,   _OO_ ,
			 _OOO_ ,   ____ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0375, "y-acute",          7900, 0x00fd,     0xfd, 0xB2,    CNULL,
		{
			 __OO_ ,   _OO_ ,
			 _____ ,   ____ ,
			 O__O_ ,   O_O_ ,
			 _O_O_ ,   OOO_ ,
			 __O__ ,   __O_ ,
			 OO___ ,   OO__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0376, "y-trema",          8100, 0x00ff,     0xff, 0xEF,    CNULL,
		{
			 O__O_ ,   O_O_ ,
			 _____ ,   ____ ,
			 O__O_ ,   O_O_ ,
			 _O_O_ ,   OOO_ ,
			 __O__ ,   __O_ ,
			 OO___ ,   OO__ ,
		}
	},

/*	  octal  name               sort  Unicode  Windows  Printer  UnicodeName */
	{ 0377, "z-hook",           8400, 0x017e,     0x9e, 0x00,    CNULL,
		{
			 OOO__ ,   OOO_ ,
			 _____ ,   ____ ,
			 OOO__ ,   OO__ ,
			 __O__ ,   _O__ ,
			 _O___ ,   _OO_ ,
			 _OOO_ ,   ____ ,
		}
	},


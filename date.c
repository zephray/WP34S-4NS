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

#include "date.h"
#include "decn.h"
#include "xeq.h"
#include "consts.h"
#include "alpha.h"

#ifdef REALBUILD
#include "atmel/rtc.h"
#else
#include <time.h>
#endif

/* Test if a date is Gregorian or Julian
 */
static int isGregorian(int year, int month, int day) {
	int y, m, d;
	if (UState.jg1582) {
		y = 1582;
		m = 10;
		d = 15;
	} else {
		y = 1752;
		m = 9;
		d = 14;
	}
	if (year > y)
		return 1;
	if (year < y)
		return 0;
	if (month < m)
		return 0;
	if (month > m)
		return 1;
	if (day < d)
		return 0;
	return 1;
}


/* Convert a date to a Julian day number
 */
static int JDN(int year, int month, int day) {
	const int a = (14 - month) / 12;
	const int y = year + 4800 - a;
	const int m = month + 12 * a - 3;
	int jdn;

	if (isGregorian(year, month, day))
		jdn = -y/100 + y/400 - 32045;
	else
		jdn = -32083;
	jdn += day + (153 * m + 2) / 5 + 365*y + y/4;
	return jdn;
}


/* Convert a julian day number to day, month and year.
 */
static void JDN2(int J, int *year, int *month, int *day) {
	int b, c, y, d, e, m;
	int jgthreshold = UState.jg1582 ? 2299161 : 2361222;

	if (J >= jgthreshold) {	// Gregorian
		const int a = J + 32044;
		b = (4*a+3)/146097;
		c = a - (b*146097)/4;
		b *= 100;
	} else {		// Julian
		b = 0;
		c = J + 32082;
	}

	d = (4*c+3)/1461;
	e = c - (1461*d)/4;
	m = (5*e+2)/153;

	*day   = e - (153*m+2)/5 + 1;
	*month = m + 3 - 12*(m/10);

	y = b + d - 4800 + m/10;
//	if (y <= 0)	y--;
	*year = y;
}


static int isleap(int year) {
	if ((year & 3) == 0) {		// leap year candidate
		if (isGregorian(year, 1, 1)) {
			// Gregorian is leap if not divisible by 100 or if devisible by 400
			if (year % 100 == 0) {
				if (year % 400 == 0)
					return 1;
				return 0;
			}
		}
		return 1;
	}
	return 0;
}


/* Validate that a date is in fact within our calculation range.
 */
static int check_date(int year, int month, int day) {
	int y, m, d;

	if (year < -4799 || year > 9999)
		return -1;
	JDN2(JDN(year, month, day), &y, &m, &d);
	if (year == y && month == m && day == d)
		return 0;
	return -1;
}


/* Day of week.
 * Display the day name and return the day as an integer (1 .. 7).
 */
static int day_of_week(int year, int month, int day, const char **msg) {
	int h;
	const char *const days = "\007\016\025\035\047\060\067"
				"Sunday\0Monday\0Tuesday\0Wednesday\0"
				"Thursday\0Friday\0Saturday";
	h = (JDN(year, month, day)+1) % 7;
	if (msg != NULL && ! Running)
		*msg = days + ((unsigned char *)days)[h];
	return h != 0 ? h : 7;
}


/* Convert a (y, m, d) triple into a decimal number that represents the date.
 * Some care is required for overflow and the like.
 */
static decNumber *build_date(decNumber *res, int year, int month, int day) {
	int sign = 1, shift = -6, r = 0;

	if (check_date(year, month, day)) {
		set_NaN(res);
		return res;
	}
	if (year < 0) {
		year = -year;
		sign = -1;
	}

	switch (UState.date_mode) {
	case DATE_YMD:
		r = ((year * 100) + month) * 100 + day;
		shift = -4;
		break;

	case DATE_DMY:
		r = ((day * 100) + month)* 10000 + year;
		break;

	case DATE_MDY:
		r = ((month * 100) + day)* 10000 + year;
		break;
	}
	int_to_dn(res, r * sign);
	dn_mulpow10(res, res, shift);
	day_of_week(year, month, day, &DispMsg);
	return res;
}

/*
 *  Convert Z = year, y = month, x = day to a valid date in X
 */
decNumber *dateFromYMD(decNumber *res, const decNumber *z, const decNumber *y, const decNumber *x) {
	if (decNumberIsSpecial(x) || decNumberIsSpecial(y) || decNumberIsSpecial(z))
		return set_NaN(res);
	else {
		return build_date(res, dn_to_int(z), dn_to_int(y), dn_to_int(x));
	}
}

/* Convert a decimal real to a date.
 * We have to honour the current date mode and make sure that things
 * don't go out of range.
 */
static int extract_date(const decNumber *x, int *year, int *month, int *day) {
	int ip, fp, y, m, d;
	decNumber z, a;

	if (is_intmode())
		return 1;

	if (decNumberIsNegative(x)) {
		dn_minus(&z, x);
	} else {
		decNumberCopy(&z, x);
	}
	decNumberTrunc(&a, &z);			// a = iii	z = iii.ffrrrr
	ip = dn_to_int(&a);
	dn_subtract(&a, &z, &a);		// a = .ffrrrr
	dn_mul100(&z, &a);			// z = ff.rrrr
	decNumberTrunc(&a, &z);			// a = ff
	fp = dn_to_int(&a);
	dn_subtract(&z, &z, &a);		// z = .rrrr
	switch (UState.date_mode) {
	default:
	case DATE_YMD:
		y = ip;
		m = fp;
		dn_mul100(&a, &z);
		decNumberTrunc(&z, &a);
		d = dn_to_int(&z);
		break;

	case DATE_DMY:
		d = ip;
		m = fp;
		goto year;

	case DATE_MDY:
		m = ip;
		d = fp;
year:		dn_mulpow10(&a, &z, 4);
		decNumberTrunc(&z, &a);
		y = dn_to_int(&z);
		break;
	}
	/* Make sense of things
	 */
	if (year != NULL)
		*year = y;
	if (month != NULL)
		*month = m;
	if (day != NULL)
		*day = d;
	return check_date(y, m, d);
}


/* Given an argument on the stack, attempt to determine the year.
 * If the argument is a plain integer, it is the year.
 * Otherwise, decode it according to the current date mode and
 * return the year.
 */
static int find_year(const decNumber *x, int *year) {
	int y;

	if (decNumberIsSpecial(x))
		return -1;
	if (is_int(x)) {
		y = dn_to_int(x);
		if (check_date(y, 1, 1))
			return -1;
	} else if (extract_date(x, &y, NULL, NULL))
		return -1;
	*year = y;
	return 0;
}


#ifdef INCLUDE_EASTER
/* Return Easter dates from the year.
 * We assume the calendar reformation took place in 1752 which means for this
 * year and before, we return the Julian date of Easter Sunday and afterwards
 * the Gregorian version.
 */
static void easter(int year, int *month, int *day) {
	if (year > (UState.jg1582 ? 1582 : 1752)) {	// Gregorian
		const int a = year % 19;
		const int b = year / 100;
		const int c = year % 100;
		const int d = b / 4;
		const int e = b % 4;
		const int f = (b + 8) / 25;
		const int g = (b - f + 1) / 3;
		const int h = (19 * a + b - d - g + 15) % 30;
		const int i = c / 4;
		const int k = c % 4;
		const int L = (32 + 2 * e + 2 * i - h - k) % 7;
		const int m = (a + 11 * h + 22 * L) / 451;
		*month = (h + L - 7 * m + 114) / 31;		// Jan = 1
		*day = ((h + L - 7 * m + 114) % 31) + 1;
	} else {
		const int a = year % 4;
		const int b = year % 7;
		const int c = year % 19;
		const int d = (19 * c + 15) % 30;
		const int e = (2 * a + 4 * b - d + 34) % 7;
		*month = (d + e + 114) / 31;			// Jan = 1
		*day = ((d + e + 114) % 31) + 1;
	}
}


/* Date of Easter.  Input is the year and output is a date.
 * The input can either be specified as an integer year or as a
 * valid date in the current format.
 */
decNumber *dateEaster(decNumber *res, const decNumber *x) {
	int y;

	if (find_year(x, &y)) {
		set_NaN(res);
		return res;
	} else {
		int m, d;

		easter(y, &m, &d);
		build_date(res, y, m, d);
	}
	return res;
}
#endif


/* Test if a year is a leap year
 */
void date_isleap(enum nilop op) {
	int y, t = 0;
	decNumber x;

	getX(&x);
	if (! find_year(&x, &y))
		t = isleap(y);
	fin_tst(t);
}


/* Return the day of the week
 */
static int dateExtract(decNumber *res, const decNumber *x, int *y, int *m, int *d) {
	if (decNumberIsSpecial(x) || extract_date(x, y, m, d)) {
		set_NaN(res);
		return 0;
	}
	return 1;
}

decNumber *dateDayOfWeek(decNumber *res, const decNumber *x) {
	int y, m, d;

	if (dateExtract(res, x, &y, &m, &d))
		int_to_dn(res, day_of_week(y, m, d, &DispMsg));
	return res;
}


/* Decompose dates into numerics
 */
decNumber *dateExtraction(decNumber *res, const decNumber *x) {
	int z[3];

	if (dateExtract(res, x, z, z+1, z+2))
		int_to_dn(res, z[argKIND(XeqOpCode) - OP_DATE_YEAR]);
	return res;
}


static void copy3(const char *p) {
	char buf[4];
	int i;

	for (i=0; i<3; i++)
		buf[i] = p[i];
	buf[3] = '\0';
	add_string(buf);
}

void date_alphaday(enum nilop op) {
	decNumber x;
	int y, m, d, dow;

	getX(&x);
	if (decNumberIsSpecial(&x) || extract_date(&x, &y, &m, &d))
		err(ERR_BAD_DATE);
	else {
		dow = day_of_week(y, m, d, NULL);
		copy3(&("MONTUEWEDTHUFRISATSUN"[3*(dow-1)]));
	}
}

void date_alphamonth(enum nilop op) {
	decNumber x;
	int y, m, d;
	static const char mons[12*3] = "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC";

	getX(&x);
	if (decNumberIsSpecial(&x) || extract_date(&x, &y, &m, &d))
		err(ERR_BAD_DATE);
	else {
		copy3(&(mons[3*m - 3]));
	}
}

/* Conversion routines from Julian days to and from dates
 */
decNumber *dateToJ(decNumber *res, const decNumber *x) {
	if (decNumberIsSpecial(x))
err:		set_NaN(res);
	else {
		int y, m, d;

		if (extract_date(x, &y, &m, &d))
			goto err;
		int_to_dn(res, JDN(y, m, d));
	}
	return res;
}

decNumber *dateFromJ(decNumber *res, const decNumber *x) {
	if (decNumberIsSpecial(x))
		set_NaN(res);
	else {
		const int j = dn_to_int(x);
		int y, m, d;

		JDN2(j, &y, &m, &d);
		return build_date(res, y, m, d);
	}
	return res;
}


/* Date and times to the Alpha register */
void date_alphadate(enum nilop op) {
	decNumber x;
	int d, m, y;
	char buf[16];
	char *p;

	getX(&x);
	xset(buf, '\0', sizeof(buf));
	if (extract_date(&x, &y, &m, &d)) {
		err(ERR_BAD_DATE);
		return;
	}
	switch (UState.date_mode) {
	default:
		p = num_arg(buf, y);
		*p++ = '-';
		p = num_arg_0(p, m, 2);
		*p++ = '-';
		num_arg_0(p, d, 2);
		break;

	case DATE_DMY:
		p = num_arg(buf, d);
		*p++ = '.';
		p = num_arg_0(p, m, 2);
		*p++ = '.';
		num_arg_0(p, y, 4);
		break;

	case DATE_MDY:
		p = num_arg(buf, m);
		*p++ = '/';
		p = num_arg_0(p, d, 2);
		*p++ ='/';
		num_arg_0(p, y, 4);
		break;
	}
	add_string(buf);
}

/*
 *  Time functions
 */
static int extract_time(int *h, int *m, int *s, int hour_threshold) {
	int hms;
	decNumber a, b;

	getX(&a);
	dn_mulpow10(&b, &a, 4);
	hms = dn_to_int(decNumberRound(&a, &b));
	*s = (hms % 100);
	hms /= 100;
	*m = (hms % 100);
	*h = hms / 100;
	if (hms < 0 || *h >= hour_threshold || *m >= 60 || *s >= 60) {
		err(ERR_BAD_DATE);
		return 1;
	}
	return 0;
}

void date_alphatime(enum nilop op) {
	char buf[16], *p;
	int h, m, s;
	const char *suffix;

	if (extract_time(&h, &m, &s, 100))
		return;
	xset(buf, '\0', sizeof(buf));
	if (UState.t12) {
		if (h >= 12) {
			h -= 12;
			suffix = " PM";
		} else
			suffix = " AM";
		if (h == 0)
			h = 12;
	} else
		suffix = "";
	p = num_arg(buf, h);
	*p++ = ':';
	p = num_arg_0(p, m, 2);
	*p++ = ':';
	p = num_arg_0(p, s, 2);
	scopy(p, suffix);
	add_string(buf);
}


/* These routines get the real time and day from the RTC */

static void query_time(unsigned int *s, unsigned int *m, unsigned int *h) {
#ifdef REALBUILD
	unsigned char hour, minute, second;
	
	RTC_GetTime(&hour, &minute, &second);
	*s = second;
	*m = minute;
	*h = hour;
#else
	time_t now = time(NULL);
	struct tm *dt = localtime(&now);

	*h = dt->tm_hour;
	*m = dt->tm_min;
	*s = dt->tm_sec;
#endif
}

static void query_date(unsigned int *d, unsigned int *m, unsigned int *y) {
#ifdef REALBUILD
	unsigned char day, month, dow;
	unsigned short year;
	
	RTC_GetDate( &year, &month, &day, &dow );

	*y = year;
	*m = month;
	*d = day;
#else
	time_t now = time(NULL);
	struct tm *dt = localtime(&now);

	*y = dt->tm_year + 1900;
	*m = dt->tm_mon + 1;
	*d = dt->tm_mday;
#endif
}


void date_date(enum nilop op) {
	if (Xtal) {
		unsigned int d, m, y;
		decNumber z;

		query_date(&d, &m, &y);
		build_date(&z, y, m, d);
		setX(&z);
	} else
		err(ERR_NO_CRYSTAL);
}

void date_time(enum nilop op) {
	if (Xtal) {
		unsigned int h, m, s;
		decNumber a, b;

		query_time(&s, &m, &h);
		h = (h * 100 + m) * 100 + s;
		int_to_dn(&a, h);
		dn_mulpow10(&b, &a, -4);
		setX(&b);
	} else
		err(ERR_NO_CRYSTAL);
}

void date_setdate(enum nilop op) {
	int d, m, y, dow;
	decNumber x;

	getX(&x);
	if (extract_date(&x, &y, &m, &d)) {
		err(ERR_BAD_DATE);
		return;
	}
	dow = day_of_week(y, m, d, NULL);
#ifdef REALBUILD
	if (Xtal) {
		busy();
		RTC_SetDate((unsigned short) y, (unsigned char) m,
			    (unsigned char) d, (unsigned char) dow);
	} else
		err(ERR_NO_CRYSTAL);
#else
	// So that very strict compilers (i.e. gcc4.6 do not complain that dow is unused with -Wall)
	(void) dow;
	err(ERR_ILLEGAL);
#endif
}

void date_settime(enum nilop op) {
	int h, m, s;
	if (extract_time(&h, &m, &s, 24))
		return;
#ifdef REALBUILD
	if (Xtal) {
		busy();
		RTC_SetTime((unsigned char) h, (unsigned char) m, (unsigned char) s);
	} else
		err(ERR_NO_CRYSTAL);
#else
	err(ERR_ILLEGAL);
#endif
}

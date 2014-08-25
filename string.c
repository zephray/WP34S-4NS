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


/* Some utilities to replace the various string and mem functions.
 * This one does something akin to memmove and memcpy.
 */
void *xcopy(void *d, const void *s, int n) {
        char *dp = d;
        const char *sp = s;

        if (sp > dp)
                while (n--)
                        *dp++ = *sp++;
        else if (sp < dp)
                while (n--)
                        dp[n] = sp[n];
        return d;
}

/* And a little something to set memory to a value.
 */
void *xset(void *d, const char c, int n) {
        char *dp = d;
        while (n--)
                *dp++ = c;
        return d;
}


#if defined(REALBUILD) && !defined(HOSTBUILD)
/* Needed by the C runtime */
#undef memcpy
#undef memset
void *memcpy() __attribute__((alias("xcopy")));
void *memset() __attribute__((alias("xset")));
#endif

/* Return the length of a string */
int slen(const char *s) {
        const char *p;

        for (p=s; *p != '\0'; p++);
        return p-s;
}

/* Find a character in a string -- strchr */
char *find_char(const char *s, const char c) {
        do
                if (*s == c)
                        return (char *)s;
        while (*s++);
        return NULL;
}

#if defined INCLUDE_YREG_CODE && defined INCLUDE_YREG_HMS
void replace_char(char *str, const char from, const char to) {
        while (*str) {
                if (*str == from) {
                        *str = to;
                }
                ++str;
        }
}
#endif

/* Copy a string across and return a pointer to the terminating null
 */
char *scopy(char *d, const char *s) {
        while (*s != '\0')
                *d++ = *s++;
        *d = '\0';
        return d;
}


/* Copy a string to a given size limit and null terminate
 */
const char *sncopy(char *d, const char *s, int n) {
        const char *const d0 = d;

        while (n-- && *s != '\0')
                *d++ = *s++;
        *d = '\0';
        return d0;
}


/* Copy a string to the buffer and append a character.
 */
char *scopy_char(char *d, const char *s, const char c) {
        d = scopy(d, s);
        *d++ = c;
        return d;
}

char *scopy_spc(char *d, const char *s) {
        return scopy_char(d, s, ' ');
}

char *sncopy_char(char *d, const char *s, int n, const char c) {
        while (n-- && *s != '\0')
                *d++ = *s++;
        *d++ = c;
        return d;
}

char *sncopy_spc(char *d, const char *s, int n) {
        return sncopy_char(d, s, n, ' ');
}

/* Convert an n digit number to a string with leading zeros
 */
char *num_arg_0(char *d, unsigned int arg, int n) {
        int i;

        for (i=0; i<n; i++) {
                d[n-i-1] = '0' + arg % 10;
                arg /= 10;
        }
        return d + n;
}

char *num_arg(char *d, unsigned int arg) {
        char buf[24];
        char *p = buf;

        do {
                *p++ = '0' + arg % 10;
                arg /= 10;
        } while (arg != 0);

        while (--p >= buf)
                *d++ = *p;
        return d;
}

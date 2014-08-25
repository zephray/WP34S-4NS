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

/* Define some alternate code points for characters */

const struct _altuni {
	int code;
	int alternate;
} AltUni[] = {
	{ '"',  0x201c },
	{ '"',  0x201d },
	{ '"',  0x201e },
	{ '\'', 0x2018 },
	{ '\'', 0x2019 },
	{ '\'', 0x201a },
	{ 'A',  0x0391 },
	{ 'B',  0x0392 },
	{ 'E',  0x0395 },
	{ 'Z',  0x0396 },
	{ 'H',  0x0397 },
	{ 'I',  0x0399 },
	{ 'K',  0x039a },
	{ 'M',  0x039c },
	{ 'N',  0x039d },
	{ 'O',  0x039f },
	{ 'P',  0x03a1 },
	{ 'T',  0x03a4 },
	{ 'Y',  0x03a5 },
	{ 'X',  0x03a7 },
	{ 'o',  0x03bf },
	{ '|',  0x00a6 },
	{ 0302, 0x00c3 }, // ?
	{ 0302, 0x0100 },
	{ 0302, 0x0102 },
	{ 0312, 0x0112 }, // ?
	{ 0312, 0x0114 },
	{ 0312, 0x011a },
	{ 0316, 0x0128 }, // ?
	{ 0316, 0x012a },
	{ 0316, 0x012c },
	{ 0320, 0x0147 }, // ~N
	{ 0323, 0x00d5 }, // ?
	{ 0323, 0x014c },
	{ 0323, 0x014e },
	{ 0332, 0x0168 }, // ?
	{ 0332, 0x016a },
	{ 0332, 0x016c },
	{ 0342, 0x00e3 }, // ?
	{ 0342, 0x0101 },
	{ 0342, 0x0103 },
	{ 0352, 0x0113 }, // ?
	{ 0352, 0x0115 },
	{ 0352, 0x011b },
	{ 0356, 0x0129 }, // ?
	{ 0356, 0x012b },
	{ 0356, 0x012d },
	{ 0360, 0x0148 }, // ~n
	{ 0363, 0x00f5 }, // ?
	{ 0363, 0x014d },
	{ 0363, 0x014f },
	{ 0372, 0x0169 }, // ?
	{ 0372, 0x016b },
	{ 0372, 0x016d },
	{ 0xab, 0x00b5 }, // ?
	{ 0 }
};
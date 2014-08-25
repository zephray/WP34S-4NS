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

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "xeq.h"

// Special return values
#define R_TIMEOUT (-1)
#define R_ERROR (-2)
#define R_BREAK (-3)

// Global flags
extern char SerialOn;

// User visible routines
extern void send_program( enum nilop op );
extern void send_registers( enum nilop op );
extern void send_sigma( enum nilop op );
extern void send_all( enum nilop op );
extern void recv_any( enum nilop op );
extern int recv_byte( int timeout );
#ifdef INCLUDE_USER_IO
extern void send_byte( REGISTER *nul1, REGISTER *nul, enum nilop op2 );
extern void serial_open( REGISTER *byte, REGISTER *nul2, enum nilop op );
extern void serial_close( REGISTER *byte, REGISTER *nul2, enum nilop op );
extern void send_alpha( REGISTER *nul1, REGISTER *nul, enum nilop op2 );
extern void recv_alpha( REGISTER *nul1, REGISTER *nul, enum nilop op2 );
#endif

// Helpers
extern int open_port_default( void );
extern void close_port_reset_state( void );

// Call-back for a received byte with error information
extern int byte_received( short byte );

// Implemented by the hardware layer
extern int open_port( int baud, int bits, int parity, int stopbits );
extern void close_port( void );
extern void put_byte( unsigned char byte );
extern void flush_comm( void );

#endif

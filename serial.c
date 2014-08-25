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

#include "serial.h"
#include "xeq.h"
#include "storage.h"
#include "display.h"
#include "lcd.h"
#include "stats.h"

#define STX 2
#define ETX 3
#define ENQ 5
#define ACK 6
#define NAK 0x15
#define MAXCONNECT 10
#define CHARTIME 30

#define IN_BUFF_LEN 32
#define IN_BUFF_MASK 0x1f
#define DATA_LEN 2048

#define TAG_PROGRAM  0x5250 // "PR"
#define TAG_REGISTER 0x4552 // "RE"
#define TAG_SIGMA    0x4D53 // "SM"
#define TAG_ALLMEM   0x4C41 // "AL"

#define SERIAL_ANNUNCIATOR LIT_EQ

#if defined(QTGUI) || defined(IOS)
extern void serial_lock(void);
extern void serial_unlock(void);
#undef lock
#undef unlock
#define lock() serial_lock()
#define unlock() serial_unlock()
#endif

/*
 *  Flags and hardware buffer for received data
 */
short InBuffer[ IN_BUFF_LEN ];
volatile char InRead, InWrite, InCount;
char SerialOn;

/*
 *  Handle the flag and the annunciator
 */
static void serial_state( int state )
{
	SerialOn = state;
#ifdef SERIAL_ANNUNCIATOR
	dot( SERIAL_ANNUNCIATOR, state );
	finish_display();
#endif
}


/*
 * Receive a single byte from the serial port and return the
 * value in X.
 * If the transfer times out or an error is detected return a negative error code.
 */
int recv_byte( int timeout )
{
	int byte;
	unsigned int ticks = Ticker + timeout;

	flush_comm();
	while ( InCount == 0 && ( timeout < 0 || Ticker < ticks ) ) {
		/*
		 *  No bytes in buffer
		 */
		busy();
		idle();
	}
	if ( InCount == 0 ) {
		return R_TIMEOUT;
	}
	lock();
	byte = InBuffer[ (int) InRead ];
	InRead = ( InRead + 1 ) & IN_BUFF_MASK;
	--InCount;
	unlock();
	return byte;
}


/*
 *  Receive a byte with fixed timeout value
 */
static int get_byte( void )
{
	return recv_byte( CHARTIME );
}


/*
 *  Add a received byte to the buffer, called from interrupt.
 */
int byte_received( short byte )
{
	if ( InCount == IN_BUFF_LEN ) {
		/*
		 *  Sorry, no room
		 */
#ifdef REALBUILD
		byte = R_ERROR;
		InWrite = ( InWrite - 1 ) & IN_BUFF_MASK;
		--InCount;
#else
		return 1;
#endif
	}
#if defined(QTGUI) || defined(IOS)
	lock();
#endif
	InBuffer[ (int) InWrite ] = byte;
	InWrite = ( InWrite + 1 ) & IN_BUFF_MASK;
	++InCount;
#if defined(QTGUI) || defined(IOS)
	unlock();
#endif
	return 0;
}


/*
 *  Get rid of any stray input data
 */
static void clear_buffer( void )
{
	lock();
	InRead = InWrite = InCount = 0;
	unlock();
}


/*
 *  Open port with default settings.
 *  Returns non zero in case of failure.
 */
void close_port_reset_state( void )
{
	if ( SerialOn ) {
		flush_comm();
		close_port();
		serial_state( 0 );
		clear_buffer();
	}
}


/*
 *  Open port with default settings.
 *  Returns non zero in case of failure.
 *  On a machine without a crystal the real speed of the device
 *  compared to the nominal speed may be specified as a percentage in X.
 *  For a slow device, try 90, for a fast device try 110.
 */
int open_port_default( void )
{
	int baud = 9600;
#if defined(REALBUILD) && !defined(XTAL)
	if ( !Xtal ) {
		int factor = (int) get_reg_n_int( regX_idx );
		if ( factor >= 80 && factor <= 120 ) {
			baud = baud * 100 / factor;
		}
	}
#endif
	close_port_reset_state();
	if ( open_port( baud, 8, 'N', 1 ) ) {
		return 1;
	}
	serial_state( 1 );
	return 0;
}



/*
 *  Connect to partner.
 *  Opens the port and sends ENQ until ACK is received.
 *  Returns non zero in case of failure.
 */
static int connect( void )
{
	int c, i = MAXCONNECT;

	if ( open_port_default() ) return 1;
	do {
		put_byte( ENQ );
		c = get_byte();
	} while ( c != ACK && c != R_BREAK && --i );
	if ( c != ACK ) {
		close_port_reset_state();
		return 1;
	}
	return 0;
}


/*
 *  Accept connection from partner.
 *  Opens the port and waits for ENQ.
 *  Returns non zero in case of failure.
 */
static int accept_connection( void )
{
	int i = MAXCONNECT;

	if ( open_port_default() ) return 1;
	while ( i-- ) {
		int c = get_byte();
		if ( c == ENQ ) {
			clear_buffer();
			put_byte( ACK );
			flush_comm();
			return 0;
		}
		if ( c == R_BREAK ) break;
	}
	close_port_reset_state();
	return 1;
}


/*
 *  Transmit a 16 bit word lsb first
 */
static void put_word( unsigned short w )
{
	put_byte( (unsigned char) w );
	put_byte( (unsigned char) ( w >> 8 ) );
}


/*
 *  Receive a 16 bit word. Negative return values are errors.
 */
static int get_word( void )
{
	int cl;
	int ch;

	cl = get_byte();
	if ( cl < 0 ) return cl;
	ch = get_byte();
	if ( ch < 0 ) return ch;
	return cl | ( ch << 8 );
}


/*
 *  Transmits block of data to the serial port.
 *  Returns non zero in case of error.
 *
 *  The protocol is as follows:
 *    Connect (Send ENQ, wait for ACK, see above)
 *    Send tag (2 bytes)
 *    Send length (16 bit, lsb first)
 *    Send CRC ^ tag (16 bit, lsb first)
 *    Send data
 *    Send ETX
 *    Wait for ACK
 *
 *    If a NAK is received while sending the transfer is aborted.
 */
static void put_block( unsigned short tag, unsigned short length, const void *data )
{
	const unsigned short crc = crc16( data, length ) ^ tag;
	unsigned char *p = (unsigned char *) data;
	int ret = connect();
	int c;

	if ( ret == 0 ) {
		/*
		 *  We are connected, send data
		 */
		put_byte( STX );
		put_word( tag );
		put_word( length );
		put_word( crc );
		while ( length-- && ret == 0 ) {
			busy();
			put_byte( *p++ );
			if ( (char) length == 0 ) {
				c = recv_byte( 0 );
				ret = ( c == NAK || c == R_BREAK );
			}
		}
		clear_buffer();
		put_byte( ETX );
		if ( ret || ACK != recv_byte( 5 * CHARTIME ) ) {
			ret = 1;
		}
		close_port_reset_state();
	}
	if ( ret ) {
		err( ERR_IO );
	}
	else {
		DispMsg = "OK";
	}
	return;
}

/*
 *  Receive block from the serial port and validate the checksum.
 *  If the checksum doesn't match, set error condition.
 *  Depending on the tag received, copy the data to its destination.
 *  This implements the RECV command.
 */
void recv_any( enum nilop op )
{
	int i, c;
	unsigned char buffer[ DATA_LEN ];
	int tag, length, crc;
	void *dest;

	if ( not_running() ) {
		/*
		 *  Only allowed as a direct command
		 */
		if ( accept_connection() ) {
			err( ERR_IO );
			return;
		}
		for ( i = 0; i < MAXCONNECT; ++i ) {
			c = get_byte();
			if ( c == STX ) break;
		}
		if ( c != STX ) {
			err( ERR_IO );
			return;
		}

		tag = get_word();

		if ( tag < 0 ) goto err;

		length = get_word();
		if ( length < 0 || length > DATA_LEN ) goto err;

		crc = get_word();
		if ( crc < 0 ) goto err;

		for ( i = 0; i < length; ++i ) {
			c = get_byte();
			if ( c < 0 ) goto err;
			buffer[ i ] = c;
		}
		c = get_byte();
		if ( c != ETX ) goto err;

		if ( crc != ( crc16( buffer, length ) ^ tag ) ) goto err;

		/*
		 *  Check the tag value and copy the data if valid
		 */
		switch ( tag ) {

		case TAG_PROGRAM:
			/*
			 *  Program received
			 */
			if ( append_program( (s_opcode *) buffer, length >> 1 ) ) {
				/*
				 *  Not enough memory
				 */
				goto nak;
			}
			DispMsg = "Program";
			goto done;

		case TAG_ALLMEM:
			/*
			 *  All memory received
			 */
			if ( length != sizeof( PersistentRam ) ) {
				  goto invalid;
			}
			dest = &PersistentRam;
			DispMsg = "All RAM";
			break;

		case TAG_REGISTER:
			/*
			 *  Registers received
			 */
			if ( (length >> 3) > NumRegs ) {
				  goto invalid;
			}
			dest = get_reg_n(0);
			DispMsg = "Register";
			break;

		case TAG_SIGMA:
			/*
			 *  Summation registers received
			 */
			if ( length != sizeof( STAT_DATA ) ) {
				  goto invalid;
			}
			if ( sigmaCopy( buffer ) ) {
				goto nak;
			}
			DispMsg = "\221 Regs";
			goto done;

		default:
			goto invalid;
		}

		/*
		 *  Copy the data and recompute the checksums
		 */
		xcopy( dest, buffer, length );
	done:
		checksum_all();

		/*
		 *  All is well
		 */
		c = ACK;
		goto close;

		/*
		 *  Various error conditions
		 */
	invalid:
		err( ERR_INVALID );
		goto nak;
	err:
		err( ERR_IO );
	nak:
		c = NAK;

	close:
		/*
		 *  Send reply to partner
		 */
		put_byte( c );
		close_port_reset_state();
	}
}


/*
 * Transmit the current program to the serial port.
 */
void send_program( enum nilop op )
{
	update_program_bounds( 1 );
	put_block( TAG_PROGRAM, (ProgEnd - ProgBegin + 1) * sizeof( s_opcode ), get_current_prog() );
}


/*
 * Send registers 00 through 99 or the configured maximum to the serial port.
 */
void send_registers( enum nilop op )
{
	put_block( TAG_REGISTER, NumRegs << 3, get_reg_n( 0 ) );
}


/*
 * Send statistical summation data
 */
void send_sigma( enum nilop op )
{
	if ( !sigmaCheck() ) {
		put_block( TAG_SIGMA, sizeof( STAT_DATA ), StatRegs );
	}
}


/*
 * Send all of RAM to the serial port.  2kb in total.
 */
void send_all( enum nilop op )
{
	put_block( TAG_ALLMEM, sizeof( PersistentRam ), &PersistentRam );
}


#ifdef INCLUDE_USER_IO
/*
 *  Open the serial port from user code.
 *  Alpha is interpreted as follows:
 *    baudrate,format
 *    - The delimiter may be any non digit.
 *    - The format is a combination of '1', '2', '7', '8', 'E', 'O' or 'N'
 *    - Any other characters are skipped and ignored.
 *    - Default: 9600,8N1
 */
void serial_open( enum nilop op )
{
	int baud = 9600;
	char bits = 8;
	char parity = 'N';
	char stop = 1;
	char c, *p = Alpha;

	close_port_reset_state();
	if ( *p != '\0' ) {
		/*
		 *  Alpha is set, parse it
		 */
		baud = 0;
		while ( ( c = *p++ ) && c >= '0' && c <= '9' ) {
			baud = baud * 10 + ( c & 0xf );
		}
		while ( ( c = *p++ ) ) {
			if ( c == '7' || c == '8' ) {
				bits = c & 0xf;
			}
			else if ( c == '1' || c == '2' ) {
				stop = c & 0xf;
			}
			else if ( c == 'E' || c == 'N' || c == 'O' ) {
				parity = c;
			}
		}
	}

	/*
	 *  Set up the port
	 */
	if ( open_port( baud, bits, parity, stop ) ) {
		err( ERR_INVALID );
		return;
	}
	serial_state( 1 );
	return;
}


 /*
  * Close the serial port from user code
  */
void serial_close( enum nilop op )
{
	close_port_reset_state();
}


/*
 *  Open with default parameters if not already opened by SOPEN
 */
static int serial_open_default( void )
{
	if ( !SerialOn && open_port_default() ) {
		err( ERR_IO );
		return 1;
	}
	return 0;
}


/*
 * Send a single byte as specified in X to the serial port.
 */
void send_byte( enum nilop op )
{
	int sgn;
	const unsigned char byte = getX_int_sgn( &sgn ) & 0xff;

	if ( !serial_open_default() ) {
		put_byte( byte );
		flush_comm();
	}
}


/*
 *  Send the contents of the Alpha register, terminated by a CR
 */
void send_alpha( enum nilop op )
{
	const char *p;
	if ( !serial_open_default() ) {
		for ( p = Alpha; *p != '\0'; p++ ) {
			put_byte(*p);
		}
		put_byte( '\r' );
		flush_comm();
	}
}


/*
 *  Receive the contents of the alpha register.
 *  CR stops the reception.
 */
void recv_alpha( enum nilop op )
{
	int i, c = 0;
	int timeout = (int) getX_int_sgn( &i );

	if ( i || timeout < 0 ) timeout = -1;
	
	if ( !serial_open_default() ) {
		i = 0;
		while ( i < NUMALPHA ) {
			c = recv_byte( i == 0 || timeout < CHARTIME ? timeout : CHARTIME );
			if ( c == '\r' || c < 0 ) break;
			Alpha[ i++ ] = c;
		}
		Alpha[ i ] = '\0';
		if ( c < 0 ) {
			err( ERR_IO );
		}
	}
}

#endif



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

#include "decn.h"
#include "xeq.h"
#include "consts.h"
#include "alpha.h"
#include "lcd.h"
#include "keys.h"
#include "stats.h"
#include "stopwatch.h"

#ifdef INCLUDE_STOPWATCH
#ifndef REALBUILD
#if defined(WIN32) && !defined(QTGUI) && !defined(IOS) && !defined(__GNUC__)
#include "win32.h"
#else
#include <sys/time.h>
#endif
#endif // REALBUILD
#include "display.h"
#include "keys.h"
#endif // INCLUDE_STOPWATCH

#ifdef REALBUILD
#include "atmel/rtc.h"
#else
#include <time.h>
#endif

#ifdef INCLUDE_STOPWATCH

#define STOPWATCH_MESSAGE "STOPWATCH"
#define StopWatchKeyticks         (StateWhileOn._keyticks)
#define STOPWATCH_APD_TICKS 65535 // Largest unsigned short possible in 32 bits. 1 hour 49 min

TStopWatchStatus StopWatchStatus; // ={ 0, 1, 0, 0, };

/*
 *  KeyCallback is used to call the StopWatch from the main loop
 * And set to NULL when the StopWatch is not running
 */
int (*KeyCallback)(int)=(int (*)(int)) NULL;

/*
 *  Stopwatch uses the ticker count. This is the starting point
 */
unsigned long FirstTicker;

/*
 * When resetting after a Sigma+ or a RoundTime storage, we original FirstTicker
 * here so we can compute TotalStopWatch the exact same way as StopWatch
 */
unsigned long TotalFirstTicker;

/*
 * Current total stopwatch value, in ticker count. Usually set to getTicker() - TotalFirstTicker
 */
unsigned long TotalStopWatch;

/*
 * Current stopwatch value, in ticker count. Usually set to getTicker() - FirstTicker
 */
unsigned long StopWatch;

/*
 * Index on memory to store split time
 */
unsigned char StopWatchMemory;

/*
 * Used to choose a memory index to store split time in
 */
signed char StopWatchMemoryFirstDigit;
signed char RclMemory;

/*
 * Use to display the chosen memory for a while
 */
unsigned char RclMemoryRemanentDisplay;

#define STOPWATCH_RS K63
#define STOPWATCH_EXIT K60
#define STOPWATCH_CLEAR K24
#define STOPWATCH_EEX K23
#define STOPWATCH_STORE K20
#define STOPWATCH_STORE_ROUND K62
#define STOPWATCH_UP K40
#define STOPWATCH_DOWN K50
#define STOPWATCH_SHOW_MEMORY K04
#define STOPWATCH_RCL K11
#define STOPWATCH_SIGMA_PLUS K00
#define STOPWATCH_SIGMA_PLUS_STORE_ROUND K64

#define RCL_MEMORY_REMANENCE 3

#ifdef REALBUILD
extern volatile SMALL_INT SpeedSetting;
extern void set_speed( unsigned int speed );
#define SPEED_HIGH     4
#endif
/*
 * Current ticker count. Or its emulation when not running on WP34s hardware
 */
unsigned long getTicker() {
#if defined(CONSOLE) || defined(IOS)
    struct timeval tv;
    gettimeofday(&tv,(struct timezone*) NULL);
    return tv.tv_sec * 10 + tv.tv_usec / 100000;
#else
    return Ticker;
#endif
}

/*
 * Displays the memory index, even if partially entered
 */
static void fill_exponent(char* exponent) {
	if(StopWatchStatus.select_memory_mode) {
		if(StopWatchMemoryFirstDigit>=0) {
			exponent[0]='0'+StopWatchMemoryFirstDigit;
		} else {
			exponent[0]='_';
		}
		exponent[1]='_';
	} else {
		num_arg_0(exponent, StopWatchMemory, 2);
	}
	exponent[2]=0;
}

/*
 * Converts a ticker count to a String
 */
static void stopwatch_to_string(unsigned long stopwatch, char* p) {
	int tenths, secs, mins, hours;

	tenths=stopwatch%10;
	secs=(stopwatch/10)%60;
	mins=(stopwatch/600)%60;
	hours=(stopwatch/36000)%100;
	p=num_arg_0(p, hours, 2);
	*p++='h';
	p=num_arg_0(p, mins, 2);
	*p++='\'';
	p=num_arg_0(p, secs, 2);
	*p++='"';
	if(StopWatchStatus.display_tenths) {
		*p++=' ';
		p=num_arg_0(p, tenths, 1);
	}
	*p=0;
}

/*
 * Displaying the stopwatch value in HHhMM'SS"t format
 */
static void display_stopwatch() {
	char buf[13];
	char exponent[3];
	char message[13];
	char display_rcl_message;
	
	fill_exponent(exponent);
	stopwatch_to_string(StopWatch, buf);
	display_rcl_message=StopWatchStatus.rcl_mode || StopWatchStatus.sigma_display_mode || RclMemory>=0;
	if(display_rcl_message) {
		char* rp=scopy(message, StopWatchStatus.sigma_display_mode?"\221+\006":"RCL\006\006");
		if(StopWatchStatus.sigma_display_mode) {
			rp=num_arg(rp, RclMemory);
		}
		else {
			if(RclMemory>=0) {
				rp=num_arg_0(rp, RclMemory, 2);
			}
			else {
				if(StopWatchMemoryFirstDigit>=0) {
					*rp++='0'+StopWatchMemoryFirstDigit;
				}
				*rp++='_';
			}
		}
		*rp=0;
	}
	else if(TotalFirstTicker>0) {
		stopwatch_to_string(TotalStopWatch, message);
		display_rcl_message=1;
	}
	stopwatch_message(display_rcl_message?message:STOPWATCH_MESSAGE, buf, StopWatchStatus.sigma_display_mode, StopWatchStatus.show_memory?exponent:(char*)NULL);
}

/*
 * Convert StopWatch current value to a decNumber
 */
static void stopwatch_to_decNumber(decNumber* number) {
	ullint_to_dn(number, StopWatch);
	dn_divide(number, number, &const_36000);
}

/*
 * Storing a split time in memory
 */
static void store_stopwatch_in_memory() {
	int max_registers=global_regs();
	if(max_registers>0) {
		decNumber s, hms;
		StopWatchStatus.show_memory=1;
		// Converting tick count to HMS format
		stopwatch_to_decNumber(&s);
		decNumberHR2HMS(&hms, &s);
		setRegister(StopWatchMemory, &hms);
		StopWatchMemory=(StopWatchMemory+1)%max_registers;
	}
}

/*
 * What is the digit for this key?
 */
static int get_digit(int key) {
	int digit=keycode_to_digit_or_register(key);
	return digit>=0 && digit<=9 && global_regs() > 0 ? digit : -1;
}

/*
 * R/S has been pressed
 */
static void toggle_running() {
	StopWatchStatus.running=!StopWatchStatus.running;
	if(StopWatchStatus.running) {
		if(FirstTicker==0) {
			FirstTicker=getTicker();
		}
		else {
			FirstTicker=getTicker()-StopWatch;
		}
	}
}

/*
 * Recalling a stored split time and converting it for display
 */
static void recall_memory(int index) {
	decNumber memory, s, hms;
	unsigned long previous;
	int sign;

	StopWatchMemoryFirstDigit=-1;
	RclMemory=index;
	StopWatchStatus.rcl_mode=0;
	StopWatchStatus.sigma_display_mode=0;
	RclMemoryRemanentDisplay=0;
	getRegister(&memory, index);
	// Converting HMS format to tick count
	decNumberHMS2HR(&hms, &memory);
	dn_multiply(&s, &hms, &const_36000);
	previous=(unsigned long) dn_to_ull(&s, &sign);

	FirstTicker=getTicker()-previous;
	StopWatch=previous;
}

/*
 * Once a memory has been chosen, we either store in it or recall it
 */
static void end_memory_selection(int index) {
	if(StopWatchStatus.rcl_mode){
		recall_memory(index);
	}
	else {
		StopWatchMemory=index;
		StopWatchMemoryFirstDigit=-1;
		StopWatchStatus.select_memory_mode=0;
	}
}

/*
 * Handling the selection of a memory index
 */
static void process_select_memory_key(int key) {
	int digit = get_digit(key);
	switch(key)	{
			case STOPWATCH_RS: {
				toggle_running();
				StopWatchStatus.select_memory_mode=0;
				StopWatchStatus.rcl_mode=0;
				StopWatchStatus.sigma_display_mode=0;
				break;
			}
			case STOPWATCH_EXIT: {
				StopWatchStatus.select_memory_mode=0;
				StopWatchStatus.rcl_mode=0;
				StopWatchStatus.sigma_display_mode=0;
				break;
			}
			case STOPWATCH_CLEAR: {
				if(StopWatchMemoryFirstDigit<0) {
					StopWatchStatus.select_memory_mode=0;
					StopWatchStatus.rcl_mode=0;
					StopWatchStatus.sigma_display_mode=0;
				} else {
					StopWatchMemoryFirstDigit=-1;
				}
				break;
			   }
			case STOPWATCH_STORE: {
				if(StopWatchMemoryFirstDigit>=0) {
					end_memory_selection(StopWatchMemoryFirstDigit);
				}
				break;
			}
			default: {
				if (digit >= 0) {
					int max_registers=global_regs();
					// Digits
					if(StopWatchMemoryFirstDigit<0) {
						if(digit<=max_registers/10) {
							StopWatchMemoryFirstDigit=digit;
						} else if(max_registers<=10 && digit<max_registers) {
							end_memory_selection(digit);
						}
					} else {
					int index=StopWatchMemoryFirstDigit*10+digit;
					if(index<max_registers) {
						end_memory_selection(index);
					}
					}
				}
				break;
			  }
	}
}

static void stopwatch_sigma_plus() {
	decNumber s;
#ifdef REALBUILD
	SMALL_INT oldSpeed=SpeedSetting;
	set_speed(SPEED_HIGH);
#endif
	stopwatch_to_decNumber(&s);
	RclMemory=sigma_plus_x(&s);
	if(RclMemory>=0) {
		StopWatchStatus.rcl_mode=0;
		StopWatchStatus.sigma_display_mode=1;
		RclMemoryRemanentDisplay=0;
		if(TotalFirstTicker==0) {
			TotalFirstTicker=FirstTicker;
		}
		FirstTicker=getTicker();
		// StopWatch=0 is need to avoid a small display glitch
		// in full time
		StopWatch=0;
	}
#ifdef REALBUILD
	set_speed(oldSpeed);
#endif
}

/*
 * Handling keys.As we need to be a 'special mode', we have to do it ourselves
 * We cannot reuse the normal main loop code
 */
static void process_stopwatch_key(int key) {
	int max_registers=global_regs();
	switch(key)	{
			case STOPWATCH_RS: {
				toggle_running();
				break;
			}
			case STOPWATCH_EXIT: {
				KeyCallback=(int(*)(int))NULL;
				break;
			}
			case STOPWATCH_CLEAR: {
				if(StopWatchStatus.running)	{
					if(TotalFirstTicker==0) {
						TotalFirstTicker=FirstTicker;
					}
					FirstTicker=getTicker();
				} else {
					TotalFirstTicker=0;
					FirstTicker=0;
				}
				StopWatch=0;
				break;
			   }
			case STOPWATCH_EEX: {
				StopWatchStatus.display_tenths=!StopWatchStatus.display_tenths;
				break;
				}
			case STOPWATCH_STORE: {
				store_stopwatch_in_memory();
				break;
				}
			case STOPWATCH_STORE_ROUND: {
				store_stopwatch_in_memory();
				if(TotalFirstTicker==0) {
					TotalFirstTicker=FirstTicker;
				}
				FirstTicker=getTicker();
				break;
				}
			case STOPWATCH_SIGMA_PLUS_STORE_ROUND: {
				store_stopwatch_in_memory();
				stopwatch_sigma_plus();
				break;
			}
			case STOPWATCH_SIGMA_PLUS: {
				stopwatch_sigma_plus();
				break;
			}
			case STOPWATCH_UP: {
				if(StopWatchMemory<max_registers-1) {
					StopWatchMemory++;
				}
				break;
				}
			case STOPWATCH_DOWN: {
				if(StopWatchMemory>0) {
					StopWatchMemory--;
				}
				break;
				}
			case STOPWATCH_SHOW_MEMORY: {
				StopWatchStatus.show_memory=!StopWatchStatus.show_memory;
				break;
				}
			case STOPWATCH_RCL: {
				StopWatchStatus.rcl_mode=max_registers>0;
				StopWatchStatus.sigma_display_mode=0;
				StopWatchMemoryFirstDigit=-1;
				break;
				}
			default: {
				if (get_digit(key) >= 0) {
					//Digits
					StopWatchStatus.select_memory_mode=1;
					StopWatchMemoryFirstDigit=-1;
					StopWatchStatus.show_memory=1;
					process_select_memory_key(key);
				}
				break;
			  }
			}
}

/*
 * Called by the main loop to increment stopwatch or process a pressed key
 */
int stopwatch_callback(int key) {
	unsigned long currentTicker;
	if(StopWatchStatus.running)	{
		currentTicker=getTicker();
		StopWatch=currentTicker-FirstTicker;
		TotalStopWatch=currentTicker-TotalFirstTicker;
		StopWatchKeyticks=0;
	} else if(StopWatchKeyticks >= STOPWATCH_APD_TICKS) {
		KeyCallback=(int(*)(int)) NULL;
		return K_HEARTBEAT;
	}

	if(key!=K_HEARTBEAT && key!=K_RELEASE) {
		StopWatchKeyticks=0;
		if(StopWatchStatus.select_memory_mode || StopWatchStatus.rcl_mode) {
			process_select_memory_key(key);
		} else {
			process_stopwatch_key(key);
		}
	}
	display_stopwatch();
	if(RclMemory>=0 && RclMemoryRemanentDisplay++ > RCL_MEMORY_REMANENCE) {
		StopWatchStatus.sigma_display_mode=0;
		RclMemory=-1;
	}

	return K_HEARTBEAT;
}

/*
 * Entering stopwach mode. If we were already running, we continue
 * If not, we set variables to their initial values
 */
void stopwatch(enum nilop op) {
#ifndef REALBUILD
	// Should never happen
	if (Running) {
		err(ERR_ILLEGAL);
		return;
	}
#endif
	if(!StopWatchRunning) {
		StopWatchStatus.show_memory=0;
		StopWatchStatus.display_tenths=1;
		StopWatchMemory=0;
		RclMemory=-1;
		StopWatch=0;
		FirstTicker=0;
		TotalStopWatch=0;
		TotalFirstTicker=0;
	}
	clr_dot(LIT_EQ);
	StopWatchStatus.select_memory_mode=0;
	StopWatchStatus.rcl_mode=0;
	StopWatchStatus.sigma_display_mode=0;
	StopWatchMemoryFirstDigit=-1;
	KeyCallback=&stopwatch_callback;
}

#endif // INCLUDE_STOPWATCH

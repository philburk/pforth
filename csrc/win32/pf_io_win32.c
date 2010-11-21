/* $Id$ */
/***************************************************************
** I/O subsystem for PForth for WIN32 systems.
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** The pForth software code is dedicated to the public domain,
** and any third party may reproduce, distribute and modify
** the pForth software code or any derivative works thereof
** without any compensation or license.  The pForth software
** code is provided on an "as is" basis without any warranty
** of any kind, including, without limitation, the implied
** warranties of merchantability and fitness for a particular
** purpose and their equivalents under the laws of any jurisdiction.
**
****************************************************************
** 941004 PLB Extracted IO calls from pforth_main.c
***************************************************************/

#include "../pf_all.h"

#include <conio.h>

/* Use console mode I/O so that KEY and ?TERMINAL will work. */
#if defined(WIN32) || defined(__NT__)
int  sdTerminalOut( char c )
{
#if defined(__WATCOMC__)
	return putch((char)(c));
#else
	return _putch((char)(c));
#endif
}

/* Needed cuz _getch() does not echo. */
int  sdTerminalEcho( char c )
{
#if defined(__WATCOMC__)
	return putch((char)(c));
#else
	return _putch((char)(c));
#endif
}

int  sdTerminalIn( void )
{
	return _getch();
}

int  sdQueryTerminal( void )
{
	return _kbhit();
}

int  sdTerminalFlush( void )
{
#ifdef PF_NO_FILEIO
	return -1;
#else
	return fflush(PF_STDOUT);
#endif
}

void sdTerminalInit( void )
{
}

void sdTerminalTerm( void )
{
}
#endif

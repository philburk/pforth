/* $Id$ */
/***************************************************************
** I/O subsystem for PForth for WIN32 systems.
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** Permission to use, copy, modify, and/or distribute this
** software for any purpose with or without fee is hereby granted.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
** THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
** CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
** CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**
****************************************************************
** 941004 PLB Extracted IO calls from pforth_main.c
***************************************************************/

#include "../pf_all.h"

#include <conio.h>
#include <synchapi.h>   /* for Sleep() */

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

cell_t sdSleepMillis(cell_t msec)
{
    if (msec < 0) return 0;
    Sleep((DWORD)msec);
    return 0;
}

#endif

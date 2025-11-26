/* $Id$ */
/***************************************************************
** I/O subsystem for PForth for common systems.
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

/* Default portable terminal I/O. */
int  sdTerminalOut( char c )
{
    return putchar(c);
}
/* We don't need to echo because getchar() echos. */
int  sdTerminalEcho( char c )
{
    return 0;
}
int  sdTerminalIn( void )
{
    return getchar();
}
int  sdQueryTerminal( void )
{
    return 0;
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
    return PF_ERR_NOT_SUPPORTED;
}

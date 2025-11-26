/* $Id$ */
/***************************************************************
** I/O subsystem for PForth when NO CHARACTER I/O is supported.
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

#include "pf_all.h"


#ifdef PF_NO_CHARIO
int  sdTerminalOut( char c )
{
    TOUCH(c);
    return 0;
}
int  sdTerminalEcho( char c )
{
    TOUCH(c);
    return 0;
}
int  sdTerminalIn( void )
{
    return -1;
}
int  sdTerminalFlush( void )
{
    return -1;
}
void sdTerminalInit( void )
{
}
void sdTerminalTerm( void )
{
}

void sdSleepMillis(cell_t /* msec */)
{
    // TODO Call some platform specific sleep function here.
    return PF_ERR_NOT_SUPPORTED;
}
#endif

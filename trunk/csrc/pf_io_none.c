/* $Id$ */
/***************************************************************
** I/O subsystem for PForth when NO CHARACTER I/O is supported.
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
#endif

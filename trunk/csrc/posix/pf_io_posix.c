/* $Id$ */
/***************************************************************
** I/O subsystem for PForth based on 'C'
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

#if PF_POSIX_IO
/* Configure console so that characters are not buffered.
 * This allows KEY to work and also HISTORY.ON
 * Thanks to Ralf Baechle and David Feuer for contributing this.
 */

#include <unistd.h>
#ifdef sun
#include <sys/int_types.h> /* Needed on Solaris for uint32_t in termio.h */
#endif
#include <termios.h>
#include <sys/poll.h>

#define stdin_fd 1

static struct termios save_termios;
static int stdin_is_tty;

/* Default portable terminal I/O. */
int  sdTerminalOut( char c )
{
	return putchar(c);
}
/* We don't need to echo because getchar() echos. */
int  sdTerminalEcho( char c )
{
	TOUCH(c);
	return 0;
}
int  sdTerminalIn( void )
{
	return getchar();
}

int  sdTerminalFlush( void )
{
#ifdef PF_NO_FILEIO
	return -1;
#else
	return fflush(PF_STDOUT);
#endif
}

/****************************************************/
int sdQueryTerminal( void )
{
	struct pollfd  pfd;
	sdTerminalFlush();
	pfd.fd = stdin_fd;
	pfd.events = stdin_fd;
	return poll( &pfd, 1, 0 );	
}

/****************************************************/
void sdTerminalInit(void)
{
        struct termios term;
	
        stdin_is_tty = isatty(stdin_fd);
        if (!stdin_is_tty)
                return;
		
/* Get current terminal attributes and save them so we can restore them. */
        tcgetattr(stdin_fd, &term);
        save_termios = term;
	
/* ICANON says to wait upon read until a character is received,
 * and then to return it immediately (or soon enough....)
 * ECHOCTL says not to echo backspaces and other control chars as ^H */
        term.c_lflag &= ~( ECHO | ECHONL | ECHOCTL | ICANON );
        term.c_cc[VTIME] = 0;
        term.c_cc[VMIN] = 1;
        tcsetattr(stdin_fd, TCSANOW, &term);
}

/****************************************************/
void sdTerminalTerm(void)
{
        if (!stdin_is_tty)
                return;

        tcsetattr(stdin_fd, TCSANOW, &save_termios);
}

#undef stdin_fd

#endif

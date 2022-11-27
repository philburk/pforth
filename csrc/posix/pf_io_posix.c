/* $Id$ */
/***************************************************************
** I/O subsystem for PForth based on 'C'
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
** 090220 PLB Fixed broken sdQueryTerminal on Mac. It always returned true.
***************************************************************/

#include "../pf_all.h"

/* Configure console so that characters are not buffered.
 * This allows KEY and ?TERMINAL to work and also HISTORY.ON
 */

#include <unistd.h>
#include <sys/time.h>
#ifdef sun
#include <sys/int_types.h> /* Needed on Solaris for uint32_t in termio.h */
#endif
#include <termios.h>
#include <sys/poll.h>

static struct termios save_termios;
static int stdin_is_tty;

/* poll() is broken in Mac OS X Tiger OS so use select() instead. */
#ifndef PF_USE_SELECT
#define PF_USE_SELECT  (1)
#endif

/* Default portable terminal I/O. */
int  sdTerminalOut( char c )
{
    return putchar(c);
}

int  sdTerminalEcho( char c )
{
    putchar(c);
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
#if PF_USE_SELECT
    int select_retval;
    fd_set readfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    /* Set timeout to zero so that we just poll and return. */
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    select_retval = select(STDIN_FILENO+1, &readfds, NULL, NULL, &tv);
    if (select_retval < 0)
    {
        perror("sdTerminalInit: select");
    }
    return FD_ISSET(STDIN_FILENO,&readfds) ? FTRUE : FFALSE;

#else
    int result;
    struct pollfd  pfd = { 0 };
    sdTerminalFlush();
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    result = poll( &pfd, 1, 0 );
    /* On a Mac it may set revents to POLLNVAL because poll() is broken on Tiger. */
    if( pfd.revents & POLLNVAL )
    {
        PRT(("sdQueryTerminal: poll got POLLNVAL, stdin not open\n"));
        return FFALSE;
    }
    else
    {
        return (pfd.revents & POLLIN) ? FTRUE : FFALSE;
    }
#endif
}

/****************************************************/
void sdTerminalInit(void)
{
    struct termios term;

    stdin_is_tty = isatty(STDIN_FILENO);
    if (stdin_is_tty)
    {
/* Get current terminal attributes and save them so we can restore them. */
        tcgetattr(STDIN_FILENO, &term);
        save_termios = term;

/* ICANON says to wait upon read until a character is received,
 * and then to return it immediately (or soon enough....)
 * ECHOCTL says not to echo backspaces and other control chars as ^H */
        term.c_lflag &= ~( ECHO | ECHONL | ECHOCTL | ICANON );
        term.c_cc[VTIME] = 0;
        term.c_cc[VMIN] = 1;
        if( tcsetattr(STDIN_FILENO, TCSANOW, &term) < 0 )
        {
            perror("sdTerminalInit: tcsetattr");
        }
        if (setvbuf(stdout, NULL, _IONBF, (size_t) 0) != 0)
        {
            perror("sdTerminalInit: setvbuf");
        }
    }
}

/****************************************************/
void sdTerminalTerm(void)
{
    if (stdin_is_tty)
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &save_termios);
    }
}

cell_t sdSleepMillis(cell_t msec)
{
    const cell_t kMaxMicros = 500000; /* to be safe, usleep() limit is 1000000 */
    cell_t micros;
    cell_t napTime;
    if (msec < 0) return 0;
    micros = msec * 1000;
    while (micros > 0)
    {
        napTime = (micros > kMaxMicros) ? kMaxMicros : micros;
        if (usleep(napTime))
        {
            perror("sdSleepMillis: usleep failed");
            return -1;
        }
        micros -= napTime;
    }
    return 0;
}

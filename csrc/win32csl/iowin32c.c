/* $Id$ */
/***************************************************************
** I/O subsystem for PForth for WIN32 systems.
**
** Use Windows Console so we can add the ANSI console commands needed to support HISTORY
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
***************************************************************/

#include "../pf_all.h"

#if defined(WIN32) || defined(__NT__)

#include <windows.h>

#define ASCII_ESCAPE  (0x1B)

static HANDLE sConsoleHandle = INVALID_HANDLE_VALUE;
static int sIsConsoleValid = FALSE;

typedef enum ConsoleState_e
{
    SDCONSOLE_STATE_IDLE = 0,
    SDCONSOLE_STATE_GOT_ESCAPE,
    SDCONSOLE_STATE_GOT_BRACKET

} ConsoleState;

static int sConsoleState = SDCONSOLE_STATE_IDLE;
static int sParam1 = 0;
static CONSOLE_SCREEN_BUFFER_INFO sScreenInfo;

/******************************************************************/
static void sdConsoleEmit( char c )
{
  /* Write a WCHAR in case we have compiled with Unicode support.
   * Otherwise we will see '?' printed.*/
    WCHAR  wc = (WCHAR) c;
    DWORD count;
    if( sIsConsoleValid )
    {
        WriteConsoleW(sConsoleHandle, &wc, 1, &count, NULL );
    }
    else
    {
          /* This will get called if we are redirecting to a file.*/
        WriteFile(sConsoleHandle, &c, 1, &count, NULL );
    }
}

/******************************************************************/
static void sdClearScreen( void )
{
    if( GetConsoleScreenBufferInfo( sConsoleHandle, &sScreenInfo ) )
    {
        COORD XY;
        int numNeeded;
        DWORD count;
        XY.X = 0;
        XY.Y = sScreenInfo.srWindow.Top;
        numNeeded = sScreenInfo.dwSize.X * (sScreenInfo.srWindow.Bottom - sScreenInfo.srWindow.Top + 1);
        FillConsoleOutputCharacter(
            sConsoleHandle, ' ', numNeeded, XY, &count );
        SetConsoleCursorPosition( sConsoleHandle, XY );
    }
}

/******************************************************************/
static void sdEraseEOL( void )
{
    if( GetConsoleScreenBufferInfo( sConsoleHandle, &sScreenInfo ) )
    {
        COORD savedXY;
        int numNeeded;
        DWORD count;
        savedXY.X = sScreenInfo.dwCursorPosition.X;
        savedXY.Y = sScreenInfo.dwCursorPosition.Y;
        numNeeded = sScreenInfo.dwSize.X - savedXY.X;
        FillConsoleOutputCharacter(
            sConsoleHandle, ' ', numNeeded, savedXY, &count );
    }
}

/******************************************************************/
static void sdCursorBack( int dx )
{
    if( GetConsoleScreenBufferInfo( sConsoleHandle, &sScreenInfo ) )
    {
        COORD XY;
        XY.X = sScreenInfo.dwCursorPosition.X;
        XY.Y = sScreenInfo.dwCursorPosition.Y;
        XY.X -= dx;
        if( XY.X < 0 ) XY.X = 0;
        SetConsoleCursorPosition( sConsoleHandle, XY );
    }
}
/******************************************************************/
static void sdCursorForward( int dx )
{
    if( GetConsoleScreenBufferInfo( sConsoleHandle, &sScreenInfo ) )
    {
        COORD XY;
        int width = sScreenInfo.dwSize.X;
        XY.X = sScreenInfo.dwCursorPosition.X;
        XY.Y = sScreenInfo.dwCursorPosition.Y;
        XY.X += dx;
        if( XY.X > width ) XY.X = width;
        SetConsoleCursorPosition( sConsoleHandle, XY );
    }
}

/******************************************************************/
/* Use console mode I/O so that KEY and ?TERMINAL will work.
 * Parse ANSI escape sequences and call the appropriate cursor
 * control functions.
 */
int  sdTerminalOut( char c )
{
    switch( sConsoleState )
    {
    case SDCONSOLE_STATE_IDLE:
        switch( c )
        {
        case ASCII_ESCAPE:
            sConsoleState = SDCONSOLE_STATE_GOT_ESCAPE;
            break;
        default:
            sdConsoleEmit( c );
        }
        break;

    case SDCONSOLE_STATE_GOT_ESCAPE:
        switch( c )
        {
        case '[':
            sConsoleState = SDCONSOLE_STATE_GOT_BRACKET;
            sParam1 = 0;
            break;
        default:
            sConsoleState = SDCONSOLE_STATE_IDLE;
            sdConsoleEmit( c );
        }
        break;

    case SDCONSOLE_STATE_GOT_BRACKET:
        if( (c >= '0') && (c <= '9') )
        {
            sParam1 = (sParam1 * 10) + (c - '0');
        }
        else
        {
            sConsoleState = SDCONSOLE_STATE_IDLE;
            if( c == 'K')
            {
                sdEraseEOL();
            }
            else if( c == 'D' )
            {
                sdCursorBack( sParam1 );
            }
            else if( c == 'C' )
            {
                sdCursorForward( sParam1 );
            }
            else if( (c == 'J') && (sParam1 == 2) )
            {
                sdClearScreen();
            }
        }
        break;
    }
    return 0;
}

/* Needed cuz _getch() does not echo. */
int  sdTerminalEcho( char c )
{
    sdConsoleEmit((char)(c));
    return 0;
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
    DWORD mode = 0;
    sConsoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );
    if( GetConsoleMode( sConsoleHandle, &mode ) )
    {
          /*printf("GetConsoleMode() mode is 0x%08X\n", mode );*/
        sIsConsoleValid = TRUE;
    }
    else
    {
          /*printf("GetConsoleMode() failed\n", mode );*/
        sIsConsoleValid = FALSE;
    }
}

void sdTerminalTerm( void )
{
}

cell_t sdSleepMillis(cell_t msec)
{
    Sleep(msec);
    return 0;
}

#endif

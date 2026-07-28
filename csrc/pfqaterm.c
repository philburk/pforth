/* @(#) pfqaterm.c 26/07/28 1.0 */
/***************************************************************
** Interactive test for the pForth terminal I/O subsystem.
**
** The terminal API is defined in "pf_io.h". A developer who
** implements that API for a new platform can run this program
** to check that the character I/O behaves as pForth expects.
**
** This test is interactive. It must be run from a terminal
** because it asks the user to press a key.
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "pf_all.h"
#include "paging/unittest.h"

PFQA_INSTANTIATE_GLOBALS;

/* How long to nap between calls to sdQueryTerminal(). */
#define POLL_PERIOD_MSEC   (50)
/* How long to wait for the user before giving up. */
#define TIMEOUT_MSEC       (10000)
#define MAX_POLLS          (TIMEOUT_MSEC / POLL_PERIOD_MSEC)

/* sdTerminalIn() should not block because a character is already
** available. time() only has one second of resolution so allow for
** the second to tick over while we are calling it. */
#define MAX_READ_SECONDS   (1)

/***************************************************************
** Print a message using the terminal API under test.
*/
static void TermPrint( const char *msg )
{
    const char *s = msg;
    while( *s )
    {
        sdTerminalOut( *s++ );
    }
    sdTerminalFlush();
}

/***************************************************************
** Throw away any characters that are already waiting so that
** stale input cannot satisfy the test.
** The loop is bounded because sdQueryTerminal() stays true forever
** if stdin is at the end of a file or a closed pipe.
*/
#define MAX_DRAIN   (100)
static void DrainTerminal( void )
{
    int i;
    for( i = 0; i < MAX_DRAIN; i++ )
    {
        if( sdQueryTerminal() == FFALSE ) break;
        if( sdTerminalIn() == EOF ) break;
    }
}

/***************************************************************
** Test sdQueryTerminal(), sdTerminalIn() and sdSleepMillis().
*/
static void pfQaTerminalIO( void )
{
    int numPolls;
    int query;
    int c;
    time_t startTime;
    time_t stopTime;

    DrainTerminal();

    TermPrint( "Terminal QA for pForth\n" );

/* Nothing has been typed yet so the terminal should be quiet. */
    ASSERT_EQ( FFALSE, sdQueryTerminal() );

    ASSERT_EQ( 0, sdSleepMillis( POLL_PERIOD_MSEC ) );

    TermPrint( "Please press a letter key.\n" );

/* Poll until a character arrives or we run out of patience. */
    for( numPolls = 0; numPolls < MAX_POLLS; numPolls++ )
    {
        query = sdQueryTerminal();
        if( query != FFALSE ) break;
        sdSleepMillis( POLL_PERIOD_MSEC );
    }

    if( numPolls >= MAX_POLLS )
    {
        TermPrint( "TIMED OUT! No key was pressed.\n" );
        ASSERT_LT( numPolls, MAX_POLLS );
    }

/* sdQueryTerminal() must not consume the character. */
    ASSERT_NE( FFALSE, sdQueryTerminal() );
    ASSERT_NE( FFALSE, sdQueryTerminal() );

/* A character is waiting so this should return immediately. */
    startTime = time( NULL );
    c = sdTerminalIn();
    stopTime = time( NULL );
    ASSERT_LE( stopTime - startTime, MAX_READ_SECONDS );

/* Did we get the key that the user pressed? */
    ASSERT_NE( EOF, c );
    EXPECT_TRUE( isalpha( c ) );
    TermPrint( "You pressed '" );
    sdTerminalEcho( (char) c );
    TermPrint( "'\n" );

/* The character was consumed so the terminal should be quiet again. */
    ASSERT_EQ( FFALSE, sdQueryTerminal() );

error:
    return;
}

/***************************************************************/
int main( void )
{
    sdTerminalInit();
    pfQaTerminalIO();
    sdTerminalTerm();

    PFQA_PRINT_RESULT;
    return PFQA_EXIT_RESULT;
}

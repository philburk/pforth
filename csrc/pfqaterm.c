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

/* How many characters must be buffered while the program is asleep. */
#define NUM_BUFFERED_CHARS (5)
/* How long the user has to type them. */
#define TYPING_TIME_MSEC   (5000)

#define MYEOL   "\r\n"

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

/** Print a message and a line terminator.
 */
static void TermPrintLine( const char *msg )
{
    TermPrint(msg);
    TermPrint(MYEOL);
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
static void pfQaQueryTerminal( void )
{
    int numPolls;
    int query;
    int c;
    time_t startTime;
    time_t stopTime;

    DrainTerminal();

/* Nothing has been typed yet so the terminal should be quiet. */
    ASSERT_EQ( FFALSE, sdQueryTerminal() );

    ASSERT_EQ( 0, sdSleepMillis( POLL_PERIOD_MSEC ) );

    TermPrintLine(MYEOL "----- pfQaQueryTerminal");
    TermPrintLine( "Please press a letter key." );

/* Poll until a character arrives or we run out of patience. */
    for( numPolls = 0; numPolls < MAX_POLLS; numPolls++ )
    {
        query = sdQueryTerminal();
        if( query != FFALSE ) break;
        sdSleepMillis( POLL_PERIOD_MSEC );
    }

    if( numPolls >= MAX_POLLS )
    {
        TermPrintLine( "TIMED OUT! No key was pressed." );
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
    sdTerminalOut( (char) c );
    TermPrintLine( "'" );

/* The character was consumed so the terminal should be quiet again. */
    ASSERT_EQ( FFALSE, sdQueryTerminal() );

error:
    return;
}

/***************************************************************
** Test that characters typed while the program is not looking
** are buffered by the terminal instead of being dropped.
*/
static void pfQaTerminalBuffering( void )
{
    int i;
    int c;
    char typed[NUM_BUFFERED_CHARS + 1];
    time_t startTime;
    time_t stopTime;

    DrainTerminal();

/* Nothing has been typed yet so the terminal should be quiet. */
    ASSERT_EQ( FFALSE, sdQueryTerminal() );

    TermPrintLine(MYEOL "----- pfQaTerminalBuffering");
    TermPrintLine( "Please type any 5 letters within the next 5 seconds." );
    TermPrintLine( "They may not be echoed." );

/* Ignore the terminal completely while the user types. */
    ASSERT_EQ( 0, sdSleepMillis( TYPING_TIME_MSEC ) );

/* All 5 characters should have been buffered while we were asleep. */
    startTime = time( NULL );
    for( i = 0; i < NUM_BUFFERED_CHARS; i++ )
    {
        ASSERT_NE( FFALSE, sdQueryTerminal() );
        c = sdTerminalIn();
        ASSERT_NE( EOF, c );
        EXPECT_TRUE( isalpha( c ) );
        typed[i] = (char) c;
    }
    typed[NUM_BUFFERED_CHARS] = '\0';

/* The characters were already waiting so none of the reads should block. */
    stopTime = time( NULL );
    ASSERT_LE( stopTime - startTime, MAX_READ_SECONDS );

    TermPrint( "You typed '" );
    TermPrint( typed );
    TermPrintLine( "'" );

/* We read every character so the terminal should be quiet again. */
    ASSERT_EQ( FFALSE, sdQueryTerminal() );

error:
    return;
}

/***************************************************************
** Test that characters echoed while the user is typing.
*/
static void pfQaTerminalEcho( void )
{
    int i;
    int c;

    DrainTerminal();

/* Nothing has been typed yet so the terminal should be quiet. */
    ASSERT_EQ( FFALSE, sdQueryTerminal() );

    TermPrintLine(MYEOL "----- pfQaTerminalEcho");
    TermPrintLine( "Please type 5 letters or numbers at any speed:" );

/* All 5 characters should have been buffered while we were asleep. */
    for( i = 0; i < NUM_BUFFERED_CHARS; i++ )
    {
        c = sdTerminalIn();
        sdTerminalEcho(c);
    }
    TermPrintLine("");
    TermPrintLine("You should have seen each key echo once and only once.\n");

error:
    return;
}

/***************************************************************
** Test for overflow when blasting characters.
*/
static void pfQaTerminalOverflow( void )
{
    int i;
    int c;
    const char text[] = "abcdefghijklmnopqrstuvwxyz1234567890";
    char typed[sizeof(text)];
    int enabled = 1;

    DrainTerminal();

/* Nothing has been typed yet so the terminal should be quiet. */
    ASSERT_EQ( FFALSE, sdQueryTerminal() );

    TermPrintLine(MYEOL "----- pfQaTerminalOverflow");
    TermPrintLine("Testing for terminal input buffer overflow.");
    TermPrintLine("Copy the string below, with no spaces, to your Paste buffer.");
    TermPrint(MYEOL "    ");
    TermPrintLine(text);

    TermPrintLine(MYEOL "Now Paste the String into the terminal and hit ENTER or RETURN");
    TermPrintLine("as many times as you like.");
    TermPrintLine("When you are done, hit '.' and ENTER or RETURN to stop.");

    while (enabled) {
        for( i = 0; i < sizeof(typed); i++ ) typed[i] = 0; /* Clear typed buffer. */
        for( i = 0; i < (int)(sizeof(text) - 1); i++ )
        {
            c = sdTerminalIn();
            sdTerminalEcho(c);
            if (c == '.') {
                TermPrintLine(MYEOL "Stop requested.");
                enabled = 0;
                break;
            }
            typed[i] = c;
            /* TermPrint(" - got "); TermPrintLine(typed); */
            if (c != text[i]) {
                TermPrintLine(MYEOL "Dropped character");
                TermPrintLine(typed);
                DrainTerminal();
                enabled = 0;
                ASSERT_EQ(text[i], c);
                break;
            }
        }
        TermPrintLine(MYEOL "Waiting for ENTER or RETURN.");
        c = sdTerminalIn(); /* Discard EOL */
        ASSERT_TRUE(!isalpha(c));
        if (enabled) {
            TermPrintLine(MYEOL "SUCCESS - Paste again and hit ENTER or RETURN.");
        }
    }

    TermPrintLine("pfQaTerminalOverflow test complete.");

error:
    return;
}

/***************************************************************/
int main( void )
{
    sdTerminalInit();
    TermPrintLine( "Terminal QA for pForth" );
    pfQaQueryTerminal();
    pfQaTerminalBuffering();
    pfQaTerminalEcho();
    pfQaTerminalOverflow();
    sdTerminalTerm();

    PFQA_PRINT_RESULT;
    return PFQA_EXIT_RESULT;
}

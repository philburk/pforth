/* @(#) pf_clib.c 96/12/18 1.12 */
/***************************************************************
** Duplicate functions from stdlib for PForth based on 'C'
**
** This code duplicates some of the code in the 'C' lib
** because it reduces the dependency on foreign libraries
** for monitor mode where no OS is available.
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
** 961124 PLB Advance pointers in pfCopyMemory() and pfSetMemory()
***************************************************************/

#include "pf_all.h"

#ifdef PF_NO_CLIB
/* Count chars until NUL.  Replace strlen() */
#define  NUL  ((char) 0)
cell_t pfCStringLength( const char *s )
{
	cell_t len = 0;
	while( *s++ != NUL ) len++;
	return len;
}
 
/*    void *memset (void *s, cell_t c, size_t n); */
void *pfSetMemory( void *s, cell_t c, cell_t n )
{
	uint8_t *p = s, byt = (uint8_t) c;
	while( (n--) > 0) *p++ = byt;
	return s;
}

/*  void *memccpy (void *s1, const void *s2, cell_t c, size_t n); */
void *pfCopyMemory( void *s1, const void *s2, cell_t n)
{
	uint8_t *p1 = s1;
	const uint8_t *p2 = s2;
	while( (n--) > 0) *p1++ = *p2++;
	return s1;
}

#endif  /* PF_NO_CLIB */

char pfCharToUpper( char c )
{
	return (char) ( ((c>='a') && (c<='z')) ? (c - ('a' - 'A')) : c );
}

char pfCharToLower( char c )
{
	return (char) ( ((c>='A') && (c<='Z')) ? (c + ('a' - 'A')) : c );
}

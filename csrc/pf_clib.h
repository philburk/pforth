/* @(#) pf_clib.h 96/12/18 1.10 */
#ifndef _pf_clib_h
#define _pf_clib_h

/***************************************************************
** Include file for PForth tools
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
***************************************************************/

#ifdef  PF_NO_CLIB

	#ifdef __cplusplus
	extern "C" {
	#endif

	cell_t pfCStringLength( const char *s );
	void *pfSetMemory( void *s, cell_t c, cell_t n );
	void *pfCopyMemory( void *s1, const void *s2, cell_t n);
	#define EXIT(n)  {while(1);}
	
	#ifdef __cplusplus
	}   
	#endif

#else   /* PF_NO_CLIB */

	#ifdef PF_USER_CLIB
		#include PF_USER_CLIB
	#else
/* Use stdlib functions if available because they are probably faster. */
		#define pfCStringLength strlen
		#define pfSetMemory     memset
		#define pfCopyMemory    memcpy
		#define EXIT(n)  exit(n)
	#endif /* PF_USER_CLIB */
	
#endif  /* !PF_NO_CLIB */

#ifdef __cplusplus
extern "C" {
#endif

/* Always use my own functions to avoid macro expansion problems with tolower(*s++) */
char pfCharToUpper( char c );
char pfCharToLower( char c );

#ifdef __cplusplus
}   
#endif

#endif /* _pf_clib_h */

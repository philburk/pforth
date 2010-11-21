/* @(#) pf_core.h 98/01/26 1.3 */
#ifndef _pf_core_h
#define _pf_core_h

/***************************************************************
** Include file for PForth 'C' Glue support
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

#ifdef __cplusplus
extern "C" {
#endif

void   pfInitGlobals( void );

void   pfDebugMessage( const char *CString );
void   pfDebugPrintDecimalNumber( int n );
	
cell_t pfUnitTestText( void );

#ifdef __cplusplus
}   
#endif


#endif /* _pf_core_h */

/* @(#) pf_cglue.h 96/12/18 1.7 */
#ifndef _pf_c_glue_h
#define _pf_c_glue_h

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

typedef cell (*CFunc0)( void );
typedef cell (*CFunc1)( cell P1 );
typedef cell (*CFunc2)( cell P1, cell P2 );
typedef cell (*CFunc3)( cell P1, cell P2, cell P3 );
typedef cell (*CFunc4)( cell P1, cell P2, cell P3, cell P4 );
typedef cell (*CFunc5)( cell P1, cell P2, cell P3, cell P4, cell P5 );

#ifdef __cplusplus
extern "C" {
#endif

Err   CreateGlueToC( const char *CName, uint32 Index, int32 ReturnMode, int32 NumParams );
Err   CompileCustomFunctions( void );
Err   LoadCustomFunctionTable( void );
int32 CallUserFunction( int32 Index, int32 ReturnMode, int32 NumParams );

#ifdef __cplusplus
}   
#endif

#define C_RETURNS_VOID (0)
#define C_RETURNS_VALUE (1)

#endif /* _pf_c_glue_h */

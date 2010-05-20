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

typedef cell_t (*CFunc0)( void );
typedef cell_t (*CFunc1)( cell_t P1 );
typedef cell_t (*CFunc2)( cell_t P1, cell_t P2 );
typedef cell_t (*CFunc3)( cell_t P1, cell_t P2, cell_t P3 );
typedef cell_t (*CFunc4)( cell_t P1, cell_t P2, cell_t P3, cell_t P4 );
typedef cell_t (*CFunc5)( cell_t P1, cell_t P2, cell_t P3, cell_t P4, cell_t P5 );

#ifdef __cplusplus
extern "C" {
#endif

Err   CreateGlueToC( const char *CName, ucell_t Index, cell_t ReturnMode, int32_t NumParams );
Err   CompileCustomFunctions( void );
Err   LoadCustomFunctionTable( void );
cell_t CallUserFunction( cell_t Index, int32_t ReturnMode, int32_t NumParams );

#ifdef __cplusplus
}   
#endif

#define C_RETURNS_VOID (0)
#define C_RETURNS_VALUE (1)

#endif /* _pf_c_glue_h */

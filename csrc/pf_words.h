/* @(#) pf_words.h 96/12/18 1.7 */
#ifndef _pforth_words_h
#define _pforth_words_h

/***************************************************************
** Include file for PForth Words
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

void ffDot( cell_t n );
void ffDotHex( cell_t n );
void ffDotS( void );
cell_t ffSkip( char *AddrIn, cell_t Cnt, char c, char **AddrOut );
cell_t ffScan( char *AddrIn, cell_t Cnt, char c, char **AddrOut );

#ifdef __cplusplus
}   
#endif

#endif /* _pforth_words_h */

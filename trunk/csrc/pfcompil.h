/* @(#) pfcompil.h 96/12/18 1.11 */

#ifndef _pforth_compile_h
#define _pforth_compile_h

/***************************************************************
** Include file for PForth Compiler
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

Err   ffPushInputStream( FileStream *InputFile );
ExecToken NameToToken( const ForthString *NFA );
FileStream * ffConvertSourceIDToStream( cell_t id );
FileStream *ffPopInputStream( void );
cell_t  ffConvertStreamToSourceID( FileStream *Stream );
cell_t  ffFind( const ForthString *WordName, ExecToken *pXT );
cell_t  ffFindC( const char *WordName, ExecToken *pXT );
cell_t  ffFindNFA( const ForthString *WordName, const ForthString **NFAPtr );
cell_t  ffNumberQ( const char *FWord, cell_t *Num );
cell_t  ffRefill( void );
cell_t  ffTokenToName( ExecToken XT, const ForthString **NFAPtr );
cell_t *NameToCode( ForthString *NFA );
PForthDictionary pfBuildDictionary( cell_t HeaderSize, cell_t CodeSize );
char *ffWord( char c );
const ForthString *NameToPrevious( const ForthString *NFA );
cell_t FindSpecialCFAs( void );
cell_t FindSpecialXTs( void );
cell_t NotCompiled( const char *FunctionName );
void  CreateDicEntry( ExecToken XT, const ForthStringPtr FName, ucell_t Flags );
void  CreateDicEntryC( ExecToken XT, const char *CName, ucell_t Flags );
void  ff2Literal( cell_t dHi, cell_t dLo );
void  ffALiteral( cell_t Num );
void  ffColon( void );
void  ffCreate( void );
void  ffCreateSecondaryHeader( const ForthStringPtr FName);
void  ffDefer( void );
void  ffFinishSecondary( void );
void  ffLiteral( cell_t Num );
void  ffStringCreate( ForthStringPtr FName);
void  ffStringDefer( const ForthStringPtr FName, ExecToken DefaultXT );
void  pfHandleIncludeError( void );

ThrowCode ffSemiColon( void );
ThrowCode ffOK( void );
ThrowCode ffInterpret( void );
ThrowCode ffOuterInterpreterLoop( void );
ThrowCode ffIncludeFile( FileStream *InputFile );

#ifdef PF_SUPPORT_FP
void ffFPLiteral( PF_FLOAT fnum );
#endif

#ifdef __cplusplus
}   
#endif

#endif /* _pforth_compile_h */

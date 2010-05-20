/* @(#) pforth.h 98/01/26 1.2 */
#ifndef _pforth_h
#define _pforth_h

/***************************************************************
** Include file for pForth, a portable Forth based on 'C'
**
** This file is included in any application that uses pForth as a tool.
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
**
***************************************************************/

/* Define stubs for data types so we can pass pointers but not touch inside. */
typedef void *PForthTask;
typedef void *PForthDictionary;

#include <stdint.h>
/* Integer types for Forth cells, signed and unsigned: */
typedef intptr_t cell_t;
typedef uintptr_t ucell_t;

typedef ucell_t ExecToken;              /* Execution Token */
typedef cell_t ThrowCode;

#ifdef __cplusplus
extern "C" {
#endif

/* Main entry point to pForth. */
cell_t pfDoForth( const char *DicName, const char *SourceName, cell_t IfInit );

/* Turn off messages. */
void  pfSetQuiet( cell_t IfQuiet );

/* Query message status. */
cell_t  pfQueryQuiet( void );

/* Send a message using low level I/O of pForth */
void  pfMessage( const char *CString );

/* Create a task used to maintain context of execution. */
PForthTask pfCreateTask( cell_t UserStackDepth, cell_t ReturnStackDepth );

/* Establish this task as the current task. */
void  pfSetCurrentTask( PForthTask task );

/* Delete task created by pfCreateTask */
void  pfDeleteTask( PForthTask task );

/* Build a dictionary with all the basic kernel words. */
PForthDictionary pfBuildDictionary( cell_t HeaderSize, cell_t CodeSize );

/* Create an empty dictionary. */
PForthDictionary pfCreateDictionary( cell_t HeaderSize, cell_t CodeSize );

/* Load dictionary from a file. */
PForthDictionary pfLoadDictionary( const char *FileName, ExecToken *EntryPointPtr );

/* Load dictionary from static array in "pfdicdat.h". */
PForthDictionary pfLoadStaticDictionary( void );

/* Delete dictionary data. */
void  pfDeleteDictionary( PForthDictionary dict );

/* Execute the pForth interpreter. Yes, QUIT is an odd name but it has historical meaning. */
ThrowCode pfQuit( void );

/* Execute a single execution token in the current task and return 0 or an error code. */
int pfCatch( ExecToken XT );
 
/* Include the given pForth source code file. */
ThrowCode pfIncludeFile( const char *FileName );

/* Execute a Forth word by name. */
ThrowCode  pfExecIfDefined( const char *CString );

#ifdef __cplusplus
}   
#endif

#endif  /* _pforth_h */

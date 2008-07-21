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

typedef unsigned long ExecToken;              /* Execution Token */
typedef long          ThrowCode;

#ifndef int32
	typedef long int32;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Main entry point to pForth. */
int32 pfDoForth( const char *DicName, const char *SourceName, int32 IfInit );

/* Turn off messages. */
void  pfSetQuiet( int32 IfQuiet );

/* Query message status. */
int32  pfQueryQuiet( void );

/* Send a message using low level I/O of pForth */
void  pfMessage( const char *CString );

/* Create a task used to maintain context of execution. */
PForthTask pfCreateTask( int32 UserStackDepth, int32 ReturnStackDepth );

/* Establish this task as the current task. */
void  pfSetCurrentTask( PForthTask task );

/* Delete task created by pfCreateTask */
void  pfDeleteTask( PForthTask task );

/* Build a dictionary with all the basic kernel words. */
PForthDictionary pfBuildDictionary( int32 HeaderSize, int32 CodeSize );

/* Create an empty dictionary. */
PForthDictionary pfCreateDictionary( int32 HeaderSize, int32 CodeSize );

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

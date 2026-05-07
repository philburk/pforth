/* @(#) pforth.h 98/01/26 1.2 */
#ifndef _pforth_h
#define _pforth_h

/***************************************************************
** Include file for pForth, a portable Forth based on 'C'
**
** This file is included in any application that uses pForth as a library.
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
**
***************************************************************/

/* Define stubs for data types so we can pass pointers but not touch inside. */
typedef void *PForthTask;
typedef void *PForthDictionary;

#include <stdint.h>
#if   INTPTR_MAX == INT64_MAX
  #define PF_64BIT 1
  #define PF_32BIT 0
#elif INTPTR_MAX == INT32_MAX
  #define PF_64BIT 0
  #define PF_32BIT 1
#else
  #error "Unsupported pointer size"
#endif

/* Integer types for Forth cells, signed and unsigned: */
typedef intptr_t cell_t;
typedef uintptr_t ucell_t;

typedef ucell_t ExecToken;              /* Execution Token */
typedef cell_t ThrowCode;

#ifndef PF_DEMAND_PAGING
#define PF_DEMAND_PAGING 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Main entry point to pForth. */
ThrowCode pfDoForth( const char *DicName, const char *SourceName, cell_t IfInit );

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
ThrowCode pfCatch( ExecToken XT );

/* Include the given pForth source code file. */
ThrowCode pfIncludeFile( const char *FileName );

/* Execute a Forth word by name. */
ThrowCode  pfExecIfDefined( const char *CString );


/* Assertion macro for pForth. */
extern cell_t gPfAssertEnabled;
#define PF_ASSERT(_expr) do { \
    if (!(_expr)) { \
        if (gPfAssertEnabled) { \
            pfMessage("PF_ASSERT failed: " #_expr "\n"); \
            (void)(*(volatile int *)0); \
        } \
    } \
} while(0)

#ifdef __cplusplus
}
#endif

#endif  /* _pforth_h */


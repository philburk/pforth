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

/* Opaque pointer types used to hide internal structures. */
typedef void *PForthTask;
typedef void *PForthDictionary;

#include <stdint.h>

#if   INTPTR_MAX == INT64_MAX
    #define PF_POINTER_SIZE 8
#elif INTPTR_MAX == INT32_MAX
    #define PF_POINTER_SIZE 4
#elif INTPTR_MAX == INT16_MAX
    #define PF_POINTER_SIZE 2
#else
    #error "Unsupported pointer size"
#endif

/* Set CELL size to match pointer size if not defined. */
#ifndef PF_CELL_SIZE
    #if   PF_POINTER_SIZE >= 4
        #define PF_CELL_SIZE PF_POINTER_SIZE
    #else
        #define PF_CELL_SIZE 4
    #endif
#endif /* PF_CELL_SIZE */

#if (PF_CELL_SIZE < PF_POINTER_SIZE)
    #error "PF_CELL_SIZE must be at least as big as a pointer."
#endif

/* Integer types for Forth cells, signed and unsigned: */
#if (PF_CELL_SIZE == 8)
    typedef int64_t cell_t;
    typedef uint64_t ucell_t;
#elif (PF_CELL_SIZE == 4)
    typedef int32_t cell_t;
    typedef uint32_t ucell_t;
#else
    #error "Unsupported PF_CELL_SIZE"
#endif

typedef cell_t ExecToken;              /* Execution Token */
typedef cell_t ThrowCode;

#ifndef PF_DEMAND_PAGING
    #if INTPTR_MAX == INT16_MAX
        /* The only way to address enough RAM for the dictionary is through demand paging. */
        #define PF_DEMAND_PAGING 1
    #else /* INTPTR_MAX */
        #define PF_DEMAND_PAGING 0
    #endif /* INTPTR_MAX */
#endif /* PF_DEMAND_PAGING */

typedef ucell_t vm_address_t; /** an address that may be in physical or paged memory */
#define PTR_TO_VMA(p) ((vm_address_t)(uintptr_t)(p))
#define PF_VM_NULL    PTR_TO_VMA(0)

#ifndef PF_ASSERT_ENABLED
#define PF_ASSERT_ENABLED 1
#endif /* PF_ASSERT_ENABLED */

#ifdef __cplusplus
extern "C" {
#endif

/* Main entry point to pForth. */
ThrowCode pfDoForth( const char *DicName, const char *SourceName, cell_t IfInit );

ThrowCode pfInitialize(const char *DicFileName,
                       cell_t IfInit,
                       ExecToken  *EntryPointPtr);

void pfTerminate(void);

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
PForthTask  pfGetCurrentTask(void);

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

/**
 * Interprets the Forth in the text.
 * The text length cannot exceed TIB_SIZE.
 * @param text the Forth code to be executed
 * @return 0 if successful or throw code.
 */
ThrowCode pfInterpretText(char *text);

/**
 * Push a value to the data stack.
 */
void pfPushToStack(cell_t value);

/**
 * Pop a value from the data stack.
 * @return value from top of stack
 */
cell_t pfPopFromStack(void);

/**
 * @return the number of items on the data stack
 */
cell_t pfGetStackDepth(void);

/* Execute a single execution token in the current task and return 0 or an error code. */
ThrowCode pfCatch( ExecToken XT );

/* Include the given pForth source code file. */
ThrowCode pfIncludeFile( const char *FileName );

/* Execute a Forth word by name. */
ThrowCode  pfExecIfDefined( const char *CString );

/**
  * Run unit tests.
  * This will get called automatically if PF_UNIT_TEST preprocessor symbol is defined.
*/
cell_t pfUnitTest( void );

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


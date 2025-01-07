#include "pf_all.h"      /* lots of stuff */
#include "cf_helpers.h"  /* to_C_string */
#include<errno.h>        /* errno         */
#include<limits.h>       /* PATH_MAX */
#include<stdlib.h>       /* malloc */
#include<stdio.h>        /* asprintf, sprintf */
#include<string.h>       /* strerror */
#include<sys/stat.h>     /* struct stat, stat */

/* 
 * put forward declarations here if necessary
*/


/****************************************************************
** Step 1: Put your own special glue routines here
**     or link them in from another file or library.
****************************************************************/

/* exported functions */

static cell_t f4711( cell_t Val )
{/* a quick way to check that custom words are available
 */
    return 11 + 47*Val;
}

static cell_t file_info( cell_t path_caddr, cell_t path_len )
{
    /* takes one filePath (string) as argument and returns some info on it (as a new string)
         Note that you need to use FREE-C on the result buffer (FREE does not work).
    */
    char* path = to_C_string( path_caddr, path_len );
    struct stat info;
    const char* fmtErr  = "error{ id=%i, desc='%s', path='%s' }";
    const char* fmtDir  = "directory{ path='%s' }";
    const char* fmtFile = "file{ size=%i, path='%s' }";
    char* result;
    /* MSYS/Cygwin may warn than asprintf() is not defined but compile and run just fine :-/ */
    if( stat(path, &info) == -1 )
        asprintf( &result, fmtErr, errno, strerror(errno), path );
    else {
        if( S_ISDIR(info.st_mode) )
            asprintf( &result, fmtDir, path );
        else
            asprintf( &result, fmtFile, info.st_size, path );
    }
    PUSH_DATA_STACK( (cell_t) result );
    return (cell_t)strlen(result);
}

static void free_c( cell_t c_allocate_buffer )
{
    /* Using FREE on a c_allocate_buffer is not possible (due to extra information stored),
         using pfAllocMem() / pfFreeMem() does not help either.
       So we need a separate word to free C-allocated buffers.
    */
    free( (void*) c_allocate_buffer );
}


/****************************************************************
** Step 2: Create CustomFunctionTable.
**     Do not change the name of CustomFunctionTable!
**     It is used by the pForth kernel.
****************************************************************/

#ifdef PF_NO_GLOBAL_INIT
/******************
** If your loader does not support global initialization, then you
** must define PF_NO_GLOBAL_INIT and provide a function to fill
** the table. Some embedded system loaders require this!
** Do not change the name of LoadCustomFunctionTable()!
** It is called by the pForth kernel.
*/
#define NUM_CUSTOM_FUNCTIONS  (3)
CFunc0 CustomFunctionTable[NUM_CUSTOM_FUNCTIONS];

Err LoadCustomFunctionTable( void )
{
    CustomFunctionTable[0] = f4711;
    CustomFunctionTable[1] = file_info;
    CustomFunctionTable[1] = free_c;
    return 0;
}

#else
/******************
** If your loader supports global initialization (most do.) then just
** create the table like this.
*/
CFunc0 CustomFunctionTable[] =
{
    (CFunc0) f4711,
    (CFunc0) file_info,
    (CFunc0) free_c
};
#endif


/****************************************************************
** Step 3: Add custom functions to the dictionary.
**     Do not change the name of CompileCustomFunctions!
**     It is called by the pForth kernel.
****************************************************************/

#if (!defined(PF_NO_INIT)) && (!defined(PF_NO_SHELL))
Err CompileCustomFunctions( void )
{
    Err err;
    int i = 0;
/* Compile Forth words that call your custom functions.
** Make sure order of functions matches that in LoadCustomFunctionTable().
** Parameters are: Name in UPPER CASE, Function, Index, Mode, NumParams
*/
    err = CreateGlueToC( "F4711"    , i++, C_RETURNS_VALUE, 1 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "FILE-INFO", i++, C_RETURNS_VALUE, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "FREE-C"   , i++, C_RETURNS_VOID , 1 );
    if( err < 0 ) return err;

    return 0;
}
#else
Err CompileCustomFunctions( void ) { return 0; }
#endif

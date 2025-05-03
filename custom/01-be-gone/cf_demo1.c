#include "pf_all.h"      /* lots of stuff */
#include "cf_helpers.h"  /* panic, safeAlloc, to_C_string, <stdio.h->{fprintf, stderr, ...}  */
#include <errno.h>       /* errno         */

/* 
 * put forward declarations here if necessary
*/


/****************************************************************
** Step 1: Put your own special glue routines here
**     or link them in from another file or library.
****************************************************************/

/* exported functions */

static cell_t f4711( cell_t Val )
{/* a quick way to check that custom worlds are available
 */
    return 11 + 47*Val;
}

static cell_t be_gone( cell_t fileName, cell_t fnLen )
{/* Demonstrates passing strings from PForth to C.
    Interprets the passed strings as file name and tries to delete the file.
	Returns 0 or errno 
 */
    int res;
    char* buf = to_C_string( fileName, fnLen );
    res = remove(buf);  /* delete file in file system */
    if( res!=0 && errno!=0 )
        res = errno;
    free(buf);
    return res;
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
#define NUM_CUSTOM_FUNCTIONS  (2)
CFunc0 CustomFunctionTable[NUM_CUSTOM_FUNCTIONS];

Err LoadCustomFunctionTable( void )
{
    CustomFunctionTable[0] = f4711;
    CustomFunctionTable[1] = be_gone;
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
    (CFunc0) be_gone
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
    err = CreateGlueToC( "F4711"  , i++, C_RETURNS_VALUE, 1 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "BE-GONE", i++, C_RETURNS_VALUE, 2 );
    if( err < 0 ) return err;

    return 0;
}
#else
Err CompileCustomFunctions( void ) { return 0; }
#endif

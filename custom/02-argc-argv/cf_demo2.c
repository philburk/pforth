#include "pf_all.h"      /* lots of stuff */
#include<stdlib.h>       /* calloc */
#include<string.h>       /* strlen */

/* 
 * put forward declarations here if necessary
*/

void fillScriptParams( int startItem, int NoItems, char *argv[] );

/****************************************************************
** Step 1: Put your own special glue routines here
**     or link them in from another file or library.
****************************************************************/

/* exported functions */

static cell_t cf_f4711( cell_t Val )
{/* a quick way to check that custom words are available
 */
    return 11 + 47*Val;
}


struct PascalString {
    char* data;
    size_t len;
} ;
typedef struct PascalString* PascalStringPtr;

static struct  {
    PascalStringPtr args;
    int             cnt;
} ScriptParams;

void fillScriptParams( int startItem, int NoItems, char *argv[] ) {
	int i;
    if( startItem < NoItems ) {
        ScriptParams.cnt = NoItems - startItem;
        ScriptParams.args = (PascalStringPtr) calloc( ScriptParams.cnt, sizeof(struct PascalString) );
		for( i=0; i<ScriptParams.cnt; i++ )  {
			char* progParam = argv[i+startItem];
			ScriptParams.args[i].data = progParam;
			ScriptParams.args[i].len  = strlen(progParam);
		}
    }
}

static cell_t cf_argc()
{/* number of command line parameters after '--'
 */ return ScriptParams.cnt;
}

static cell_t cf_argv( cell_t paramNo )
{/* return argv[paramNo] as '--> caddr len'   OR in case of error  '--> 0 -1'
 */ cell_t len = -1;  /* assume something will go wrong */
    cell_t data = 0;  /* dito                           */
    if( 0<=paramNo  &&  paramNo < ScriptParams.cnt ) {
        data = (cell_t) ScriptParams.args[paramNo].data;
        len  = ScriptParams.args[paramNo].len;
    }
    PUSH_DATA_STACK( (cell_t) data );
    return len;
}



/****************************************************************
** Step 2: Create CustomFunctionTable.
**     Do not change the name of CustomFunctionTable!
**     It is used by the pForth kernel.
****************************************************************/

#ifdef PF_NO_GLOBAL_INIT
#define NUM_CUSTOM_FUNCTIONS  (3)
CFunc0 CustomFunctionTable[NUM_CUSTOM_FUNCTIONS];

Err LoadCustomFunctionTable( void )
{
    CustomFunctionTable[0] = cf_f4711;
    CustomFunctionTable[1] = cf_argc;
    CustomFunctionTable[2] = cf_argv;
    return 0;
}

#else
CFunc0 CustomFunctionTable[] =
{
    (CFunc0) cf_f4711,
    (CFunc0) cf_argc,
    (CFunc0) cf_argv
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
    err = CreateGlueToC( "F4711", i++, C_RETURNS_VALUE, 1 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "ARGC" , i++, C_RETURNS_VALUE, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "ARGV" , i++, C_RETURNS_VALUE, 1 );
    if( err < 0 ) return err;

    return 0;
}
#else
Err CompileCustomFunctions( void ) { return 0; }
#endif

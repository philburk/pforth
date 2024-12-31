/* custom code for pforth (hence Custom Forth = cf)
    This is a hack and for demonstration purposes only.
       It simplifies a few things (like patching of Makefile) 
       but violates rules for production C code (e.g placing definitions in header files and terminating at the 1st sign of trouble).
    Defines helper functions for several examples.
*/

#ifndef CF_HELPERS_H
#define CF_HELPERS_H

#include <stdio.h>       /* fprintf()     */
#include <stdlib.h>      /* exit()        */

static void panic( const char* exitMsg )
{/* Terminates program with panic message on stderr 
    Beware: might mess up the terminal sometimes :-/  (restarting pforth works fine though)
 */
    fprintf(stderr, "\n====> panic! about to exit: %s\n", exitMsg);
    exit(1);
}

static void* safeAlloc( size_t bytes )
{/* allocate memory and panic if that did not succees
 */
    void* result = malloc(bytes);  /* TODO: replace by calloc() ?! */
    if(result==NULL)
        panic("can not allocate memory!");
    return result;
}

static char* to_C_string( cell_t strData, cell_t iStrLen )
{/* copy PForth string to C-string (zero terminated) 
    Don't forget to free() the result!
    TODO: check if there is already defined a similar function in pforth
 */
    char* buf = safeAlloc(iStrLen+1);
    memcpy( buf, (void*)strData, iStrLen );
    buf[iStrLen] = 0;
    return buf;
}

#endif

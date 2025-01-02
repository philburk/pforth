/* custom code for pforth (hence Custom Forth = cf)
    This is a hack and for demonstration purposes only.
       It simplifies a few things (like patching of Makefile) 
       but violates rules for production C code (e.g placing definitions in header files and terminating at the 1st sign of trouble).
    Defines helper functions for several examples.
*/

#ifndef CF_HELPERS_H
#define CF_HELPERS_H

#include <stdlib.h>      /* malloc */
#include <string.h>      /* memcpy */

static char* to_C_string( cell_t strData, cell_t iStrLen )
{/* copy PForth string to C-string (zero terminated) 
    Don't forget to free() the result!
 */
    char* buf = malloc(iStrLen+1);
    if( buf != NULL ) {
        memcpy( buf, (void*)strData, iStrLen );
        buf[iStrLen] = 0;
    }
    return buf;
}

#endif  /* CF_HELPERS_H */
/***************************************************************
** File access routines based on ANSI C (no Unix stuff).
**
** This file is part of pForth
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
****************************************************************/

#include "../pf_all.h"

#ifndef PF_NO_FILEIO

#include <limits.h>		/* For LONG_MAX */

typedef int bool_t;

/* Copy SIZE bytes from File FROM to File TO.  Return non-FALSE on error. */
static bool_t CopyFile( FileStream *From, FileStream *To, long Size)
{
    bool_t Error = TRUE;
    size_t Diff = Size;
    size_t BufSize = 512;
    char *Buffer = pfAllocMem( BufSize );
    if( Buffer != 0 )
    {
	while( Diff > 0 )
	{
	    size_t N = MIN( Diff, BufSize );
	    if( fread( Buffer, 1, N, From ) < N ) goto cleanup;
	    if( fwrite( Buffer, 1, N, To ) < N ) goto cleanup;
	    Diff -= N;
	}
	Error = FALSE;

      cleanup:
	pfFreeMem( Buffer );
    }
    return Error;
}

/* Shrink the file FILE to NEWSIZE.  Return non-FALSE on error.
 *
 * There's no direct way to do this in ANSI C.  The closest thing we
 * have is freopen(3), which truncates a file to zero length if we use
 * "w+b" as mode argument.  So we do this:
 *
 *   1. copy original content to temporary file
 *   2. re-open and truncate FILE
 *   3. copy the temporary file to FILE
 *
 * Unfortunately, "w+b" may not be the same mode as the original mode
 * of FILE.  I don't see a away to avoid this, though.
 *
 * We call freopen with NULL as path argument, because we don't know
 * the actual file-name.  It seems that the trick with path=NULL is
 * not part of C89 but it's in C99.
 */
static bool_t TruncateFile( FileStream *File, long Newsize )
{
    bool_t Error = TRUE;
    if( fseek( File, 0, SEEK_SET ) == 0)
    {
	FileStream *TmpFile = tmpfile();
	if( TmpFile != NULL )
	{
	    if( CopyFile( File, TmpFile, Newsize )) goto cleanup;
	    if( fseek( TmpFile, 0, SEEK_SET ) != 0 ) goto cleanup;
	    if( freopen( NULL, "w+b", File ) == NULL ) goto cleanup;
	    if( CopyFile( TmpFile, File, Newsize )) goto cleanup;
	    Error = FALSE;

	  cleanup:
	    fclose( TmpFile );
	}
    }
    return Error;
}

/* Write DIFF 0 bytes to FILE. Return non-FALSE on error. */
static bool_t ExtendFile( FileStream *File, size_t Diff )
{
    bool_t Error = TRUE;
    size_t BufSize = 512;
    char * Buffer = pfAllocMem( BufSize );
    if( Buffer != 0 )
    {
	pfSetMemory( Buffer, 0, BufSize );
	while( Diff > 0 )
	{
	    size_t N = MIN( Diff, BufSize );
	    if( fwrite( Buffer, 1, N, File ) < N ) goto cleanup;
	    Diff -= N;
	}
	Error = FALSE;
      cleanup:
	pfFreeMem( Buffer );
    }
    return Error;
}

ThrowCode sdResizeFile( FileStream *File, uint64_t Size )
{
    bool_t Error = TRUE;
    if( Size <= LONG_MAX )
    {
	long Newsize = (long) Size;
	if( fseek( File, 0, SEEK_END ) == 0 )
	{
	    long Oldsize = ftell( File );
	    if( Oldsize != -1L )
	    {
		Error = ( Oldsize <= Newsize
			  ? ExtendFile( File, Newsize - Oldsize )
			  : TruncateFile( File, Newsize ));
	    }
	}
    }
    return Error ? THROW_RESIZE_FILE : 0;
}

#endif /* !PF_NO_FILEIO */

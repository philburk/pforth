/***************************************************************
** File access routines based on ANSI C
**   (no more Unix stuff than strictly necessary).
**
** This file is part of pForth
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
****************************************************************/

#include "../pf_all.h"

#ifndef PF_NO_FILEIO

#include <limits.h>     /* For LONG_MAX */

typedef int bool_t;

static bool_t TruncateFile( FileStream *File, long Newsize );  /* Shrink the file FILE to NEWSIZE.  Return non-FALSE on error. */

#if defined( __CYGWIN__) || defined( __FreeBSD__) || defined(__NetBSD__)  || defined(__minix__) /* __unix__ */
/*  Cygwin, FreeBSD, NetBSD and (Manjaro)Linux all define "__unix__", 
      which might also be defined on incompatible platforms as well (so we do not use it).
      Linux is excluded from the if statement in order to test the portable code on build.
    This uses Unix specific APIs to work around problems with the portable implementation.
*/

#include<stdio.h>   /* fileno()  */
#include<unistd.h>  /* ftruncate */

static bool_t TruncateFile( FileStream *File, long Newsize )
{
	bool_t Error = TRUE;
	int fd;
	if(  -1  !=  ( fd = fileno(File) )  )
	{
		if( 0 == ftruncate(fd, Newsize) )
			Error = FALSE;
	}
	return Error;
}

#else   /* __unix__ */

/*
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
 * It does not work on NetBSD and Cygwin though.
*/

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

#endif  /* __unix__ */


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
                Error = (Oldsize <= Newsize)
                        ? ExtendFile( File, Newsize - Oldsize )
                        : TruncateFile( File, Newsize );
            }
        }
    }
    return Error ? THROW_RESIZE_FILE : 0;
}

#endif /* !PF_NO_FILEIO */

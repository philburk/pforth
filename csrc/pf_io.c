/* @(#) pf_io.c 96/12/23 1.12 */
/***************************************************************
** I/O subsystem for PForth based on 'C'
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
****************************************************************
** 941004 PLB Extracted IO calls from pforth_main.c
***************************************************************/

#include "pf_all.h"
#ifdef PF_UNIT_TEST
#include "paging/unittest.h"
#endif

/***************************************************************
** Initialize I/O system.
*/
void ioInit( void )
{
    /* System dependent terminal initialization. */
    sdTerminalInit();
}
void ioTerm( void )
{
    sdTerminalTerm();
}

/***************************************************************
** Send single character to output stream.
*/
void ioEmit( char c )
{
    cell_t Result;

    Result = sdTerminalOut(c);
    if( Result < 0 ) EXIT(1);

    if( gCurrentTask )
    {
        if(c == '\n')
        {
            gCurrentTask->td_OUT = 0;
            sdTerminalFlush();
        }
        else
        {
            gCurrentTask->td_OUT++;
        }
    }
}

/***************************************************************
** Send an entire string..
*/
void ioType( const char *s, cell_t n )
{
    cell_t i;

    for( i=0; i<n; i++)
    {
        ioEmit ( *s++ );
    }
}

/***************************************************************
** Return single character from input device, always keyboard.
*/
cell_t ioKey( void )
{
    cell_t c;
    sdEnableInput();
    c = sdTerminalIn();
    sdDisableInput();
    return c;
}

/**************************************************************
** Receive line from keyboard.
** Return number of characters enterred.
*/
#define SPACE      (0x20)
#define BACKSPACE  (0x08)
#define DELETE     (0x7F)
cell_t ioAccept( char *buffer, cell_t maxChars )
{
    int c;
    cell_t len;
    char *p;

DBUGX(("ioAccept(0x%x, 0x%x)\n", buffer, len ));

    sdEnableInput();

    p = buffer;
    len = 0;
    while(len < maxChars)
    {
        c = sdTerminalIn();
        switch(c)
        {
            case '\r':
            case '\n':
                DBUGX(("EOL\n"));
                goto gotline;
                break;

            case BACKSPACE:
            case DELETE:
                if( len > 0 )  /* Don't go beyond beginning of line. */
                {
                    EMIT(BACKSPACE);
                    EMIT(' ');
                    EMIT(BACKSPACE);
                    p--;
                    len--;
                }
                break;

            default:
                sdTerminalEcho( (char) c );
                *p++ = (char) c;
                len++;
                break;
        }

    }

gotline:
    sdDisableInput();
    sdTerminalEcho( SPACE );

/* NUL terminate line to simplify printing when debugging. */
    if( len < maxChars ) p[len] = '\0';

    return len;
}

#define UNIMPLEMENTED(name) { MSG(name); MSG("is unimplemented!\n"); }

/***********************************************************************************/
/*********** File I/O **************************************************************/
/***********************************************************************************/
#ifdef PF_NO_FILEIO

/* Provide stubs for standard file I/O */

FileStream *PF_STDIN;
FileStream *PF_STDOUT;

cell_t  sdInputChar( FileStream *stream )
{
    UNIMPLEMENTED("sdInputChar");
    TOUCH(stream);
    return -1;
}

FileStream *sdOpenFile( const char *FileName, const char *Mode )
{
    UNIMPLEMENTED("sdOpenFile");
    TOUCH(FileName);
    TOUCH(Mode);
    return NULL;
}
cell_t sdFlushFile( FileStream * Stream  )
{
    TOUCH(Stream);
    return 0;
}
cell_t sdReadFile( void *ptr, cell_t Size, int32_t nItems, FileStream * Stream  )
{
    UNIMPLEMENTED("sdReadFile");
    TOUCH(ptr);
    TOUCH(Size);
    TOUCH(nItems);
    TOUCH(Stream);
    return 0;
}
cell_t sdWriteFile( void *ptr, cell_t Size, int32_t nItems, FileStream * Stream  )
{
    UNIMPLEMENTED("sdWriteFile");
    TOUCH(ptr);
    TOUCH(Size);
    TOUCH(nItems);
    TOUCH(Stream);
    return 0;
}
cell_t sdSeekFile( FileStream * Stream, file_offset_t Position, int32_t Mode )
{
    UNIMPLEMENTED("sdSeekFile");
    TOUCH(Stream);
    TOUCH(Position);
    TOUCH(Mode);
    return 0;
}
file_offset_t sdTellFile( FileStream * Stream )
{
    UNIMPLEMENTED("sdTellFile");
    TOUCH(Stream);
    return 0;
}
cell_t sdCloseFile( FileStream * Stream )
{
    UNIMPLEMENTED("sdCloseFile");
    TOUCH(Stream);
    return 0;
}

cell_t sdDeleteFile( const char *FileName )
{
    UNIMPLEMENTED("sdDeleteFile");
    TOUCH(FileName);
    return -1;
}

cell_t sdRenameFile( const char *OldName, const char *NewName )
{
    UNIMPLEMENTED("sdRenameFile");
    TOUCH(OldName);
    TOUCH(NewName);
    return -1;
}

ThrowCode sdResizeFile( FileStream * File, uint64_t NewSize )
{
    UNIMPLEMENTED("sdResizeFile");
    TOUCH(NewSize);
    return THROW_RESIZE_FILE;
}

#endif

#ifdef PF_UNIT_TEST
int pfQaFileIO(void) {
    int savedNumFailed = pfQaNumFailed;
    cell_t numWritten, numRead, c, result;
    char buffer[128];
    const char kTestFileName[] = "pf_qa_fileio_test.txt";
    const char kTextNumbers[] = "0123456789";
    const int kTextNumbersSize = sizeof(kTextNumbers) - 1; /* skip NUL */
    const char kTextLines[] = "pumpkin\npie";
    const int kTextLinesSize = sizeof(kTextLines) - 1; /* skip NUL */

    printf("pfQaFileIO() called\n");

    /* Create a test file. */
    FileStream *fid = sdOpenFile(kTestFileName, PF_FAM_CREATE_WO);
    ASSERT_NE(NULL, fid);
    numWritten = sdWriteFile(kTextNumbers, 1, kTextNumbersSize, fid);
    ASSERT_EQ(kTextNumbersSize, numWritten);
    sdCloseFile(fid);

    /* Test repositioning within a file. */
    fid = sdOpenFile(kTestFileName, PF_FAM_OPEN_RO);
    ASSERT_NE(NULL, fid);
    ASSERT_EQ(0, sdTellFile(fid));
    ASSERT_EQ(0, sdSeekFile(fid, 5, PF_SEEK_SET));
    ASSERT_EQ(5, sdTellFile(fid));
    c = sdInputChar(fid);
    ASSERT_EQ(kTextNumbers[5], c);
    ASSERT_EQ(6, sdTellFile(fid));
    ASSERT_EQ(0, sdSeekFile(fid, 0, PF_SEEK_END));
    ASSERT_EQ(kTextNumbersSize, sdTellFile(fid));
    ASSERT_EQ(0, sdSeekFile(fid, -6, PF_SEEK_END));
    ASSERT_EQ(kTextNumbersSize - 6, sdTellFile(fid));
    c = sdInputChar(fid);
    ASSERT_EQ(kTextNumbers[4], c);
    ASSERT_EQ(5, sdTellFile(fid));
    ASSERT_EQ(0, sdSeekFile(fid, 2, PF_SEEK_CUR));
    ASSERT_EQ(7, sdTellFile(fid));
    ASSERT_EQ(0, sdSeekFile(fid, -4, PF_SEEK_CUR));
    ASSERT_EQ(3, sdTellFile(fid));

    /* Test repositioning past the end of the file.
     * fseek() behaves this way! */
    ASSERT_EQ(0, sdSeekFile(fid, 5, PF_SEEK_END));
    ASSERT_EQ(kTextNumbersSize + 5, sdTellFile(fid));


    /* Test repositioning past the beginning of the file.
     * fseek() behaves this way! */
    ASSERT_EQ(0, sdSeekFile(fid, 3, PF_SEEK_SET));
    ASSERT_EQ(3, sdTellFile(fid));
    ASSERT_EQ(-1, sdSeekFile(fid, -5, PF_SEEK_SET));
    ASSERT_EQ(3, sdTellFile(fid));

    sdCloseFile(fid);

    /* Cleanup */
    result = sdDeleteFile(kTestFileName);
    ASSERT_EQ(0, result);
    result = sdDeleteFile(kTestFileName); /* should fail */
    ASSERT_NE(0, result);
    fid = sdOpenFile(kTestFileName, PF_FAM_OPEN_RO); /* should fail */
    ASSERT_EQ(NULL, fid);

    printf("pfQaFileIO() finished\n");

error:
    PFQA_PRINT_RESULT;
    return pfQaNumFailed - savedNumFailed;
}
#endif


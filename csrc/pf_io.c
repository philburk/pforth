/* @(#) pf_io.c 96/12/23 1.12 */
/***************************************************************
** I/O subsystem for PForth based on 'C'
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
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
****************************************************************
** 941004 PLB Extracted IO calls from pforth_main.c
***************************************************************/

#include "pf_all.h"


/***************************************************************
** Initialize I/O system.
*/
void ioInit( void )
{
	/* System dependant terminal initialization. */
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
	int len;
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
cell_t sdSeekFile( FileStream * Stream, cell_t Position, int32_t Mode ) 
{ 
	UNIMPLEMENTED("sdSeekFile");
	TOUCH(Stream);
	TOUCH(Position);
	TOUCH(Mode);
	return 0; 
}
cell_t sdTellFile( FileStream * Stream ) 
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

FileStream *sdDeleteFile( const char *FileName )
{
	UNIMPLEMENTED("sdDeleteFile");
	TOUCH(FileName);
	return NULL;
}
#endif


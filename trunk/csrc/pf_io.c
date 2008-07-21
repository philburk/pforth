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
	int32 Result;
	
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
void ioType( const char *s, int32 n )
{
	int32 i;

	for( i=0; i<n; i++)
	{
		ioEmit ( *s++ );
	}
}

/***************************************************************
** Return single character from input device, always keyboard.
*/
cell ioKey( void )
{
	cell c;
	sdEnableInput();
	c = sdTerminalIn();
	sdDisableInput();
	return c;
}

/**************************************************************
** Receive line from keyboard.
** Return number of characters enterred.
*/
#define BACKSPACE  (8)
#define DELETE     (0x7F)
cell ioAccept( char *buffer, cell maxChars )
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

int32  sdInputChar( FileStream *stream )
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
int32 sdFlushFile( FileStream * Stream  )
{
	TOUCH(Stream);
	return 0;
}
int32 sdReadFile( void *ptr, int32 Size, int32 nItems, FileStream * Stream  ) 
{ 
	UNIMPLEMENTED("sdReadFile");
	TOUCH(ptr);
	TOUCH(Size);
	TOUCH(nItems);
	TOUCH(Stream);
	return 0; 
}
int32 sdWriteFile( void *ptr, int32 Size, int32 nItems, FileStream * Stream  )
{ 
	UNIMPLEMENTED("sdWriteFile");
	TOUCH(ptr);
	TOUCH(Size);
	TOUCH(nItems);
	TOUCH(Stream);
	return 0; 
}
int32 sdSeekFile( FileStream * Stream, int32 Position, int32 Mode ) 
{ 
	UNIMPLEMENTED("sdSeekFile");
	TOUCH(Stream);
	TOUCH(Position);
	TOUCH(Mode);
	return 0; 
}
int32 sdTellFile( FileStream * Stream ) 
{ 
	UNIMPLEMENTED("sdTellFile");
	TOUCH(Stream);
	return 0; 
}
int32 sdCloseFile( FileStream * Stream ) 
{ 
	UNIMPLEMENTED("sdCloseFile");
	TOUCH(Stream);
	return 0; 
}
#endif


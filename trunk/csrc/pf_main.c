/* @(#) pf_main.c 98/01/26 1.2 */
/***************************************************************
** Forth based on 'C'
**
** main() routine that demonstrates how to call PForth as
** a module from 'C' based application.
** Customize this as needed for your application.
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
***************************************************************/

#if (defined(PF_NO_STDIO) || defined(PF_EMBEDDED))
	#define NULL  ((void *) 0)
	#define ERR(msg) /* { printf msg; } */
#else
	#include <stdio.h>
	#define ERR(msg) { printf msg; }
#endif

#include "pforth.h"

#ifndef PF_DEFAULT_DICTIONARY
#define PF_DEFAULT_DICTIONARY "pforth.dic"
#endif

#ifdef __MWERKS__
	#include <console.h>
	#include <sioux.h>
#endif

#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif

#ifdef PF_EMBEDDED
int main( void )
{
    char IfInit = 0; 
    const char *DicName = NULL;
    const char *SourceName = NULL;
    pfMessage("\npForth Embedded\n");
    return pfDoForth( DicName, SourceName, IfInit);
}
#else

int main( int argc, char **argv )
{
#ifdef PF_STATIC_DIC
	const char *DicName = NULL;
#else /* PF_STATIC_DIC */
	const char *DicName = PF_DEFAULT_DICTIONARY;
#endif /* !PF_STATIC_DIC */

	const char *SourceName = NULL;
	char IfInit = FALSE;
	char *s;
	cell_t i;
	int Result;

/* For Metroworks on Mac */
#ifdef __MWERKS__
	argc = ccommand(&argv);
#endif
	
	pfSetQuiet( FALSE );
/* Parse command line. */
	for( i=1; i<argc; i++ )
	{
		s = argv[i];

		if( *s == '-' )
		{
			char c;
			s++; /* past '-' */
			c = *s++;
			switch(c)
			{
			case 'i':
				IfInit = TRUE;
				DicName = NULL;
				break;
				
			case 'q':
				pfSetQuiet( TRUE );
				break;
				
			case 'd':
				if( *s != '\0' ) DicName = s;
				/* Allow space after -d (Thanks Aleksej Saushev) */
				/* Make sure there is another argument. */
				else if( (i+1) < argc )
				{
					DicName = argv[++i];
				}
				if (DicName == NULL || *DicName == '\0')
				{
					DicName = PF_DEFAULT_DICTIONARY;
				}
				break;
				
			default:
				ERR(("Unrecognized option!\n"));
				ERR(("pforth {-i} {-q} {-dfilename.dic} {sourcefilename}\n"));
				Result = 1;
				goto on_error;
				break;
			}
		}
		else
		{
			SourceName = s;
		}
	}
/* Force Init */
#ifdef PF_INIT_MODE
	IfInit = TRUE;
	DicName = NULL;
#endif

#ifdef PF_UNIT_TEST
	if( (Result = pfUnitTest()) != 0 )
	{
		ERR(("pForth stopping on unit test failure.\n"));
		goto on_error;
	}
#endif
	
	Result = pfDoForth( DicName, SourceName, IfInit);

on_error:
	return Result;
}

#endif  /* PF_EMBEDDED */



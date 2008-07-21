/*  @(#) pf_unix.h 98/01/28 1.4 */
#ifndef _pf_embedded_h
#define _pf_embedded_h

/***************************************************************
** Embedded System include file for PForth, a Forth based on 'C'
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

#ifndef PF_NO_CLIB
	#include <string.h>    /* Needed for strlen(), memcpy(), and memset(). */
	#include <stdlib.h>    /* Needed for exit(). */
#endif

#ifdef PF_NO_STDIO
	#define NULL  ((void *) 0)
	#define EOF   (-1)
#else
	#include <stdio.h>
#endif

#ifdef PF_SUPPORT_FP
	#include <math.h>

	#ifndef PF_USER_FP
		#include "pf_float.h"
	#else
		#include PF_USER_FP
	#endif
#endif

#endif /* _pf_embedded_h */

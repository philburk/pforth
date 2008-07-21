/* @(#) pf_types.h 96/12/18 1.3 */
#ifndef _pf_types_h
#define _pf_types_h

/***************************************************************
** Type declarations for PForth, a Forth based on 'C'
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

/***************************************************************
** Type Declarations
***************************************************************/

#ifndef uint32
	typedef unsigned long uint32;
#endif
#ifndef int16
	typedef signed short int16;
#endif
#ifndef uint16
	typedef unsigned short uint16;
#endif
#ifndef int8
	typedef signed char int8;
#endif
#ifndef uint8
	typedef unsigned char uint8;
#endif
#ifndef Err
	typedef long Err;
#endif

typedef int32  cell;
typedef uint32 ucell;
typedef cell  *dicptr;

typedef char  ForthString;
typedef char *ForthStringPtr;

#endif /* _pf_types_h */

/* @(#) pf_mem.h 98/01/26 1.3 */
#ifndef _pf_mem_h
#define _pf_mem_h

/***************************************************************
** Include file for PForth Fake Memory Allocator
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** The pForth software code is dedicated to the public domain,
** and any third party may reproduce, distribute and modify
** code is provided on an "as is" basis without any warranty
** of any kind, including, without limitation, the implied
** warranties of merchantability and fitness for a particular
***************************************************************/

#ifdef PF_NO_MALLOC

	#ifdef __cplusplus
	extern "C" {
	#endif

	void  pfInitMemoryAllocator( void );
	char *pfAllocMem( cell_t NumBytes );
	void  pfFreeMem( void *Mem );

	#ifdef __cplusplus
	}   
	#endif

#else

	#ifdef PF_USER_MALLOC
/* Get user prototypes or macros from include file.
** API must match that defined above for the stubs.
*/
		#include PF_USER_MALLOC
	#else
		#define pfInitMemoryAllocator()
		#define pfAllocMem malloc
		#define pfFreeMem free
	#endif
	
#endif /* PF_NO_MALLOC */

#endif /* _pf_mem_h */

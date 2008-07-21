/* @(#) pf_win32.h 98/01/26 1.2 */
#ifndef _pf_win32_h
#define _pf_win32_h

#include <conio.h>

/***************************************************************
** WIN32 dependant include file for PForth, a Forth based on 'C'
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

/* Include as PF_USER_INC2 for PCs */

/* Modify some existing defines. */

/*
** The PC will insert LF characters into the dictionary files unless
** we use "b" mode!
*/
#undef PF_FAM_CREATE
#define PF_FAM_CREATE  ("wb+")

#undef PF_FAM_OPEN_RO
#define PF_FAM_OPEN_RO  ("rb")

#undef PF_FAM_OPEN_RW
#define PF_FAM_OPEN_RW  ("rb+")

#endif /* _pf_win32_h */

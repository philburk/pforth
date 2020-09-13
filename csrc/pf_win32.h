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

/* @(#) pf_words.h 96/12/18 1.7 */
#ifndef _pforth_words_h
#define _pforth_words_h

/***************************************************************
** Include file for PForth Words
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

#ifdef __cplusplus
extern "C" {
#endif

void ffDot( cell_t n );
void ffDotHex( cell_t n );
void ffDotS( void );
cell_t ffSkip(const char *AddrIn, cell_t Cnt, char c, const char **AddrOut);
cell_t ffScan(const char *AddrIn, cell_t Cnt, char c, const char **AddrOut);

size_t ffReadFile( vm_address_t vp, size_t Size, size_t nItems, FileStream * Stream  );
size_t ffWriteFile( vm_address_t vp, size_t Size, size_t nItems, FileStream * Stream  );

#ifdef __cplusplus
}
#endif

#endif /* _pforth_words_h */

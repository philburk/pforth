/* @(#) search.h 15/04/09 1.0 */
#ifndef _pf_search_h
#define _pf_search_h
/***************************************************************
** search order for PForth based on 'C'
**
** Author: Hannu Vuolasaho
** Copyright 2015 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** The pForth software code is dedicated to the public domain,
** and any third party may reproduce, distribute and modify
** the pForth software code or any derivative works thereof
** without any compensation or license.  The pForth software
** code is provided on an "as is" basis without any warranty
** of any kind, including, without limitation, the implied
** warranties of merchantability and fitness for a particular
** purpose and their equivalents under the laws of any jurisdiction.
**/

#ifdef PF_SUPPORT_WORDLIST
#define PF_WORDLIST_EXPORT_FUNCTIONS (2)

/* Search order and word list arrays */

extern cell_t searchFirstIndex;
extern cell_t wordLists;

/* compilationIndex is wordLists[compilationIdnex], head of comp. list. */
extern cell_t compilationIndex;

/* (init-wordlists) ( search_addr search_index wl_addr comp_index -- ) */
void ffInitWordLists( cell_t search_addr, cell_t search_index,
                      cell_t wl_addr, cell_t comp_index );
/* search-wordlist ( c-addr u wid -- 0 | xt 1 | xt -1 ) */
cell_t ffSearchWordList( cell_t c_addr, cell_t u, cell_t wid);

/* Helper function. 
 * Get the head of wordlist in search order index 'index' */
cell_t getWordList( cell_t index );

#else
#define PF_WORDLIST_EXPORT_FUNCTIONS (0)
#endif
#endif

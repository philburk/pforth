/* @(#) search.c 15/04/09 1.0 */
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
#include "pf_all.h"
#ifdef PF_SUPPORT_WORDLIST

/* Search order and word list arrays */
cell_t arrSearchOrder;

/* global search order start index. points wl.order.first on forth. */
cell_t gVarWlOrderFirst;
cell_t gVarWordLists;
/* gVarWlCompileIndex is gVarWordLists[compilationIdnex], head of comp. list. */
cell_t gVarWlCompileIndex;



/* (init-wordlists) ( search_addr search_index wl_addr comp_index -- ) */
void ffInitWordLists( cell_t search_addr, cell_t search_index,
                      cell_t wl_addr, cell_t comp_index )
{
        gVarWlCompileIndex = comp_index;
        gVarWordLists = wl_addr;
        gVarWlOrderFirst = search_index;
        arrSearchOrder = search_addr;
        /* Debug Stuff. remove. */
        MSG_NUM_D("comp ind ", gVarWlCompileIndex);
        MSG_NUM_D("comp ind * ", *(cell_t *)gVarWlCompileIndex);
        MSG_NUM_D("wl ", gVarWordLists );
        MSG_NUM_D("wl * ", *(cell_t *)gVarWordLists );
        MSG_NUM_D("wl * * ",  *(cell_t *)(*(cell_t *)gVarWordLists));
        MSG_NUM_D("wl+1 * * ",  *(cell_t *)(*(cell_t *)gVarWordLists+1));
        MSG_NUM_D("first ", gVarWlOrderFirst);
        MSG_NUM_D("first * ", *(cell_t *) gVarWlOrderFirst);
        MSG_NUM_D("order ", arrSearchOrder);
        MSG_NUM_D("order * ", *(cell_t *)arrSearchOrder);
        MSG_NUM_D("order name * ",  NAMEREL_TO_ABS((*(cell_t *)arrSearchOrder)));
        MSG_NUM_D("order code * ",  CODEREL_TO_ABS((*(cell_t *)arrSearchOrder)));
        MSG_NUM_D("order code * * ",  *(cell_t *)(CODEREL_TO_ABS((*(cell_t *)arrSearchOrder))));
}
cell_t getWordList( cell_t index )
{
  cell_t temp_addr, *tmp_arr;
        if(gVarWordLists)
        {
                /* Don't underflow search */
                if( index < 0 ) return (cell_t) NULL;
                /* Address to wordlist */
                tmp_arr = (cell_t *) arrSearchOrder;
                temp_addr = tmp_arr[index];
                if(temp_addr)
                {
                        temp_addr = CODEREL_TO_ABS(temp_addr);
                        return *(cell_t *)temp_addr;
                }
                else
                {
                        /* Empty wordlist in search order */
                        return (cell_t) NULL;
                }
        }
        else
        {
                return gVarContext;
        }
}

/* This should be written in forth */
/* search-wordlist ( c-addr u wid -- 0 | xt 1 | xt -1 ) */
cell_t ffSearchWordList( cell_t c_addr, cell_t u, cell_t wid)
{
        cell_t Searching = TRUE;
        cell_t Result = 0;
        uint8_t NameLen;
        const char *NameField;

        if( wid == 0 || !(*((cell_t *) (CODEREL_TO_ABS(wid))) )) return 0;
        /* wid is code relative address of wordlists
         * referencing give content of gVarContext of
         * compilation time of last word  in word list*/
        NameField = (ForthString *) *((cell_t *) (CODEREL_TO_ABS(wid)) );
        do
        {
                NameLen = (uint8_t) ((ucell_t)(*NameField) & MASK_NAME_SIZE);
                if( ((*NameField & FLAG_SMUDGE) == 0) &&
                    (NameLen == u) &&
                    ffCompareTextCaseN( NameField +1, (const char *) c_addr, u ) )
                {
                        PUSH_DATA_STACK(NameToToken(NameField)); /* XT to stack */
                        Result = ((*NameField) & FLAG_IMMEDIATE) ? 1 : -1;
                        Searching = FALSE;
                }
                else
                {
                        NameField = NameToPrevious( NameField );
			if( NameField == NULL )
			{
                                Searching = FALSE;
                        }
                }
        }while(Searching);
        return Result;
}

#endif

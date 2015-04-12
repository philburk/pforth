\ @(#) case.fth 15/04/10 0.1
\ Search-Order wordset
\
\
\
\ Author: Hannu Vuolasaho
\ Copyright 2015 3DO, Phil Burk, Larry Polansky, Devid Rosenboom
\
\ The pForth software code is dedicated to the public domain,
\ and any third party may reproduce, distribute and modify
\ the pForth software code or any derivative works thereof
\ without any compensation or license.  The pForth software
\ code is provided on an "as is" basis without any warranty
\ of any kind, including, without limitation, the implied
\ warranties of merchantability and fitness for a particular
\ purpose and their equivalents under the laws of any jurisdiction.

anew task-search.fth

\ This constant defines how many wordlist you get. Increase it
\ if more lists needed.
16 constant WORDLISTS

\ Exeption codes
-49 constant ERR_SEARCH_OVERFLOW
-50 constant ERR_SEARCH_UNDERFLOW

create forth-wordlist WORDLISTS cells allot
create [searchorder] WORDLISTS cells allot
variable wl.compile.index
variable wl.order.first
variable wl.used

    
\ works
    
: wl.check ( index -- , throw if out of bounds )
    dup WORDLISTS >= ERR_SEARCH_OVERFLOW and throw
    0< ERR_SEARCH_UNDERFLOW and throw
;

\ works

: wl.check.wid ( wid -- throw if out of bounds )
    forth-wordlist if.use->rel dup wl.used @
    cells + ( wid min max )
    >r over ( wid min wid ) > ERR_SEARCH_UNDERFLOW and throw
    r> > ERR_SEARCH_OVERFLOW and throw
;

\ works

: get-current ( -- wid , compilation word list )
    wl.compile.index @ cells forth-wordlist + if.use->rel
;

\ works

: get-order ( -- widn ... wid1 n )
    wl.order.first @ 1+ 0
    ?do
        wl.order.first @ i - cells
        [searchorder] + @
    loop
    wl.order.first @ 1+
;

\ works
: set-current ( wid -- , compilation word list to wid )
    \ check wid
    dup wl.check.wid

    \ make index and put it under
    forth-wordlist use->rel -
    cell / ( wid index-to-forth-wordlist )

    wl.compile.index !
;

: definitions ( -- )
    \ get first in search order and set it to compilation
    wl.order.first @ ( index )
    cells [searchorder] + @ ( wid )
    set-current
;

\ works

: set-order ( widn ... wid1 n -- , Set the search order )
    dup -1 =
    if
        drop forth-wordlist if.use->rel [searchorder] !
        0 
    else
        dup wl.check
        dup 0=
        if
            drop -1
        else
            dup >r 0 do
                i cells
                [searchorder] + !
            loop
            r> 1-
        then
    then
    wl.order.first !
;

\ works

: wordlist ( -- wid , Create a new empty word list )
    wl.used @ 1+ dup wl.check dup wl.used ! ( index , incerment and store )
    cells forth-wordlist + dup 0 swap ! ( addr , zero the wordlist )
    if.use->rel ( wid )
;

: also ( -- , copy first search wordlist to first in search order )
    get-order over swap 1+ set-order
;

: forth ( -- , Remove first wordlist and put forth-wordlist as first )
    get-order nip forth-wordlist if.use->rel swap set-order
;

: only ( -- , Set search order to forth-wordlist )
    -1 set-order
;

\ works
: order ( -- , print search order wordlist )
    get-order 0 ?do
        i . ." 0x" .hex cr
    loop
;

\ works
: previous ( -- ) get-order nip 1- set-order ;

: init-wordlists ( -- , put forth context to forth-wordlist )
    WORDLISTS 1 do
        0 i cells forth-wordlist + !
        0 i cells [searchorder] + !
    loop
    context @ forth-wordlist !
    forth-wordlist if.use->rel [searchorder] !
    \ send variables to C
    [searchorder] wl.order.first forth-wordlist  wl.compile.index
    (init-wordlists)
;
: auto.init
    auto.init init-wordlists
;


\ implemented in C kernel
\ : search-wordlist ( c-addr u wid -- 0 | xt 1 | xt -1 )
    \ Find the definition identified by the string
    \ c-addr u in the word list identified by wid
\ ;
\ : find  ( c-addr -- c-addr 0 | xt 1 | xt -1 )
    \ Find named definitions from all word lists
\ ;

: wordlists? ( -- )
    cr ." wordlists:" forth-wordlist dup use->rel .hex .hex cr
    WORDLISTS 0
    do
        forth-wordlist i cells + use->rel .hex i wl.used @ > if ." wl not in use: " then
        i dup . cells forth-wordlist + @ .hex cr
    loop
    ." search order:" [searchorder] dup use->rel .hex .hex cr
    WORDLISTS 1+ 1 ?do
        WORDLISTS i - cells
        [searchorder] + dup .hex @
        wl.order.first @  WORDLISTS i - < if ." order not in use: " then
        WORDLISTS i - . .hex cr
    loop
;

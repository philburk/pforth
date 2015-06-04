\ @(#) search.fth 15/05/20 0.2
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

\ Arrays for word lists and search order
\ Also available on C side.
create [wordlists] WORDLISTS cells allot
create [searchorder] WORDLISTS cells allot
variable wl.compile.index \ [wordlists] index which is compilation list
variable wl.order.first \ Start index of [searchorder] search is decending.

\ Keep track which wordlists are already given.
variable wl.used

variable wl.offset

: wl.check ( index -- , throw if out of bounds )
    dup WORDLISTS >= ERR_SEARCH_OVERFLOW and throw
    0< ERR_SEARCH_UNDERFLOW and throw
;

: wl.check.wid ( wid -- throw if out of bounds )
    [wordlists] if.use->rel dup wl.used @
    cells + ( wid min max )
    >r over ( wid min wid ) > ERR_SEARCH_UNDERFLOW and throw
    r> > ERR_SEARCH_OVERFLOW and throw
;

: get-current ( -- wid , compilation word list )
    wl.compile.index @ cells [wordlists] + if.use->rel
;

: get-order ( -- widn ... wid1 n )
    wl.order.first @ 1+ 0
    ?do
        i cells [searchorder] + @
    loop
    wl.order.first @ 1+
;

: set-current ( wid -- , compilation word list to wid )
    \ check wid
    dup wl.check.wid

    \ make index and put it under
    [wordlists] use->rel -
    cell / ( wid index-to-[wordlists] )

    wl.compile.index !
;

: definitions ( -- )
    \ get first in search order and set it to compilation
    wl.order.first @ ( index )
    cells [searchorder] + @ ( wid )
    set-current
;

: set-order ( widn ... wid1 n -- , Set the search order )
    dup -1 =
    if
        drop [wordlists] if.use->rel [searchorder] !
        0
    else
        dup wl.check
        dup 0=
        if
            drop -1
        else
            dup 1- swap
            0 do
                dup i - cells ( wid1 ... widn n offset )
                [searchorder] + ( wid1 ... widn n addr )
                rot swap !
            loop
        then
    then
    wl.order.first !
;

: wordlist ( -- wid , Create a new empty word list )
    wl.used @ 1+ dup wl.check dup wl.used ! ( index , incerment and store )
    cells [wordlists] + dup 0 swap ! ( addr , zero the wordlist )
    if.use->rel ( wid )
;

: also ( -- , copy first search wordlist to first in search order )
    get-order over swap 1+ set-order
;

: forth ( -- , Remove first wordlist and put [wordlists] as first )
    get-order nip [wordlists] if.use->rel swap set-order
;

: only ( -- , Set search order to forth-wordlist )
    -1 set-order
;

: order ( -- , print search order wordlist )
    get-order 0 ?do
        i . ." 0x" .hex cr
    loop
;

: previous ( -- ) get-order nip 1- set-order ;

: init-wordlists ( -- , put forth context to [wordlists] )
    WORDLISTS 0 do
        0 i cells [wordlists] + !
        0 i cells [searchorder] + !
    loop
    context @ [wordlists] !
    [wordlists] if.use->rel [searchorder] !
;

init-wordlists

: init-wordlists ( -- , put forth context to [wordlists] and send to C )
    context @ [wordlists] !
    [wordlists] if.use->rel [searchorder] !
    \ Fix dictionaries.
    WORDLISTS 1 do
        i cells [wordlists] + dup @ dup 0<> ( addr val flag )
        if
            wl.offset @ - namebase + swap ! ( )
        else
            2drop
        then
    loop
    [searchorder] wl.order.first [wordlists]  wl.compile.index
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
: forth-wordlist ( -- wid , Convert variable [wordlists] to wid )
    [wordlists] if.use->rel
;

: seal ( -- , Make the top of the search order only word list in search order )
    get-order over >r 0 do drop loop r> 1 set-order
;

\ As values are in [wordlists] in usable format, save namebase to wl.offset
\ so next time it is possible to use them.

\ This works as this file is included by loadp4th.fth
\ later than save-forth in system.fth

\ redefine save-forth
: save-forth ( $name -- )
    namebase wl.offset ! save-forth
;

\ Now there yoy go. Use the wordlist
init-wordlists
\ debugiging words
\ Wordlists could be included earlier. misc2.fth provides
\ .hex which is limiting word inside wordlists?.

 true [if]
\ false [if]
: wordlists? ( -- )
    cr ." wordlists:"cr
    WORDLISTS 0
    do
        i ." index: " . [wordlists] i cells +
        dup ." use: " .hex
        dup ." rel: " use->rel .hex
        @ ." val: " .hex
        i wl.used @ > if ." wl not in use. " then
        cr
    loop
    cr ." compilation list index: "  wl.compile.index @ .
    ." and list: " get-current .hex cr
    ." search order:" cr
    WORDLISTS 1+ 1
    ?do
        WORDLISTS i -
        dup ." index: " .
        cells [searchorder] +
        dup ." use: " .hex
        dup ." rel: " use->rel .hex
        @ ." val: " .hex wl.order.first @  WORDLISTS i - < if ." order not in use. " then
        cr
    loop
;


VARIABLE wid1
wordlist wid1 !

wid1 @ set-current
: hello ." Hello wid1" cr ;

forth-wordlist set-current
: hello ." Hello forth" cr ;

[then]
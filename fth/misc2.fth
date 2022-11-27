\ @(#) misc2.fth 98/01/26 1.2
\ Utilities for PForth extracted from HMSL
\
\ Author: Phil Burk
\ Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
\
\ Permission to use, copy, modify, and/or distribute this
\ software for any purpose with or without fee is hereby granted.
\
\ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
\ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
\ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
\ THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
\ CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
\ FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
\ CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
\ OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
\
\ 00001 9/14/92 Added call, 'c w->s
\ 00002 11/23/92 Moved redef of : to loadcom.fth

anew task-misc2.fth

: 'N  ( <name> -- , make 'n state smart )
    bl word find
    IF
        state @
        IF  namebase - ( make nfa relocatable )
            [compile] literal   ( store nfa of word to be compiled )
            compile namebase+
        THEN
    THEN
; IMMEDIATE

: ?LITERAL  ( n -- , do literal if compiling )
    state @
    IF [compile] literal
    THEN
;

: 'c ( <name> -- xt , state sensitive ' )
    ' ?literal
; immediate

variable if-debug

: ? ( address -- , fatch from address and print value )
    @ .
;

decimal
create MSEC-DELAY 100000 ,   \ calibrate this for your system
: (MSEC.SPIN) ( #msecs -- , busy wait, not accurate )
    0 max   \ avoid endless loop
    0
    ?do  msec-delay @ 0
        do loop
    loop
;

: (MSEC) ( millis -- )
    dup (sleep) \ call system sleep in kernel
    IF
        ." (SLEEP) failed or not implemented! Using (MSEC.SPIN)" CR
        (msec.spin)
    ELSE
        drop
    THEN
;

defer msec

\ (SLEEP) uses system sleep functions to actually sleep.
\ Use (MSEC.SPIN) on embedded systems that do not support Win32 Sleep() posix usleep().
1 (SLEEP) [IF]
    ." (SLEEP) failed or not implemented! Use (MSEC.SPIN) for MSEC" CR
    ' (msec.spin) is msec
[ELSE]
    ' (msec) is msec
[THEN]

: MS ( msec -- , sleep, ANS standard )
    msec
;

: SHIFT ( val n -- val<<n )
    dup 0<
    IF negate arshift
    ELSE lshift
    THEN
;

variable rand-seed here rand-seed !
: random ( -- random_number )
    rand-seed @
    31421 * 6927 +
    65535 and dup rand-seed !
;
: choose  ( range -- random_number , in range )
    random * -16 shift
;

: wchoose ( hi lo -- random_number )
    tuck - choose +
;


\ sort top two items on stack.
: 2sort ( a b -- a<b | b<a , largest on top of stack)
    2dup >
    if swap
    then
;

\ sort top two items on stack.
: -2sort ( a b -- a>b | b>a , smallest on top of stack)
    2dup <
    if swap
    then
;

: barray  ( #bytes -- ) ( index -- addr )
    create allot
    does>  +
;

: warray  ( #words -- ) ( index -- addr )
    create 2* allot
    does> swap 2* +
;

: array  ( #cells -- ) ( index -- addr )
    create cell* allot
    does> swap cell* +
;

: .bin  ( n -- , print in binary )
    base @ binary swap . base !
;
: .dec  ( n -- )
    base @ decimal swap . base !
;
: .hex  ( n -- )
    base @ hex swap . base !
;

: B->S ( c -- c' , sign extend byte )
    dup $ 80 and
    IF
        [ $ 0FF invert ] literal or
    ELSE
        $ 0FF and
    THEN
;
: W->S ( 16bit-signed -- cell-signed )
    dup $ 8000 and
    IF
        [ $ 0FFFF invert ] literal or
    ELSE
        $ 0FFFF and
    THEN
;

: WITHIN { n1 n2 n3 -- flag }
    n2 n3 <=
    IF
        n2 n1 <=
        n1 n3 <  AND
    ELSE
        n2 n1 <=
        n1 n3 <  OR
    THEN
;

: MOVE ( src dst num -- )
    >r 2dup - 0<
    IF
        r> CMOVE>
    ELSE
        r> CMOVE
    THEN
;

: ERASE ( caddr num -- )
    dup 0>
    IF
        0 fill
    ELSE
        2drop
    THEN
;

: BLANK ( addr u -- , set memory to blank )
    DUP 0>
    IF
        BL FILL
    ELSE
        2DROP
    THEN
;

\ Obsolete but included for CORE EXT word set.
: QUERY REFILL DROP ;
VARIABLE SPAN
: EXPECT accept span ! ;
: TIB source drop ;


: UNUSED ( -- unused , dictionary space )
    CODELIMIT HERE -
;

: MAP  ( -- , dump interesting dictionary info )
    ." Code Segment" cr
    ."    CODEBASE           = " codebase .hex cr
    ."    HERE               = " here .hex cr
    ."    CODELIMIT          = " codelimit .hex cr
    ."    Compiled Code Size = " here codebase - . cr
    ."    CODE-SIZE          = " code-size @ . cr
    ."    Code Room UNUSED   = " UNUSED . cr
    ." Name Segment" cr
    ."    NAMEBASE           = " namebase .hex cr
    ."    HEADERS-PTR @      = " headers-ptr @ .hex cr
    ."    NAMELIMIT          = " namelimit .hex cr
    ."    CONTEXT @          = " context @ .hex cr
    ."    LATEST             = " latest .hex  ."  = " latest id. cr
    ."    Compiled Name size = " headers-ptr @ namebase - . cr
    ."    HEADERS-SIZE       = " headers-size @ . cr
    ."    Name Room Left     = " namelimit headers-ptr @ - . cr
;


\ Search for substring S2 in S1
: SEARCH { addr1 cnt1 addr2 cnt2 | addr3 cnt3 flag --  addr3 cnt3 flag }
\ ." Search for " addr2 cnt2 type  ."  in "  addr1 cnt1 type cr
\ if true, s1 contains s2 at addr3 with cnt3 chars remaining
\ if false, s3 = s1
    addr1 -> addr3
    cnt1 -> cnt3
    cnt1 cnt2 < not
    IF
        cnt1 cnt2 - 1+ 0
        DO
            true -> flag
            cnt2 0
            ?DO
                addr2 i chars + c@
                addr1 i j + chars + c@ <> \ mismatch?
                IF
                    false -> flag
                    LEAVE
                THEN
            LOOP
            flag
            IF
                addr1 i chars + -> addr3
                cnt1 i - -> cnt3
                LEAVE
            THEN
        LOOP
    THEN
    addr3 cnt3 flag
;

private{

: env= ( c-addr u c-addr1 u1 x -- x true true | c-addr u false )
    { x } 2over compare 0= if 2drop x true true else false then
;

: 2env= ( c-addr u c-addr1 u1 x y -- x y true true | c-addr u false )
    { x y } 2over compare 0= if 2drop x y true true else false then
;

0 invert constant max-u
0 invert 1 rshift constant max-n

}private

: ENVIRONMENT? ( c-addr u -- false | i*x true )
    s" /COUNTED-STRING"      255 env= if exit then
    s" /HOLD"                128 env= if exit then \ same as PAD
    s" /PAD"                 128 env= if exit then
    s" ADDRESS-UNITS-BITS"     8 env= if exit then
    s" FLOORED"            false env= if exit then
    s" MAX-CHAR"             255 env= if exit then
    s" MAX-D"       max-n max-u 2env= if exit then
    s" MAX-N"              max-n env= if exit then
    s" MAX-U"              max-u env= if exit then
    s" MAX-UD"      max-u max-u 2env= if exit then
    s" RETURN-STACK-CELLS"   512 env= if exit then \ DEFAULT_RETURN_DEPTH
    s" STACK-CELLS"          512 env= if exit then \ DEFAULT_USER_DEPTH
    \ FIXME: maybe define those:
    \ s" FLOATING-STACK"
    \ s" MAX-FLOAT"
    \ s" #LOCALS"
    \ s" WORDLISTS"
    2drop false
;

privatize

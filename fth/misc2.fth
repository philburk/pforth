\ @(#) misc2.fth 98/01/26 1.2
\ Utilities for PForth extracted from HMSL
\
\ Author: Phil Burk
\ Copyright 1994 3DO, Phil Burk, Larry Polansky, Devid Rosenboom
\
\ The pForth software code is dedicated to the public domain,
\ and any third party may reproduce, distribute and modify
\ the pForth software code or any derivative works thereof
\ without any compensation or license.  The pForth software
\ code is provided on an "as is" basis without any warranty
\ of any kind, including, without limitation, the implied
\ warranties of merchantability and fitness for a particular
\ purpose and their equivalents under the laws of any jurisdiction.
\
\ 00001 9/14/92 Added call, 'c w->s
\ 00002 11/23/92 Moved redef of : to loadcom.fth

anew task-misc2.fth

: 'N  ( <name> -- , make 'n state smart )
	bl word find
	IF
		state @
		IF	namebase - ( make nfa relocatable )
			[compile] literal	( store nfa of word to be compiled )
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

decimal
create msec-delay 10000 ,  ( default for SUN )
: (MSEC) ( #msecs -- )
    0
    do  msec-delay @ 0
        do loop
    loop
;

defer msec
' (msec) is msec

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
		$ FFFFFF00 or
	ELSE
		$ 000000FF and
	THEN
;
: W->S ( 16bit-signed -- 32bit-signed )
	dup $ 8000 and
	if
		$ FFFF0000 or
	ELSE
		$ 0000FFFF and
	then
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


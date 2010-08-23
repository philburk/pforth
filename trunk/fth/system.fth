: FIRST_COLON ;

: LATEST context @ ;

: FLAG_IMMEDIATE 64 ;

: IMMEDIATE
        latest dup c@ flag_immediate OR
        swap c!
;

: (   41 word drop ; immediate
( That was the definition for the comment word. )
( Now we can add comments to what we are doing! )
( Note that we are in decimal numeric input mode. )

: \ ( <line> -- , comment out rest of line )
        EOL word drop
; immediate

\ 1 echo !  \ Uncomment this line to echo Forth code while compiling.

\ *********************************************************************
\ This is another style of comment that is common in Forth.
\ pFORTH - Portable Forth System
\ Based on HMSL Forth
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
\ *********************************************************************

: COUNT  dup 1+ swap c@ ;

\ Miscellaneous support words
: ON ( addr -- , set true )
        -1 swap !
;
: OFF ( addr -- , set false )
        0 swap !
;

: CELL+ ( n -- n+cell )  cell + ;
: CELL- ( n -- n+cell )  cell - ;
: CELL* ( n -- n*cell )  cells ;

: CHAR+ ( n -- n+size_of_char ) 1+ ;
: CHARS ( n -- n*size_of_char , don't do anything)  ; immediate

\ useful stack manipulation words
: -ROT ( a b c -- c a b )
        rot rot
;
: 3DUP ( a b c -- a b c a b c )
        2 pick 2 pick 2 pick
;
: 2DROP ( a b -- )
        drop drop
;
: NIP ( a b -- b )
        swap drop
;
: TUCK ( a b -- b a b )
        swap over
;

: <= ( a b -- f , true if A <= b )
        > 0=
;
: >= ( a b -- f , true if A >= b )
        < 0=
;

: INVERT ( n -- 1'comp )
    -1 xor
;

: NOT ( n -- !n , logical negation )
        0=
;

: NEGATE ( n -- -n )
        0 swap -
;

: DNEGATE ( d -- -d , negate by doing 0-d )
        0 0 2swap d-
;


\ --------------------------------------------------------------------

: ID.   ( nfa -- )
    count 31 and type
;

: DECIMAL   10 base !  ;
: OCTAL      8 base !  ;
: HEX       16 base !  ;
: BINARY     2 base !  ;

: PAD ( -- addr )
        here 128 +
;

: $MOVE ( $src $dst -- )
        over c@ 1+ cmove
;
: BETWEEN ( n lo hi -- flag , true if between lo & hi )
        >r over r> > >r
        < r> or 0=
;
: [ ( -- , enter interpreter mode )
        0 state !
; immediate
: ] ( -- enter compile mode )
        1 state !
;

: EVEN-UP  ( n -- n | n+1 , make even )  dup 1 and +  ;
: ALIGNED  ( addr -- a-addr )
        [ cell 1- ] literal +
        [ cell 1- invert ] literal and
;
: ALIGN ( -- , align DP )  dp @ aligned dp ! ;
: ALLOT ( nbytes -- , allot space in dictionary ) dp +! ( align ) ;

: C,    ( c -- )  here c! 1 chars dp +! ;
: W,    ( w -- )  dp @ even-up dup dp !    w!  2 chars dp +! ;
: , ( n -- , lay into dictionary )  align here !  cell allot ;

\ Dictionary conversions ------------------------------------------

: N>NEXTLINK  ( nfa -- nextlink , traverses name field )
        dup c@ 31 and 1+ + aligned
;

: NAMEBASE  ( -- base-of-names )
        Headers-Base @
;
: CODEBASE  ( -- base-of-code dictionary )
        Code-Base @
;

: NAMELIMIT  ( -- limit-of-names )
        Headers-limit @
;
: CODELIMIT  ( -- limit-of-code, last address in dictionary )
        Code-limit @
;

: NAMEBASE+   ( rnfa -- nfa , convert relocatable nfa to actual )
        namebase +
;

: >CODE ( xt -- secondary_code_address, not valid for primitives )
        codebase +
;

: CODE> ( secondary_code_address -- xt , not valid for primitives )
        codebase -
;

: N>LINK  ( nfa -- lfa )
        2 CELLS -
;

: >BODY   ( xt -- pfa )
    >code body_offset +
;

: BODY>   ( pfa -- xt )
    body_offset - code>
;

\ convert between addresses useable by @, and relocatable addresses.
: USE->REL  ( useable_addr -- rel_addr )
        codebase -
;
: REL->USE  ( rel_addr -- useable_addr )
        codebase +
;

\ for JForth code
\ : >REL  ( adr -- adr )  ; immediate
\ : >ABS  ( adr -- adr )  ; immediate

: X@ ( addr -- xt , fetch execution token from relocatable )   @ ;
: X! ( addr -- xt , store execution token as relocatable )   ! ;

\ Compiler support ------------------------------------------------
: COMPILE, ( xt -- , compile call to xt )
        ,
;

( Compiler support , based on FIG )
: [COMPILE]  ( <name> -- , compile now even if immediate )
    ' compile,
;  IMMEDIATE

: (COMPILE) ( xt -- , postpone compilation of token )
        [compile] literal       ( compile a call to literal )
        ( store xt of word to be compiled )
        
        [ ' compile, ] literal   \ compile call to compile,
        compile,
;
        
: COMPILE  ( <name> -- , save xt and compile later )
    ' (compile)
; IMMEDIATE


: :NONAME ( -- xt , begin compilation of headerless secondary )
        align
        here code>   \ convert here to execution token
        ]
;

\ Error codes defined in ANSI Exception word set.
: ERR_ABORT         -1 ;   \ general abort
: ERR_EXECUTING    -14 ;   \ compile time word while not compiling
: ERR_PAIRS        -22 ;   \ mismatch in conditional
: ERR_DEFER       -258 ;  \ not a deferred word

: ABORT ( i*x -- )
	ERR_ABORT throw
;

\ Conditionals in '83 form -----------------------------------------
: CONDITIONAL_KEY ( -- , lazy constant ) 29521 ;
: ?CONDITION   ( f -- )  conditional_key - err_pairs ?error ;
: >MARK      ( -- addr )   here 0 ,  ;
: >RESOLVE   ( addr -- )   here over - swap !  ;
: <MARK      ( -- addr )   here  ;
: <RESOLVE   ( addr -- )   here - ,  ;

: ?COMP  ( -- , error if not compiling )
        state @ 0= err_executing ?error
;
: ?PAIRS ( n m -- )
        - err_pairs ?error
;
\ conditional primitives
: IF     ( -- f orig )  ?comp compile 0branch  conditional_key >mark     ; immediate
: THEN   ( f orig -- )  swap ?condition  >resolve   ; immediate
: BEGIN  ( -- f dest )  ?comp conditional_key <mark   ; immediate
: AGAIN  ( f dest -- )  compile branch  swap ?condition  <resolve  ; immediate
: UNTIL  ( f dest -- )  compile 0branch swap ?condition  <resolve  ; immediate
: AHEAD  ( -- f orig )  compile branch   conditional_key >mark     ; immediate

\ conditionals built from primitives
: ELSE   ( f orig1 -- f orig2 )
	[compile] AHEAD  2swap [compile] THEN  ; immediate
: WHILE  ( f dest -- f orig f dest )  [compile]  if   2swap ; immediate
: REPEAT ( -- f orig f dest ) [compile] again  [compile] then  ; immediate

: [']  ( <name> -- xt , define compile time tick )
        ?comp ' [compile] literal
; immediate

\ for example:
\ compile time:  compile create , (does>) then ;
\ execution time:  create <name>, ',' data, then patch pi to point to @
\    : con create , does> @ ;
\    345 con pi
\    pi
\ 
: (DOES>)  ( xt -- , modify previous definition to execute code at xt )
        latest name> >code \ get address of code for new word
        cell + \ offset to second cell in create word
        !      \ store execution token of DOES> code in new word
;

: DOES>   ( -- , define execution code for CREATE word )
        0 [compile] literal \ dummy literal to hold xt
        here cell-          \ address of zero in literal
        compile (does>)     \ call (DOES>) from new creation word
		>r                  \ move addrz to return stack so ; doesn't see stack garbage
        [compile] ;         \ terminate part of code before does>
		r>
        :noname       ( addrz xt )
        swap !              \ save execution token in literal
; immediate

: VARIABLE  ( <name> -- )
    CREATE 0 , \ IMMEDIATE
\       DOES> [compile] aliteral  \ %Q This could be optimised
;

: 2VARIABLE  ( <name> -c- ) ( -x- addr )
        create 0 , 0 ,
;

: CONSTANT  ( n <name> -c- ) ( -x- n )
        CREATE , ( n -- )
        DOES> @ ( -- n )
;



0 1- constant -1
0 2- constant -2

: 2! ( x1 x2 addr -- , store x2 followed by x1 )
        swap over ! cell+ !
;
: 2@ ( addr -- x1 x2 )
        dup cell+ @ swap @
;


: ABS ( n -- |n| )
        dup 0<
        IF negate
        THEN
;
: DABS ( d -- |d| )
        dup 0<
        IF dnegate
        THEN
;

: S>D  ( s -- d , extend signed single precision to double )
        dup 0<
        IF -1
        ELSE 0
        THEN
;

: D>S ( d -- s ) drop ;

: /MOD ( a b -- rem quo , unsigned version, FIXME )
        >r s>d r> um/mod
;

: MOD ( a b -- rem )
        /mod drop
;

: 2* ( n -- n*2 )
        1 lshift
;
: 2/ ( n -- n/2 )
        1 arshift
;

: D2*  ( d -- d*2 )
        2* over 
        cell 8 * 1- rshift or  swap
        2* swap
;

\ define some useful constants ------------------------------
1 0= constant FALSE
0 0= constant TRUE
32 constant BL


\ Store and Fetch relocatable data addresses. ---------------
: IF.USE->REL  ( use -- rel , preserve zero )
        dup IF use->rel THEN
;
: IF.REL->USE  ( rel -- use , preserve zero )
        dup IF rel->use THEN
;

: A!  ( dictionary_address addr -- )
    >r if.use->rel r> !
;
: A@  ( addr -- dictionary_address )
    @ if.rel->use
;

: A, ( dictionary_address -- )
    if.use->rel ,
;

\ Stack data structure ----------------------------------------
\ This is a general purpose stack utility used to implement necessary
\ stacks for the compiler or the user.  Not real fast.
\ These stacks grow up which is different then normal.
\   cell 0 - stack pointer, offset from pfa of word
\   cell 1 - limit for range checking
\   cell 2 - first data location

: :STACK   ( #cells -- )
        CREATE  2 cells ,          ( offset of first data location )
                dup ,              ( limit for range checking, not currently used )
                cells cell+ allot  ( allot an extra cell for safety )
;

: >STACK  ( n stack -- , push onto stack, postincrement )
        dup @ 2dup cell+ swap ! ( -- n stack offset )
        + !
;

: STACK>  ( stack -- n , pop , predecrement )
        dup @ cell- 2dup swap !
        + @
;

: STACK@ ( stack -- n , copy )
        dup @ cell- + @ 
;

: STACK.PICK ( index stack -- n , grab Nth from top of stack )
        dup @ cell- +
        swap cells -   \ offset for index
        @ 
;
: STACKP ( stack -- ptr , to next empty location on stack )
	dup @ +
;

: 0STACKP  ( stack -- , clear stack)
    8 swap !
;

32 :stack ustack
ustack 0stackp

\ Define JForth like words.
: >US ustack >stack ;
: US> ustack stack> ;
: US@ ustack stack@ ;
: 0USP ustack 0stackp ;


\ DO LOOP ------------------------------------------------

3 constant do_flag
4 constant leave_flag
5 constant ?do_flag

: DO    ( -- , loop-back do_flag jump-from ?do_flag )
        ?comp
        compile  (do)
        here >us do_flag  >us  ( for backward branch )
; immediate

: ?DO    ( -- , loop-back do_flag jump-from ?do_flag  , on user stack )
        ?comp
        ( leave address to set for forward branch )
        compile  (?do)
        here 0 ,
        here >us do_flag  >us  ( for backward branch )
        >us ( for forward branch ) ?do_flag >us
; immediate

: LEAVE  ( -- addr leave_flag )
        compile (leave)
        here 0 , >us
        leave_flag >us
; immediate

: LOOP-FORWARD  ( -us- jump-from ?do_flag -- )
        BEGIN
                us@ leave_flag =
                us@ ?do_flag =
                OR
        WHILE
                us> leave_flag =
                IF
                        us> here over - cell+ swap !
                ELSE
                        us> dup
                        here swap -
                        cell+ swap !
                THEN
        REPEAT
;

: LOOP-BACK  (  loop-addr do_flag -us- )
        us> do_flag ?pairs
        us> here -  here
        !
        cell allot
;

: LOOP    ( -- , loop-back do_flag jump-from ?do_flag )
   compile  (loop)
   loop-forward loop-back
; immediate

\ : DOTEST 5 0 do 333 . loop 888 . ;
\ : ?DOTEST0 0 0 ?do 333 . loop 888 . ;
\ : ?DOTEST1 5 0 ?do 333 . loop 888 . ;

: +LOOP    ( -- , loop-back do_flag jump-from ?do_flag )
   compile  (+loop)
   loop-forward loop-back
; immediate
        
: UNLOOP ( loop-sys -r- )
        r> \ save return pointer
        rdrop rdrop
        >r
;

: RECURSE ( ? -- ? , call the word currently being defined )
        latest  name> compile,
; immediate



: SPACE  bl emit ;
: SPACES  512 min 0 max 0 ?DO space LOOP ;
: 0SP depth 0 ?do drop loop ;

: >NEWLINE ( -- , CR if needed )
        out @ 0>
        IF cr
        THEN
;


\ Support for DEFER --------------------
: CHECK.DEFER  ( xt -- , error if not a deferred word by comparing to type )
    >code @
        ['] emit >code @
        - err_defer ?error
;

: >is ( xt -- address_of_vector )
        >code
        cell +
;

: (IS)  ( xt_do xt_deferred -- )
        >is !
;

: IS  ( xt <name> -- , act like normal IS )
        '  \ xt
        dup check.defer 
        state @
        IF [compile] literal compile (is)
        ELSE (is)
        THEN
; immediate

: (WHAT'S)  ( xt -- xt_do )
        >is @
;
: WHAT'S  ( <name> -- xt , what will deferred word call? )
        '  \ xt
        dup check.defer
        state @
        IF [compile] literal compile (what's)
        ELSE (what's)
        THEN
; immediate

: /STRING   ( addr len n -- addr' len' )
   over min  rot over   +  -rot  -
;
: PLACE   ( addr len to -- , move string )
   3dup  1+  swap cmove  c! drop
;

: PARSE-WORD   ( char -- addr len )
   >r  source tuck >in @ /string  r@ skip over swap r> scan
   >r  over -  rot r>  dup 0<> + - >in !
;
: PARSE   ( char -- addr len )
   >r  source >in @  /string  over swap  r> scan
   >r  over -  dup r> 0<>  -  >in +!
;

: LWORD  ( char -- addr )
        parse-word here place here \ 00002 , use PARSE-WORD
;

: ASCII ( <char> -- char , state smart )
        bl parse drop c@
        state @
        IF [compile] literal
        THEN
; immediate

: CHAR ( <char> -- char , interpret mode )
        bl parse drop c@
;

: [CHAR] ( <char> -- char , for compile mode )
        char [compile] literal
; immediate

: $TYPE  ( $string -- )
        count type
;

: 'word   ( -- addr )   here ;

: EVEN    ( addr -- addr' )   dup 1 and +  ;

: (C")   ( -- $addr , some Forths return addr AND count, OBSOLETE?)
        r> dup count + aligned >r
;
: (S")   ( -- c-addr cnt )
        r> count 2dup + aligned >r
;

: (.")  ( -- , type following string )
        r> count 2dup + aligned >r type
;

: ",  ( adr len -- , place string into dictionary )
         tuck 'word place 1+ allot align
;
: ,"   ( -- )
   [char] " parse ",
;

: .(  ( <string> -- , type string delimited by parentheses )
	[CHAR] ) PARSE TYPE
; IMMEDIATE

: ."   ( <string> -- , type string )
        state @
        IF      compile (.")  ,"
        ELSE [char] " parse type
        THEN
; immediate


: .'   ( <string> -- , type string delimited by single quote )
        state @
        IF    compile (.")  [char] ' parse ",
        ELSE [char] ' parse type
        THEN
; immediate

: C"    ( <string> -- addr , return string address, ANSI )
        state @
        IF compile (c")   ,"
        ELSE [char] " parse pad place pad
        THEN
; immediate

: S"    ( <string> -- , -- addr , return string address, ANSI )
        state @
        IF compile (s")   ,"
        ELSE [char] " parse pad place pad count
        THEN
; immediate

: "    ( <string> -- , -- addr , return string address )
        [compile] C"
; immediate
: P"    ( <string> -- , -- addr , return string address )
        [compile] C"
; immediate

: ""  ( <string> -- addr )
        state @
        IF 
                compile (C")
                bl parse-word  ",
        ELSE
                bl parse-word pad place pad
        THEN
; immediate

: SLITERAL ( addr cnt -- , compile string )
	compile (S")
	",
; IMMEDIATE

: $APPEND ( addr count $1 -- , append text to $1 )
    over >r
        dup >r
    count +  ( -- a2 c2 end1 )
    swap cmove
    r> dup c@  ( a1 c1 )
    r> + ( -- a1 totalcount )
    swap c!
;


\ ANSI word to replace [COMPILE] and COMPILE ----------------
: POSTPONE  ( <name> -- )
	bl word find
	dup 0=
	IF
		." Postpone could not find " count type cr abort
	ELSE
		0>
		IF compile,  \ immediate
		ELSE (compile)  \ normal
		THEN
	THEN
; immediate

\ -----------------------------------------------------------------
\ Auto Initialization
: AUTO.INIT  ( -- )
\ Kernel finds AUTO.INIT and executes it after loading dictionary.
\	." Begin AUTO.INIT ------" cr
;
: AUTO.TERM  ( -- )
\ Kernel finds AUTO.TERM and executes it on bye.
\	." End AUTO.TERM ------" cr
;

\ -------------- INCLUDE ------------------------------------------
variable TRACE-INCLUDE

: INCLUDE.MARK.START  ( $filename -- , mark start of include for FILE?)
	" ::::"  pad $MOVE
	count pad $APPEND
	pad ['] noop (:)
;

: INCLUDE.MARK.END  ( -- , mark end of include )
	" ;;;;" ['] noop (:)
;

: $INCLUDE ( $filename -- )
\ Print messages.
        trace-include @
        IF
                >newline ." Include " dup count type cr
        THEN
        here >r
        dup
        count r/o open-file 
        IF  ( -- $filename bad-fid )
                drop ." Could not find file " $type cr abort
        ELSE ( -- $filename good-fid )
                swap include.mark.start
                depth >r
                include-file    \ will also close the file
                depth 1+ r> -
                IF
                        ." Warning: stack depth changed during include!" cr
                        .s cr
                        0sp
                THEN
                include.mark.end
        THEN
        trace-include @
        IF
                ."     include added " here r@ - . ." bytes,"
                codelimit here - . ." left." cr
        THEN
        rdrop
;

create INCLUDE-SAVE-NAME 128 allot
: INCLUDE ( <fname> -- )
        BL lword
        dup include-save-name $move  \ save for RI
        $include
;

: RI ( -- , ReInclude previous file as a convenience )
        include-save-name $include
;

: INCLUDE? ( <word> <file> -- , load file if word not defined )
        bl word find
        IF drop bl word drop  ( eat word from source )
        ELSE drop include
        THEN
;

\ desired sizes for dictionary loaded after SAVE-FORTH
variable HEADERS-SIZE  
variable CODE-SIZE

: AUTO.INIT
	auto.init
	codelimit codebase - code-size !
	namelimit namebase - headers-size !
;
auto.init

: SAVE-FORTH ( $name -- )
    0                                    \ Entry point
    headers-ptr @ namebase - 65536 +     \ NameSize
    headers-size @ MAX
    here codebase - 131072 +              \ CodeSize
    code-size @ MAX
    (save-forth)
    IF
		." SAVE-FORTH failed!" cr abort
    THEN
;

: TURNKEY ( $name entry-token-- )
    0     \ NameSize = 0, names not saved in turnkey dictionary
    here codebase - 131072 +             \ CodeSize, remember that base is HEX
    (save-forth)
    IF
		." TURNKEY failed!" cr abort
    THEN
;

\ Now that we can load from files, load remainder of dictionary.

trace-include on
trace-stack on

include loadp4th.fth

decimal

: ;;;; ;  \ Mark end of this file so FILE? can find things in here.
FREEZE    \ prevent forgetting below this point

.( Dictionary compiled, save in "pforth.dic".) cr
c" pforth.dic" save-forth

\ Save the dictionary in "pfdicdat.h" file so pForth can be compiled for standalone mode.
SDAD

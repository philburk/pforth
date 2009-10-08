\ @(#) dump_struct.fth 97/12/10 1.1
\ Dump contents of structure showing values and member names.
\
\ Author: Phil Burk
\ Copyright 1987 Phil Burk
\ All Rights Reserved.
\
\ MOD: PLB 9/4/88 Print size too.
\ MOD: PLB 9/9/88 Print U/S , add ADST
\ MOD: PLB 12/6/90 Modified to work with H4th
\ 941109 PLB Converted to pforth.  Added RP detection.
\ 090609 PLB Convert >rel to use->rel and ..! to s!

include? task-member.fth member.fth
include? task-c_struct c_struct.fth

ANEW TASK-DUMP_STRUCT

: EMIT-TO-COLUMN ( char col -- )
	out @ - 0 max 80 min 0
	DO  dup emit
	LOOP drop
;

VARIABLE SN-FENCE
: STACK.NFAS  ( fencenfa topnfa -- 0 nfa0 nfa1 ... )
\ Fill stack with nfas of words until fence hit.
    >r sn-fence !
    0 r>  ( set terminator )
    BEGIN ( -- 0 n0 n1 ... top )
      dup sn-fence @ >
    WHILE
\      dup n>link @   \ JForth
       dup prevname   \ HForth
    REPEAT
    drop
;

: DST.DUMP.TYPE  ( +-size -- , dump data type, 941109)
	dup abs 4 =
	IF
		0<
		IF ." RP"
		ELSE ." U4"
		THEN
	ELSE
		dup 0<
        	IF ascii U
        	ELSE ascii S
        	THEN emit abs 1 .r
	THEN
;

: DUMP.MEMBER ( addr member-pfa -- , dump member of structure)
    ob.stats  ( -- addr offset size )
    >r + r> ( -- addr' size )
    dup ABS 4 >  ( -- addr' size flag )
    IF   cr 2dup swap . . ABS dump
    ELSE tuck @bytes 10 .r ( -- size )
        3 spaces dst.dump.type
    THEN
;

VARIABLE DS-ADDR
: DUMP.STRUCT ( addr-data addr-structure -- )
    >newline swap >r  ( -- as , save addr-data for dumping )
\    dup cell+ @ over +  \ JForth
	dup code> >name swap cell+ @ over +   \ HForth
	stack.nfas   ( fill stack with nfas of members )
    BEGIN
        dup
    WHILE   ( continue until non-zero )
        dup name> >body r@ swap dump.member
        bl 18 emit-to-column id. cr
        ?pause
    REPEAT drop rdrop
;

: DST ( addr <name> -- , dump contents of structure )
    ob.findit
    state @
    IF [compile] literal compile dump.struct
    ELSE dump.struct
    THEN
; immediate

: ADST ( absolute_address -- , dump structure )
    use->rel [compile] dst     \ mod 090609
; immediate

\ For Testing Purposes
false [IF]
:STRUCT GOO
    LONG DATAPTR
    SHORT GOO_WIDTH
    USHORT GOO_HEIGHT
;STRUCT

:STRUCT FOO
    LONG ALONG1
    STRUCT GOO AGOO
    SHORT ASHORT1
    BYTE ABYTE
    BYTE ABYTE2
;STRUCT

FOO AFOO
: AFOO.INIT
    $ 12345678 afoo s! along1
    $ -665 afoo s! ashort1
    $ 21 afoo s! abyte
    $ 43 afoo s! abyte2
    -234 afoo .. agoo s! goo_height
;
afoo.init

: TDS ( afoo -- )
    dst foo
;

[THEN]


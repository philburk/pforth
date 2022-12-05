\ @(#) t_corex.fth 98/03/16 1.2
\ Test ANS Forth Core Extensions
\
\ Copyright 1994 3DO, Phil Burk

INCLUDE? }T{  t_tools.fth

ANEW TASK-T_COREX.FTH

DECIMAL

TEST{

\ ==========================================================
T{ 1 2 3 }T{ 1 2 3 }T

\  ----------------------------------------------------- .(
T{ 27 .( IF YOU SEE THIS THEN .( WORKED!) }T{ 27 }T

CR .(     1234 - SHOULD LINE UP WITH NEXT LINE.) CR 1234 8 .R CR

T{ .( )   987   .( TEST NULL STRING IN .( ) CR }T{ 987 }T

\  ----------------------------------------------------- 0<>
T{ 5 0<> }T{ TRUE }T
T{ 0 0<> }T{ 0 }T
T{ -1000 0<> }T{ TRUE }T

\  ----------------------------------------------------- 2>R 2R> 2R@
: T2>R  ( -- .... )
    17
    20 5 2>R
    19
    2R@
    37
    2R>
\ 2>R should be the equivalent of SWAP >R >R so this next construct
\ should reduce to a SWAP.
    88 77 2>R R> R>
;
T{ T2>R }T{ 17 19 20 5 37 20 5 77 88 }T

\  ----------------------------------------------------- :NONAME
T{ :NONAME  100 50 + ; EXECUTE }T{ 150 }T

\  ----------------------------------------------------- <>
T{ 12345 12305 <> }T{ TRUE }T
T{ HEX 98765432 98765432 DECIMAL <> }T{ 0 }T

\  ----------------------------------------------------- ?DO
: T?DO  ( n -- sum_n ) 0 SWAP 1+ 0 ?DO i + LOOP ;
T{ 0 T?DO }T{ 0 }T
T{ 4 T?DO }T{ 10 }T

\  ----------------------------------------------------- AGAIN
: T.AGAIN  ( n --  )
    BEGIN
        DUP .
        DUP 6 < IF EXIT THEN
        1-
    AGAIN
;
T{ 10 T.AGAIN CR }T{ 5 }T

\  ----------------------------------------------------- C"
: T.C"  ( -- $STRING )
    C" x5&"
;
T{ T.C"  C@  }T{ 3 }T
T{ T.C"  COUNT DROP C@  }T{ CHAR x }T
T{ T.C"  COUNT DROP CHAR+ C@ }T{  CHAR 5 }T
T{ T.C"  COUNT DROP 2 CHARS + C@  }T{ CHAR & }T

\  ----------------------------------------------------- CASE
: T.CASE  ( N -- )
    CASE
        1 OF 101 ENDOF
        27 OF 892 ENDOF
        941 SWAP \ default
    ENDCASE
;
T{ 1 T.CASE }T{ 101 }T
T{ 27 T.CASE }T{ 892 }T
T{ 49 T.CASE }T{ 941 }T

\  ----------------------------------------------------- COMPILE,
: COMPILE.SWAP    ['] SWAP COMPILE, ; IMMEDIATE
: T.COMPILE,
    19 20 27 COMPILE.SWAP 39
;
T{ T.COMPILE, }T{ 19 27 20 39 }T

\  ----------------------------------------------------- CONVERT
: T.CONVERT
    0 S>D  S" 1234xyz" DROP CONVERT
    >R
    D>S
    R> C@
;
T{ T.CONVERT }T{ 1234 CHAR x }T

\  ----------------------------------------------------- ERASE
: T.COMMA.SEQ  ( n -- , lay down N sequential bytes )
    0 ?DO I C, LOOP
;
CREATE T-ERASE-DATA   64 T.COMMA.SEQ
T{ T-ERASE-DATA 8 + C@ }T{ 8 }T
T{ T-ERASE-DATA 7 + 3 ERASE
T{ T-ERASE-DATA 6 + C@ }T{ 6 }T
T{ T-ERASE-DATA 7 + C@ }T{ 0 }T
T{ T-ERASE-DATA 8 + C@ }T{ 0 }T
T{ T-ERASE-DATA 9 + C@ }T{ 0 }T
T{ T-ERASE-DATA 10 + C@ }T{ 10 }T

\  ----------------------------------------------------- FALSE
T{ FALSE }T{ 0 }T

\  ----------------------------------------------------- HEX
T{ HEX 10 DECIMAL }T{ 16 }T

\  ----------------------------------------------------- MARKER
: INDIC?  ( <name> -- ifInDic , is the following word defined? )
    bl word find
    swap drop 0= 0=
;
create FOOBAR
MARKER MYMARK  \ create word that forgets itself
create GOOFBALL
MYMARK
T{ indic? foobar  indic? mymark indic? goofball }T{ true false false }T

\  ----------------------------------------------------- NIP
T{ 33 44 55 NIP  }T{ 33 55 }T

\  ----------------------------------------------------- PARSE
: T.PARSE  ( char <string>char -- addr num )
    PARSE
    >R  \ save length
    PAD R@ CMOVE  \ move string to pad
    PAD R>
;
T{ CHAR % T.PARSE wxyz% SWAP C@ }T{  4  CHAR w }T

\  ----------------------------------------------------- PICK
T{ 13 12 11 10 2 PICK  }T{ 13 12 11 10 12 }T

\  ----------------------------------------------------- QUERY
T{ ' QUERY 0<> }T{ TRUE }T

\  ----------------------------------------------------- REFILL
T{ ' REFILL 0<> }T{ TRUE }T

\  ----------------------------------------------------- RESTORE-INPUT
T{ : T.SAVE-INPUT SAVE-INPUT RESTORE-INPUT ; T.SAVE-INPUT }T{ 0 }T

\ TESTING SAVE-INPUT and RESTORE-INPUT with a string source

VARIABLE SI_INC 0 SI_INC !

: SI1
   SI_INC @ >IN +!
   15 SI_INC !
;

: S$ S" SAVE-INPUT SI1 RESTORE-INPUT 12345" ;

T{ S$ EVALUATE SI_INC @ }T{ 0 2345 15 }T

\  ----------------------------------------------------- ROLL
T{ 15 14 13 12 11 10 0 ROLL  }T{  15 14 13 12 11 10 }T
T{ 15 14 13 12 11 10 1 ROLL  }T{  15 14 13 12 10 11 }T
T{ 15 14 13 12 11 10 2 ROLL  }T{  15 14 13 11 10 12 }T
T{ 15 14 13 12 11 10 3 ROLL  }T{  15 14 12 11 10 13 }T
T{ 15 14 13 12 11 10 4 ROLL  }T{  15 13 12 11 10 14 }T

\  ----------------------------------------------------- SOURCE-ID
T{ SOURCE-ID 0<> }T{ TRUE }T
T{ : T.SOURCE-ID  S" SOURCE-ID" EVALUATE  ;   T.SOURCE-ID }T{ -1 }T

\  ----------------------------------------------------- SPAN
T{ ' SPAN 0<>  }T{ TRUE }T

\  ----------------------------------------------------- TO VALUE
333 VALUE  MY-VALUE
T{ MY-VALUE }T{ 333 }T
T{ 1000 TO MY-VALUE   MY-VALUE }T{ 1000 }T
: TEST.VALUE  ( -- 19 100 )
    100 TO MY-VALUE
    19
    MY-VALUE
;
T{ TEST.VALUE }T{ 19 100 }T

\  ----------------------------------------------------- TRUE
T{ TRUE }T{ 0 0= }T

\  ----------------------------------------------------- TUCK
T{ 44 55 66 TUCK }T{ 44 66 55 66 }T

\  ----------------------------------------------------- U.R
HEX CR .(     ABCD4321 - SHOULD LINE UP WITH NEXT LINE.) CR
ABCD4321 C U.R CR DECIMAL

\  ----------------------------------------------------- U>
T{ -5 3 U> }T{ TRUE }T
T{ 10 8 U> }T{ TRUE }T

\  ----------------------------------------------------- UNUSED
T{ UNUSED 0> }T{ TRUE }T

\  ----------------------------------------------------- WITHIN
T{  4  5 10 WITHIN }T{ 0 }T
T{  5  5 10 WITHIN }T{ TRUE }T
T{  9  5 10 WITHIN }T{ TRUE }T
T{ 10  5 10 WITHIN }T{ 0 }T

T{  4  10 5 WITHIN }T{ TRUE }T
T{  5  10 5 WITHIN }T{ 0 }T
T{  9  10 5 WITHIN }T{ 0 }T
T{ 10  10 5 WITHIN }T{ TRUE }T

T{  -6  -5 10 WITHIN }T{ 0 }T
T{  -5  -5 10 WITHIN    }T{ TRUE }T
T{  9  -5 10 WITHIN    }T{ TRUE }T
T{ 10  -5 10 WITHIN }T{ 0 }T


\  ----------------------------------------------------- [COMPILE]
: T.[COMPILE].IF  [COMPILE] IF ; IMMEDIATE
: T.[COMPILE]  40 0> T.[COMPILE].IF 97 ELSE 53 THEN 97 = ;
T{ T.[COMPILE] }T{ TRUE }T

\  ----------------------------------------------------- \

\ .( TESTING DO +LOOP with large and small increments )

\ Contributed by Andrew Haley
0 invert CONSTANT MAX-UINT
0 INVERT 1 RSHIFT CONSTANT MAX-INT
0 INVERT 1 RSHIFT INVERT    CONSTANT MIN-INT
MAX-UINT 8 RSHIFT 1+ CONSTANT USTEP
USTEP NEGATE CONSTANT -USTEP
MAX-INT 7 RSHIFT 1+ CONSTANT STEP
STEP NEGATE CONSTANT -STEP

VARIABLE BUMP

T{ : GD8 BUMP ! DO 1+ BUMP @ +LOOP ; }T{ }T

T{ 0 MAX-UINT 0 USTEP GD8 }T{ 256 }T
T{ 0 0 MAX-UINT -USTEP GD8 }T{ 256 }T

T{ 0 MAX-INT MIN-INT STEP GD8 }T{ 256 }T
T{ 0 MIN-INT MAX-INT -STEP GD8 }T{ 256 }T

\ Two's complement arithmetic, wraps around modulo wordsize
\ Only tested if the Forth system does wrap around, use of conditional
\ compilation deliberately avoided

MAX-INT 1+ MIN-INT = CONSTANT +WRAP?
MIN-INT 1- MAX-INT = CONSTANT -WRAP?
MAX-UINT 1+ 0=       CONSTANT +UWRAP?
0 1- MAX-UINT =      CONSTANT -UWRAP?

: GD9  ( n limit start step f result -- )
   >R IF GD8 ELSE 2DROP 2DROP R@ THEN }T{ R> }T
;

T{ 0 0 0  USTEP +UWRAP? 256 GD9
T{ 0 0 0 -USTEP -UWRAP?   1 GD9
T{ 0 MIN-INT MAX-INT  STEP +WRAP? 1 GD9
T{ 0 MAX-INT MIN-INT -STEP -WRAP? 1 GD9

\ --------------------------------------------------------------------------
\ .( TESTING DO +LOOP with maximum and minimum increments )

: (-MI) MAX-INT DUP NEGATE + 0= IF MAX-INT NEGATE ELSE -32767 THEN ;
(-MI) CONSTANT -MAX-INT

T{ 0 1 0 MAX-INT GD8  }T{ 1 }T
T{ 0 -MAX-INT NEGATE -MAX-INT OVER GD8  }T{ 2 }T

T{ 0 MAX-INT  0 MAX-INT GD8  }T{ 1 }T
T{ 0 MAX-INT  1 MAX-INT GD8  }T{ 1 }T
T{ 0 MAX-INT -1 MAX-INT GD8  }T{ 2 }T
T{ 0 MAX-INT DUP 1- MAX-INT GD8  }T{ 1 }T

T{ 0 MIN-INT 1+   0 MIN-INT GD8  }T{ 1 }T
T{ 0 MIN-INT 1+  -1 MIN-INT GD8  }T{ 1 }T
T{ 0 MIN-INT 1+   1 MIN-INT GD8  }T{ 2 }T
T{ 0 MIN-INT 1+ DUP MIN-INT GD8  }T{ 1 }T

\ ----------------------------------------------------------------------------
\ .( TESTING number prefixes # $ % and 'c' character input )
\ Adapted from the Forth 200X Draft 14.5 document

VARIABLE OLD-BASE
DECIMAL BASE @ OLD-BASE !
T{ #1289 }T{ 1289 }T
T{ #-1289 }T{ -1289 }T
T{ $12eF }T{ 4847 }T
T{ $-12eF }T{ -4847 }T
T{ %10010110 }T{ 150 }T
T{ %-10010110 }T{ -150 }T
T{ 'z' }T{ 122 }T
T{ 'Z' }T{ 90 }T
\ Check BASE is unchanged
T{ BASE @ OLD-BASE @ = }T{ TRUE }T

\ Repeat in Hex mode
16 OLD-BASE ! 16 BASE !
T{ #1289 }T{ 509 }T
T{ #-1289 }T{ -509 }T
T{ $12eF }T{ 12EF }T
T{ $-12eF }T{ -12EF }T
T{ %10010110 }T{ 96 }T
T{ %-10010110 }T{ -96 }T
T{ 'z' }T{ 7a }T
T{ 'Z' }T{ 5a }T
\ Check BASE is unchanged
T{ BASE @ OLD-BASE @ = }T{ TRUE }T   \ 2

DECIMAL
\ Check number prefixes in compile mode
T{ : nmp  #8327 $-2cbe %011010111 ''' ; nmp }T{ 8327 -11454 215 39 }T

\  ----------------------------------------------------- ENVIRONMENT?

T{ s" unknown-query-string" ENVIRONMENT? }T{ FALSE }T
T{ s" MAX-CHAR" ENVIRONMENT? }T{ 255 TRUE }T
T{ s" ADDRESS-UNITS-BITS" ENVIRONMENT? }T{ 8 TRUE }T

\  ----------------------------------------------------- PROGRAMMING

T{ exists? words }T{ true }T  \ high level
T{ exists? swap }T{ true }T   \ in kernel
T{ exists? lkajsdlakjs }T{ false }T

T{ [defined] if }T{ true }T   \ high level
T{ [defined] dup }T{ true }T  \ in kernel
T{ [defined] k23jh42 }T{ false }T

T{ [undefined] if }T{ false }T  \ high level
T{ [undefined] dup }T{ false }T \ in kernel
T{ [undefined] k23jh42 }T{ true }T

\  ----------------------------------------------------- Structures

BEGIN-STRUCTURE XYZS
    cfield: xyz.c1
    field:  xyz.w1
    cfield: xyz.c2
END-STRUCTURE

T{ xyzs }T{ 2 cells 1+ }T
T{ 0 xyz.c1 }T{ 0 }T
T{ 0 xyz.w1 }T{ cell }T
T{ 0 xyz.c2 }T{ 2 cells }T

CREATE MY-XYZS XYZS ALLOT
\ test forward order
77 my-xyzs xyz.c1 c!
1234567 my-xyzs xyz.w1 !
99 my-xyzs xyz.c2 c!

T{  my-xyzs xyz.c1 c@ }T{ 77 }T
T{  my-xyzs xyz.w1 @ }T{ 1234567 }T
T{  my-xyzs xyz.c2 c@ }T{ 99 }T


}TEST


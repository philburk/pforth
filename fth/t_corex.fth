\ @(#) t_corex.fth 98/03/16 1.2
\ Test ANS Forth Core Extensions
\
\ Copyright 1994 3DO, Phil Burk

INCLUDE? }T{  t_tools.fth

ANEW TASK-T_COREX.FTH

DECIMAL

\ STUB because missing definition in pForth - FIXME
: SAVE-INPUT ;
: RESTORE-INPUT -1 ;

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
T{ : T.SAVE-INPUT SAVE-INPUT RESTORE-INPUT ; T.SAVE-INPUT }T{ 0 }T  \ EXPECTED FAILURE

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
}TEST


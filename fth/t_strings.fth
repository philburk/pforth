\ @(#) t_strings.fth 97/12/10 1.1
\ Test ANS Forth String Word Set
\
\ Copyright 1994 3DO, Phil Burk

include? }T{  t_tools.fth

marker task-t_string.fth

decimal

test{

echo off

\ ==========================================================
\ test is.ok?
T{ 1 2 3 }T{ 1 2 3 }T

: STR1  S" Hello    " ;
: STR2  S" Hello World" ;
: STR3  S" " ;

\  ----------------------------------------------------- -TRAILING
T{ STR1 -TRAILING }T{ STR1 DROP 5 }T
T{ STR2 -TRAILING }T{ STR2 }T
T{ STR3 -TRAILING }T{ STR3 }T

\  ----------------------------------------------------- /STRING
T{ STR2  6  /STRING  }T{ STR2 DROP 6 CHARS +   STR2 NIP 6 -  }T


\  ----------------------------------------------------- BLANK
: T.COMMA.SEQ  ( n -- , lay down N sequential bytes )
	0 ?DO I C, LOOP
;
CREATE T-BLANK-DATA   64 T.COMMA.SEQ
T{ T-BLANK-DATA 8 + C@ }T{ 8 }T
T-BLANK-DATA 7 + 3 BLANK
T{ T-BLANK-DATA 6 + C@ }T{ 6 }T
T{ T-BLANK-DATA 7 + C@ }T{ BL }T
T{ T-BLANK-DATA 8 + C@ }T{ BL }T
T{ T-BLANK-DATA 9 + C@ }T{ BL }T
T{ T-BLANK-DATA 10 + C@ }T{ 10 }T
FORGET T.COMMA.SEQ

\  ----------------------------------------------------- CMOVE
: T.COMMA.SEQ  ( n -- , lay down N sequential bytes )
	0 ?DO I C, LOOP
;
CREATE T-BLANK-DATA   64 T.COMMA.SEQ
T-BLANK-DATA 7 + T-BLANK-DATA 6 + 3 CMOVE
T{ T-BLANK-DATA 5 + C@ }T{ 5 }T
T{ T-BLANK-DATA 6 + C@ }T{ 7 }T
T{ T-BLANK-DATA 7 + C@ }T{ 8 }T
T{ T-BLANK-DATA 8 + C@ }T{ 9 }T
T{ T-BLANK-DATA 9 + C@ }T{ 9 }T
FORGET T.COMMA.SEQ

\  ----------------------------------------------------- CMOVE>
: T.COMMA.SEQ  ( n -- , lay down N sequential bytes )
	0 ?DO I C, LOOP
;
CREATE T-BLANK-DATA   64 T.COMMA.SEQ
T{ T-BLANK-DATA 6 + T-BLANK-DATA 7 + 3 CMOVE>
T{ T-BLANK-DATA 5 + C@ }T{ 5 }T
T{ T-BLANK-DATA 6 + C@ }T{ 6 }T
T{ T-BLANK-DATA 7 + C@ }T{ 6 }T
T{ T-BLANK-DATA 8 + C@ }T{ 7 }T
T{ T-BLANK-DATA 9 + C@ }T{ 8 }T
T{ T-BLANK-DATA 10 + C@ }T{ 10 }T
FORGET T.COMMA.SEQ

\  ----------------------------------------------------- COMPARE
T{ : T.COMPARE.1 S" abcd" S" abcd"    compare ; t.compare.1 }T{   0 }T
T{ : T.COMPARE.2 S" abcd" S" abcde"   compare ; t.compare.2 }T{  -1 }T
T{ : T.COMPARE.3 S" abcdef" S" abcde" compare ; t.compare.3 }T{   1 }T
T{ : T.COMPARE.4 S" abGd" S" abcde"   compare ; t.compare.4 }T{  -1 }T
T{ : T.COMPARE.5 S" abcd" S" aXcde"   compare ; t.compare.5 }T{   1 }T
T{ : T.COMPARE.6 S" abGd" S" abcd"    compare ; t.compare.6 }T{  -1 }T
T{ : T.COMPARE.7 S" World" S" World"  compare ; t.compare.7 }T{   0 }T
FORGET T.COMPARE.1

\  ----------------------------------------------------- SEARCH
: STR-SEARCH S" ABCDefghIJKL" ;
T{ : T.SEARCH.1 STR-SEARCH S" ABCD" SEARCH ; T.SEARCH.1 }T{ STR-SEARCH TRUE }T
T{ : T.SEARCH.2 STR-SEARCH S" efg"  SEARCH ; T.SEARCH.2 }T{
	 STR-SEARCH 4 - SWAP 4 CHARS + SWAP TRUE }T
T{ : T.SEARCH.3 STR-SEARCH S" IJKL" SEARCH ; T.SEARCH.3 }T{
	 STR-SEARCH DROP 8 CHARS + 4 TRUE }T
T{ : T.SEARCH.4 STR-SEARCH STR-SEARCH SEARCH ; T.SEARCH.4 }T{
	 STR-SEARCH  TRUE }T

T{ : T.SEARCH.5 STR-SEARCH S" CDex" SEARCH ; T.SEARCH.5 }T{
	 STR-SEARCH  FALSE }T
T{ : T.SEARCH.6 STR-SEARCH S" KLM" SEARCH ; T.SEARCH.6 }T{
	 STR-SEARCH  FALSE }T
FORGET STR-SEARCH

\  ----------------------------------------------------- SLITERAL
CREATE FAKE-STRING  CHAR H C,   CHAR e C,  CHAR l C, CHAR l C, CHAR o C, 
ALIGN
T{ : T.SLITERAL.1  [ FAKE-STRING 5 ] SLITERAL ; T.SLITERAL.1   FAKE-STRING 5 COMPARE
	 }T{ 0 }T
	
}test

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

\  ----------------------------------------------------- S\"
HEX
T{ : GC5 S\" \a\b\e\f\l\m\q\r\t\v\x0F0\x1Fa\xaBx\z\"\\" ; }T{ }T
T{ GC5 SWAP DROP          }T{ 14 }T \ String length
T{ GC5 DROP            C@ }T{ 07 }T \ \a   BEL  Bell
T{ GC5 DROP  1 CHARS + C@ }T{ 08 }T \ \b   BS   Backspace
T{ GC5 DROP  2 CHARS + C@ }T{ 1B }T \ \e   ESC  Escape
T{ GC5 DROP  3 CHARS + C@ }T{ 0C }T \ \f   FF   Form feed
T{ GC5 DROP  4 CHARS + C@ }T{ 0A }T \ \l   LF   Line feed
T{ GC5 DROP  5 CHARS + C@ }T{ 0D }T \ \m        CR of CR/LF pair
T{ GC5 DROP  6 CHARS + C@ }T{ 0A }T \           LF of CR/LF pair
T{ GC5 DROP  7 CHARS + C@ }T{ 22 }T \ \q   "    Double Quote
T{ GC5 DROP  8 CHARS + C@ }T{ 0D }T \ \r   CR   Carriage Return
T{ GC5 DROP  9 CHARS + C@ }T{ 09 }T \ \t   TAB  Horizontal Tab
T{ GC5 DROP  A CHARS + C@ }T{ 0B }T \ \v   VT   Vertical Tab
T{ GC5 DROP  B CHARS + C@ }T{ 0F }T \ \x0F      Given Char
T{ GC5 DROP  C CHARS + C@ }T{ 30 }T \ 0    0    Digit follow on
T{ GC5 DROP  D CHARS + C@ }T{ 1F }T \ \x1F      Given Char
T{ GC5 DROP  E CHARS + C@ }T{ 61 }T \ a    a    Hex follow on
T{ GC5 DROP  F CHARS + C@ }T{ AB }T \ \xaB      Insensitive Given Char
T{ GC5 DROP 10 CHARS + C@ }T{ 78 }T \ x    x    Non hex follow on
T{ GC5 DROP 11 CHARS + C@ }T{ 00 }T \ \z   NUL  No Character
T{ GC5 DROP 12 CHARS + C@ }T{ 22 }T \ \"   "    Double Quote
T{ GC5 DROP 13 CHARS + C@ }T{ 5C }T \ \\   \    Back Slash
DECIMAL

}test

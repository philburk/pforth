\ @(#) t_wordlist.fth 15/05/20 0.2
\ Search-Order wordset
\
\
\
\ Author: Hannu Vuolasaho
\
\ Copied and modified from
\ http://www.forth200x.org/documents/html3/testsuite.html#section.F.19
exists? (init-wordlists)
[if]
include? }T{  t_tools.fth

test{
VARIABLE wid1
VARIABLE wid2
wordlist wid1 !
wordlist wid2 !

: save-orderlist ( widn ... wid1 n -- )
   DUP , 0 ?DO , LOOP
;

CREATE order-list
T{ GET-ORDER save-orderlist }T{ }T

: get-orderlist ( -- widn ... wid1 n )
   order-list DUP @ CELLS	   ( -- ad n )
   OVER +	                      ( -- AD AD' )
   ?DO I @ -1 CELLS +LOOP    ( -- )
;
\ F.16.6.1.1595
\ FORTH-WORDLIST
T{ FORTH-WORDLIST wid1 ! }T{ }T
\ F.16.6.1.1180
\ DEFINITIONS
T{ ONLY FORTH DEFINITIONS }T{ }T
T{ GET-CURRENT }T{ FORTH-WORDLIST }T

T{ GET-ORDER wid2 @ SWAP 1+ SET-ORDER DEFINITIONS GET-CURRENT
}T{ wid2 @ }T

T{ GET-ORDER }T{ get-orderlist wid2 @ SWAP 1+ }T

T{ PREVIOUS GET-ORDER }T{ get-orderlist }T

T{ DEFINITIONS GET-CURRENT }T{ FORTH-WORDLIST }T

: alsowid2 ALSO GET-ORDER wid2 @ ROT DROP SWAP SET-ORDER ;
alsowid2
: w1 1234 ;
DEFINITIONS : w1 -9876 ; IMMEDIATE

ONLY FORTH
T{ w1 }T{ 1234 }T
DEFINITIONS
T{ w1 }T{ 1234 }T
alsowid2
T{ w1 }T{ -9876 }T
DEFINITIONS T{ w1 }T{ -9876 }T

ONLY FORTH DEFINITIONS
: so5 DUP IF SWAP EXECUTE THEN ;

 T{ S" w1" wid1 @ SEARCH-WORDLIST so5 }T{ -1  1234 }T
 T{ S" w1" wid2 @ SEARCH-WORDLIST so5 }T{  1 -9876 }T

: c"w1" C" w1" ;
T{ alsowid2 c"w1" FIND so5 }T{  1 -9876 }T
T{ PREVIOUS c"w1" FIND so5 }T{ -1  1234 }T
\ F.16.6.1.1550
\ FIND

VARIABLE xt ' DUP xt !
VARIABLE xti ' .( xti ! \ Immediate word

: c"dup" C" DUP" ;
: c".(" C" .(" ;
: c"x" C" unknown word" ;

T{ c"dup" FIND }T{ xt  @ -1 }T
T{ c".("  FIND }T{ xti @  1 }T
T{ c"x"   FIND }T{ c"x"   0 }T

\ F.16.6.1.2192
\ SEARCH-WORDLIST
ONLY FORTH DEFINITIONS

T{ S" DUP" wid1 @ SEARCH-WORDLIST }T{ xt  @ -1 }T
T{ S" .("  wid1 @ SEARCH-WORDLIST }T{ xti @  1 }T
T{ S" DUP" wid2 @ SEARCH-WORDLIST }T{        0 }T
\ F.16.6.1.2195
\ SET-CURRENT
T{ GET-CURRENT }T{ wid1 @ }T

T{ WORDLIST wid2 ! }T{ }T
T{ wid2 @ SET-CURRENT }T{ }T
T{ GET-CURRENT }T{ wid2 @ }T

T{ wid1 @ SET-CURRENT }T{ }T
\ F.16.6.1.2197
\ SET-ORDER
T{ GET-ORDER OVER      }T{ GET-ORDER wid1 @ }T
T{ GET-ORDER SET-ORDER }T{ }T
T{ GET-ORDER           }T{ get-orderlist }T T{ get-orderlist DROP get-orderList 2* SET-ORDER }T{ }T
T{ GET-ORDER }T{ get-orderlist DROP get-orderList 2* }T
T{ get-orderlist SET-ORDER GET-ORDER }T{ get-orderlist }T

: so2a GET-ORDER get-orderlist SET-ORDER ;
: so2 0 SET-ORDER so2a ;

T{ so2 }T{ 0 }T	    \ 0 SET-ORDER leaves an empty search order

: so3 -1 SET-ORDER so2a ;
: so4 ONLY so2a ;

T{ so3 }T{ so4 }T	   \ -1 SET-ORDER is the same as ONLY
\ F.16.6.2.0715
\ ALSO
T{ ALSO GET-ORDER ONLY }T{ get-orderlist OVER SWAP 1+ }T
\ F.16.6.2.1965
\ ONLY
T{ ONLY FORTH GET-ORDER }T{ get-orderlist }T

: so1 SET-ORDER ; \ In case it is unavailable in the forth wordlist

T{ ONLY FORTH-WORDLIST 1 SET-ORDER get-orderlist so1 }T{ }T
T{ GET-ORDER }T{ get-orderlist }T
\ F.16.6.2.1985
\ ORDER
CR .( ONLY FORTH DEFINITIONS search order and compilation list) CR
T{ ONLY FORTH DEFINITIONS ORDER }T{ }T

CR .( Plus another unnamed wordlist at head of search order) CR
T{ alsowid2 DEFINITIONS ORDER }T{ }T
}test
[else]
." This compilation doesn't support word lists or SEARCH-ORDER word set" cr
[then]
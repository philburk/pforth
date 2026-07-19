\ ANS Forth Double-Number Word Tests
\ Tests for DOUBLE word set: 2CONSTANT 2VARIABLE 2! 2@ D+ D- DNEGATE
\ Follows the style of fth/coretest.fth (JHU/APL test framework)

include? testing fth/tester.fth

TESTING DOUBLE NUMBER WORDS

DECIMAL

\ 2VARIABLE and 2!  2@
2VARIABLE DVAR
{ 5 6 DVAR 2! -> }
{ DVAR 2@ -> 5 6 }

\ 2CONSTANT
7 8 2CONSTANT DCON
{ DCON -> 7 8 }

\ D+ (double-precision addition)
{ 1 0 2 0 D+ -> 3 0 }
{ -1 -1 1 0 D+ -> 0 0 }
{ 0 1 0 1 D+ -> 0 2 }         \ test carry propagation

\ D- (double-precision subtraction)
{ 5 0 3 0 D- -> 2 0 }
{ 3 0 5 0 D- -> -2 -1 }       \ negative result (2s complement)

\ DNEGATE
{ 1 0 DNEGATE -> -1 -1 }
{ -1 -1 DNEGATE -> 1 0 }
{ 0 0 DNEGATE -> 0 0 }

\ D0= (double equal zero)
{ 0 0 D0= -> TRUE }
{ 1 0 D0= -> FALSE }
{ 0 1 D0= -> FALSE }

\ D= (double equality)
{ 1 2 1 2 D= -> TRUE }
{ 1 2 1 3 D= -> FALSE }

\ D< (double less-than, signed)
{ 0 0 1 0 D< -> TRUE }
{ 1 0 0 0 D< -> FALSE }
{ -1 -1 0 0 D< -> TRUE }

CR .( Double-number tests complete.) CR

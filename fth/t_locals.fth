\ @(#) t_locals.fth 97/01/28 1.1
\ Test PForth LOCAL variables.
\
\ Copyright 1996 3DO, Phil Burk

include? }T{  t_tools.fth

anew task-t_locals.fth
decimal

test{

\ test value and locals
T{ 333 value  my-value   my-value }T{  333 }T
T{ 1000 -> my-value   my-value }T{ 1000 }T
T{ 35 +-> my-value   my-value }T{ 1035 }T
: test.value  ( -- ok )
	100 -> my-value
	my-value 100 =
	47 +-> my-value
	my-value 147 = AND
;
T{ test.value }T{ TRUE }T

\ test locals in a word
: test.locs  { aa bb | cc -- ok }
	cc 0=
	aa bb + -> cc
	aa bb +   cc = AND
	aa -> cc
	bb +->  cc
	aa bb +   cc = AND
;

T{ 200 59 test.locs }T{  TRUE }T

.( Test warning when no locals defined.) cr
: loc.nonames { -- } 1234 ;
T{ loc.nonames }T{ 1234 }T

\ try to put EOLs and comments in variable list
: calc.area {
	width    \ horizontal dimension
	height   \ vertical dimension
	-- area , calculate area of a rectangle }
	width height *
;

T{ 5 20 calc.area }T{ 100 }T

}test


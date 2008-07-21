\ Test INCLUDE errors.
\
\ Copyright 2001Phil Burk

include? }T{  t_tools.fth

marker task-t_string.fth

decimal

: F_UNDEF " t_load_undef.fth" ;

test{

T{ F_UNDEF ' $include catch }T{ F_UNDEF -13 }T

	
}test

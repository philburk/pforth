\ Test INCLUDE errors.
\
\ Copyright 2001 Phil Burk

include? }T{  t_tools.fth

marker task-t_string.fth

decimal

test{

\ Test long line bug in #174
\ This should be tested with:
\   make clean && make ASAN=1 test
T{
\ The following numbers need to be on one line and over 130 characters.
123456701 123456702 123456703 123456704 123456705 123456706 123456707 123456708 123456709 123456710 123456711 123456712 123456713 123456714 123456715 123456716
}T{
    123456701 123456702 123456703 123456704
    123456705 123456706 123456707 123456708
    123456709 123456710 123456711 123456712
    123456713 123456714 123456715 123456716
}T

." Intentional error! Test whether INCLUDE can catch an unrecognized word error." cr
: F_UNDEF " t_load_undef.fth" ;
T{ F_UNDEF ' $include catch }T{ F_UNDEF -13 }T

}test

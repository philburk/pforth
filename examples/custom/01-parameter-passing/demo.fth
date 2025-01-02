\ f4711 is a clear indicator that compilation of custom functions was successful
." f4711( 0, 1, 10, 100, 1000 ) = ( "
   0 f4711 . ." , "
   1 f4711 . ." , "
  10 f4711 . ." , "
 100 f4711 . ." , "
1000 f4711 . ." )"
CR

\ example of passing passing strings from PForth to custom C code
." be-gone: "
s" terrible_nuisance.asm" be-gone dup 0= 
if 
  ." works."
  drop
else
  ." returns error=" .
then CR

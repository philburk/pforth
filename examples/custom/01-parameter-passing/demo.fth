\ f4711 is a clear indicator that compilation of custom functions was successful
." f4711( 0, 1, 10, 100, 1000 ) = ( "
   0 f4711 . ." , "
   1 f4711 . ." , "
  10 f4711 . ." , "
 100 f4711 . ." , "
1000 f4711 . ." )"
CR

\ example of passing strings from PForth to custom C code
: SHOW-FILE-INFO 
  FILE-INFO over -rot
  ."    " type cr 
  free-c
  ;
." FILE-INFO: " cr
s" Makefile"       SHOW-FILE-INFO
s" ../../examples" SHOW-FILE-INFO
s" fileNotHere"    SHOW-FILE-INFO

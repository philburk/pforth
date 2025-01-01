argc ." argc=" . cr

argc 0 do
  i dup ." 
  argv[" (.) type ." ] = '" 
  argv type ." '" cr
loop

: SYNONYM
  create immediate bl word find
  dup 0= if ." could not find " drop count type -13 throw then
  -1 = , , does> 2@
    state @ and dup . if
      compile,
    else
      execute dup .
    then ;

synonym [DEFINED] exists?

: [UNDEFINED] POSTPONE [DEFINED] 0= ;

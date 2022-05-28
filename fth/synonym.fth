: SYNONYM
  create bl word find
  dup 0= if ." could not find " drop count type -13 throw then
  1 = if immediate then , does> @ execute ;

synonym [DEFINED] exists?
: [UNDEFINED] [DEFINED] 0= ;

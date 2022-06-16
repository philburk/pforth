: BEGIN-STRUCTURE ( "<spaces>name" -- struct-sys 0 ) create here 0 , 0 does> @ ;
: END-STRUCTURE ( struct-sys +n -- ) swap ! ;
: +FIELD ( n1 n2 "<spaces>name" -- n3 ) create over , + does> @ + ;
: CFIELD: ( n1 "<spaces>name" -- n2 ) 1 +field ; \ assumes chars have size 1, otherwise we would need to perform character alignment
: FIELD: ( n1 "<spaces>name" -- n2 ) aligned cell +field ;
exists? F* [IF]
: FFIELD: ( n1 "<spaces>name" -- n2 ) faligned [ 0 float+ ] literal +field ;
[THEN]

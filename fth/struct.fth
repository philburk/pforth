: BEGIN-STRUCTURE create here 0 , 0 does> @ ;
: END-STRUCTURE swap ! ;
: +FIELD create over , + does> @ + ;
: CFIELD: 1 +field ; \ assumes chars have size 1, otherwise we would need to perform character alignment
: FIELD: aligned cell +field ;
exists? F* [IF]
: FFIELD: faligned [ 0 float+ ] literal +field ;
[THEN]

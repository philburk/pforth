\ @(#) t_tools.fth 97/12/10 1.1
\ Test Tools for pForth
\
\ Based on testing tools from John Hayes
\ (c) 1993 Johns Hopkins University / Applied Physics Laboratory
\
\ Syntax was changed to avoid conflict with { -> and } for local variables.
\ Also added tracking of #successes and #errors.

anew task-t_tools.fth

decimal

variable TEST-DEPTH
variable TEST-PASSED
variable TEST-FAILED
40 constant TEST_EXIT_FAILURE \ returned form pForth to shell

: TEST{
    depth test-depth !
    0 test-passed !
    0 test-failed !
;


: }TEST
    test-passed @ 4 .r ."  passed, "
    test-failed @ 4 .r ."  failed." cr
    test-failed @ 0> IF
        TEST_EXIT_FAILURE bye-code !
    THEN
;


VARIABLE actual-depth       \ stack record
CREATE actual-results 20 CELLS ALLOT

: empty-stack \ ( ... -- ) Empty stack.
   DEPTH dup 0>
   IF 0 DO DROP LOOP
   ELSE drop
   THEN ;

CREATE the-test 128 CHARS ALLOT

: ERROR     \ ( c-addr u -- ) Display an error message followed by
        \ the line that had the error.
   TYPE the-test COUNT TYPE CR \ display line corresponding to error
   empty-stack          \ throw away every thing else
;


: T{
    source the-test place
    empty-stack
;

: }T{   \ ( ... -- ) Record depth and content of stack.
    DEPTH actual-depth !    \ record depth
    DEPTH 0
    ?DO
        actual-results I CELLS + !
    LOOP \ save them
;


: int>str s>d swap over dabs <# #s rot sign #> ;
: concat { addr1 len1 addr2 len2 | addr3 len3 -- addr3 len3 }
  \ concatenates string at addr2 to string at addr1
  len1 len2 + dup -> len3
  chars allocate abort" panic: can not allocate result buffer in concat" -> addr3
  addr1 addr3        len1 cmove
  addr2 addr3 len1 + len2 cmove
  addr3 len3
  ;
: concat+free { addr1 len1 addr2 len2  -- addr3 len3 }
  addr1 len1 addr2 len2 concat addr1 free abort" panic: can not free temporary buffer-1 in concat+free" ;
: }T    \ ( ... -- ) Compare stack (expected) contents with saved
        \ (actual) contents.
    DEPTH
    actual-depth @ =
    IF  \ if depths match
        1 test-passed +!  \ assume will pass
        DEPTH 0
        ?DO             \ for each stack item
            actual-results I CELLS + @ \ compare actual with expected
            2dup
            =
            if
                2drop
            else
                >R >R
                -1 test-passed +!
                1 test-failed +!
                s" INCORRECT RESULT (expected=" R> int>str concat
                s" , got="         concat+free  R> int>str concat+free
                s" ): "            concat+free
                error
                LEAVE
            THEN
        LOOP
    ELSE                \ depth mismatch
        1 test-failed +!
        S" WRONG NUMBER OF RESULTS: " error
    THEN
;

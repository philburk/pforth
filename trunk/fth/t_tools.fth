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

: TEST{
        depth test-depth !
        0 test-passed !
        0 test-failed !
;


: }TEST
        test-passed @ 4 .r ."  passed, "
        test-failed @ 4 .r ."  failed." cr
;


VARIABLE actual-depth 		\ stack record
CREATE actual-results 20 CELLS ALLOT

: empty-stack \ ( ... -- ) Empty stack.
   DEPTH dup 0>
   IF 0 DO DROP LOOP
   ELSE drop
   THEN ;

CREATE the-test 128 CHARS ALLOT

: ERROR 	\ ( c-addr u -- ) Display an error message followed by
		\ the line that had the error.
   TYPE the-test COUNT TYPE CR \ display line corresponding to error
   empty-stack 			\ throw away every thing else
;


: T{
	source the-test place
	empty-stack
;

: }T{ 	\ ( ... -- ) Record depth and content of stack.
	DEPTH actual-depth ! 	\ record depth
	DEPTH 0
	?DO
		actual-results I CELLS + !
	LOOP \ save them
;

: }T 	\ ( ... -- ) Compare stack (expected) contents with saved
		\ (actual) contents.
	DEPTH
	actual-depth @ =
	IF 	\ if depths match
		1 test-passed +!  \ assume will pass
		DEPTH 0
		?DO 			\ for each stack item
			actual-results I CELLS + @ \ compare actual with expected
			<>
			IF
				-1 test-passed +!
				1 test-failed +!
				S" INCORRECT RESULT: " error
				LEAVE
			THEN
		LOOP
	ELSE 				\ depth mismatch
		1 test-failed +!
		S" WRONG NUMBER OF RESULTS: " error
	THEN
;

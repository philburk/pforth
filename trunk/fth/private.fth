\ @(#) private.fth 98/01/26 1.2
\ PRIVATIZE
\
\ Privatize words that are only needed within the file
\ and do not need to be exported.
\
\ Usage:
\    PRIVATE{
\    : FOO ;  \ Everything between PRIVATE{ and }PRIVATE will become private.
\    : MOO ;
\    }PRIVATE
\    : GOO   foo moo ;  \ can use foo and moo
\    PRIVATIZE          \ smudge foo and moo
\    ' foo              \ will fail
\
\ Copyright 1996 Phil Burk
\
\ 19970701 PLB Use unsigned compares for machines with "negative" addresses.

anew task-private.fth

variable private-start
variable private-stop
$ 20 constant FLAG_SMUDGE

: PRIVATE{
	latest private-start !
	0 private-stop !
;
: }PRIVATE
	private-stop @ 0= not abort" Extra }PRIVATE"
	latest private-stop !
;
: PRIVATIZE  ( -- , smudge all words between PRIVATE{ and }PRIVATE )
	private-start @ 0= abort" Missing PRIVATE{"
	private-stop @ 0= abort" Missing }PRIVATE"
	private-stop @
	BEGIN
		dup private-start @ u>    \ 19970701
	WHILE
\		." Smudge " dup id. cr
		dup c@ flag_smudge or over c!
		prevname
	REPEAT
	drop
	0 private-start !
	0 private-stop !
;

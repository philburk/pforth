\ @(#) case.fth 98/01/26 1.2
\ CASE Statement
\
\ This definition is based upon Wil Baden's assertion that
\ >MARK >RESOLVE ?BRANCH etc. are not needed if one has IF ELSE THEN etc.
\
\ Author: Phil Burk
\ Copyright 1994 3DO, Phil Burk, Larry Polansky, Devid Rosenboom
\
\ The pForth software code is dedicated to the public domain,
\ and any third party may reproduce, distribute and modify
\ the pForth software code or any derivative works thereof
\ without any compensation or license.  The pForth software
\ code is provided on an "as is" basis without any warranty
\ of any kind, including, without limitation, the implied
\ warranties of merchantability and fitness for a particular
\ purpose and their equivalents under the laws of any jurisdiction.
\
\ MOD: PLB 6/24/91 Check for missing ENDOF
\ MOD: PLB 8/7/91 Add ?OF and RANGEOF
\ MOD: PLB 11/2/99 Fixed nesting of CASE. Needed to save of-depth on stack as well as case-depth.

anew TASK-CASE

variable CASE-DEPTH
variable OF-DEPTH

: CASE  ( n -- , start case statement ) ( -c- case-depth )
	?comp
	of-depth @   0 of-depth !   \ 11/2/99
	case-depth @ 0 case-depth !  ( allow nesting )
; IMMEDIATE

: ?OF  ( n flag -- | n , doit if true ) ( -c- addr )
	[compile] IF
	compile drop
	1 case-depth +!
	1 of-depth +!
; IMMEDIATE

: OF  ( n t -- | n , doit if match ) ( -c- addr )
	?comp
	compile over compile =
	[compile] ?OF
; IMMEDIATE

: (RANGEOF?)  ( n lo hi -- | n  flag )
	>r over ( n lo n ) <=
	IF
		dup r> ( n n hi ) <=
	ELSE
		rdrop false
	THEN
;

: RANGEOF  ( n lo hi -- | n , doit if within ) ( -c- addr )
	compile (rangeof?)
	[compile] ?OF
; IMMEDIATE

: ENDOF  ( -- ) ( addr -c- addr' )
	[compile] ELSE
	-1 of-depth +!
; IMMEDIATE

: ENDCASE ( n -- )  ( old-case-depth addr' addr' ??? -- )
	of-depth @
	IF >newline ." Missing ENDOF in CASE!" cr abort
	THEN
\
	compile drop
	case-depth @ 0
	?DO [compile] THEN
	LOOP
	case-depth !
	of-depth !
; IMMEDIATE


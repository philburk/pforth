\ @(#) $M$ 98/01/26 1.2
\ standard { v0 v1 ... vn | l0 l1 .. lm -- } syntax
\ based on ANSI basis words (LOCAL) and TO
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

\ MOD: PLB 2/11/00 Allow EOL and \ between { }.

anew task-locals.fth

private{
variable loc-temp-mode    \ if true, declaring temporary variables
variable loc-comment-mode \ if true, in comment section
variable loc-done
}private

: { ( <local-declaration}> -- )
	loc-done off
	loc-temp-mode off
	loc-comment-mode off
	BEGIN
		bl word count
		dup 0>           \ make sure we are not at the end of a line
		IF
			over c@
			CASE
	\ handle special characters
			ascii }  OF  loc-done on          2drop  ENDOF
			ascii |  OF  loc-temp-mode on     2drop  ENDOF
			ascii -  OF  loc-comment-mode on  2drop  ENDOF
			ascii )  OF  ." { ... ) imbalance!" cr abort  ENDOF
			ascii \  OF  postpone \  2drop ENDOF   \ Forth comment
			
	\ process name
			>r  ( save char )
			( addr len )
			loc-comment-mode @
			IF
				2drop
			ELSE
	\ if in temporary mode, assign local var = 0
				loc-temp-mode @
				IF compile false
				THEN
	\ otherwise take value from stack
				(local)
			THEN
			r>
			ENDCASE
		ELSE
			2drop refill 0= abort" End of input while defining local variables!"
		THEN
		loc-done @
	UNTIL
	0 0 (local)
; immediate

privatize

\ tests
: tlv1  { n -- }  n  dup n *  dup n *  ;

: tlv2 { v1 v2 | l1 l2 -- }
	v1 . v2 . cr
	v1 v2 + -> l1
	l1 . l2 . cr
;

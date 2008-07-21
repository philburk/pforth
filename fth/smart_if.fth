\ @(#) smart_if.fth 98/01/26 1.2
\ Smart Conditionals
\ Allow use of if, do, begin, etc.outside of colon definitions.
\
\ Thanks to Mitch Bradley for the idea.
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

anew task-smart_if.fth

variable SMIF-XT    \ execution token for conditional code
variable SMIF-DEPTH \ depth of nested conditionals

: SMIF{   ( -- , if executing, start compiling, setup depth )
	state @ 0=
	IF
		:noname smif-xt !
		1 smif-depth !
	ELSE
		1 smif-depth +!
	THEN
;

: }SMIF  ( -- , unnest, stop compiling, execute code and forget )
	smif-xt @
	IF
		-1 smif-depth +!
		smif-depth @ 0 <=
		IF
			postpone ;             \ stop compiling
			smif-xt @ execute      \ execute conditional code
			smif-xt @ >code dp !   \ forget conditional code
			0 smif-xt !   \ clear so we don't mess up later
		THEN
	THEN
;
		
\ redefine conditionals to use smart mode
: IF      smif{   postpone if     ; immediate
: DO      smif{   postpone do     ; immediate
: ?DO     smif{   postpone ?do    ; immediate
: BEGIN   smif{   postpone begin  ; immediate
: THEN    postpone then    }smif  ; immediate
: REPEAT  postpone repeat  }smif  ; immediate
: UNTIL   postpone until   }smif  ; immediate
: LOOP    postpone loop    }smif  ; immediate
: +LOOP   postpone +loop   }smif  ; immediate

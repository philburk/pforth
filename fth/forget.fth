\ @(#) forget.fth 98/01/26 1.2
\ forget.fth
\
\ forget part of dictionary
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
\ 19970701 PLB Use unsigned compares for machines with "negative" addresses.

variable RFENCE    \ relocatable value below which we won't forget

: FREEZE  ( -- , protect below here )
	here rfence a!
;

: FORGET.NFA  ( nfa -- , set DP etc. )
	dup name> >code dp !
	prevname ( dup current ! ) dup context ! n>nextlink headers-ptr !
;

: VERIFY.FORGET  ( nfa -- , ask for verification if below fence )
	dup name> >code rfence a@ u<  \ 19970701
	IF
		>newline dup id. ."  is below fence!!" cr
		drop
	ELSE forget.nfa
	THEN
;

: (FORGET)  ( <name> -- )
	BL word findnfa
	IF	verify.forget
	ELSE ." FORGET - couldn't find " count type cr abort
	THEN
;

variable LAST-FORGET   \ contains address of last if.forgotten frame
0 last-forget !

: IF.FORGOTTEN  ( <name> -- , place links in dictionary without header )
	bl word find
	IF	( xt )
		here                \ start of frame
		last-forget a@ a,   \ Cell[0] = rel address of previous frame
		last-forget a!      \ point to this frame
		compile,            \ Cell[1] = xt for this frame
	ELSE ." IF.FORGOTTEN - couldn't find " dup 9 dump cr count type cr abort
	THEN
;
if.forgotten noop

: [FORGET]  ( <name> -- , forget then exec forgotten words )
	(forget)
	last-forget
	BEGIN a@ dup 0<>   \ 19970701
		IF dup here u>   \ 19970701
			IF dup cell+ x@ execute false
			ELSE dup last-forget a! true
			THEN
		ELSE true
		THEN
	UNTIL drop
;

: FORGET ( <name> -- , execute latest [FORGET] )
	" [FORGET]" find
	IF  execute
	ELSE ." FORGET - couldn't find " count type cr abort
	THEN
;

: ANEW ( -- , forget if defined then redefine )
	>in @
	bl word find
	IF over >in ! forget
	THEN drop
	>in ! variable
;

: MARKER  ( <name> -- , define a word that forgets itself when executed, ANS )
	CREATE
		latest namebase -  \ convert to relocatable
		,                  \ save for DOES>
	DOES>  ( -- body )
		@ namebase +       \ convert back to NFA
		verify.forget
;

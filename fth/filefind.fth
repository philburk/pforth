\ @(#) filefind.fth 98/01/26 1.2
\ FILE?  ( <name> -- , report which file this Forth word was defined in )
\
\ FILE? looks for ::::Filename and ;;;; in the dictionary
\ that have been left by INCLUDE.  It figures out nested
\ includes and reports each file that defines the word.
\
\ Author: Phil Burk
\ Copyright 1992 Phil Burk
\
\ 00001 PLB 2/21/92 Handle words from kernel or keyboard.
\		Support EACH.FILE?
\ 961213 PLB Port to pForth.

ANEW TASK-FILEFIND.FTH

: BE@ { addr | val -- val , fetch from unaligned address in BigEndian order }
	4 0
	DO
		addr i + c@
		val 8 lshift or -> val
	LOOP
	val
;

: BE! { val addr -- , store to unaligned address in BigEndian order }
	4 0
	DO
	    val 3 i - 8 * rshift
		addr i + c!
	LOOP
;
: BEW@ { addr -- , fetch word from unaligned address in BigEndian order }
	addr c@ 8 lshift
	addr 1+ c@ OR
;

: BEW! { val addr -- , store word to unaligned address in BigEndian order }
	val 8 rshift addr c!
	val addr 1+ c!
;

\ scan dictionary from NFA for filename
: F?.SEARCH.NFA { nfa | dpth stoploop keyb nfa0 -- addr count }
	0 -> dpth
	0 -> stoploop
	0 -> keyb
	nfa -> nfa0
	BEGIN
		nfa prevname -> nfa
		nfa 0>
		IF
			nfa 1+ be@
			CASE
				$ 3a3a3a3a ( :::: )
				OF
					dpth 0=
					IF
						nfa count 31 and
						4 - swap 4 + swap
						true -> stoploop
					ELSE
						-1 dpth + -> dpth
					THEN
				ENDOF
				$ 3b3b3b3b ( ;;;; )
				OF
						1 dpth + -> dpth
						true -> keyb     \ maybe from keyboard
				ENDOF
			ENDCASE
		ELSE
			true -> stoploop
			keyb
			IF
				" keyboard"
			ELSE
				" 'C' kernel"
			THEN
			count
		THEN
		stoploop
	UNTIL
;

: FINDNFA.FROM { $name start_nfa -- nfa true | $word false }
	context @ >r
	start_nfa context !
	$name findnfa
	r> context !
;

\ Search entire dictionary for all occurences of named word.
: FILE? {  | $word nfa done? -- , take name from input }
	0 -> done?
	bl word -> $word
	$word findnfa
	IF  ( -- nfa )
		$word count type ."  from:" cr
		-> nfa
		BEGIN
			nfa f?.search.nfa ( addr cnt )
			nfa name> 12 .r   \ print xt
			4 spaces type cr
			nfa prevname dup -> nfa
			0>
			IF
				$word nfa findnfa.from  \ search from one behind found nfa
				swap -> nfa
				not
			ELSE
				true
			THEN
		UNTIL
	ELSE ( -- $word )
		count type ."  not found!" cr
	THEN
;


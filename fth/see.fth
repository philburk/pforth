\ @(#) see.fth 98/01/26 1.4
\ SEE ( <name> -- , disassemble pForth word )
\
\ Copyright 1996 Phil Burk

' file? >code rfence a!

anew task-see.fth

: .XT ( xt -- , print execution tokens name )
	>name
	dup c@ flag_immediate and
	IF
		." POSTPONE "
	THEN
	id. space
;

\ dictionary may be defined as byte code or cell code
0 constant BYTE_CODE

BYTE_CODE [IF]
	: CODE@ ( addr -- xt , fetch from code space )   C@ ;
	1 constant CODE_CELL
	.( BYTE_CODE not implemented) abort
[ELSE]
	: CODE@ ( addr -- xt , fetch from code space )   @ ;
	CELL constant CODE_CELL
[THEN]

private{

0 value see_level  \ level of conditional imdentation
0 value see_addr   \ address of next token
0 value see_out

: SEE.INDENT.BY ( -- n )
	see_level 1+ 1 max 4 *
;

: SEE.CR
	>newline
	see_addr ." ( ".hex ." )"
	see.indent.by spaces
	0 -> see_out
;
: SEE.NEWLINE
	see_out 0>
	IF see.cr
	THEN
;
: SEE.CR?
	see_out 6 >
	IF
		see.newline
	THEN
;
: SEE.OUT+
	1 +-> see_out
;

: SEE.ADVANCE
	code_cell +-> see_addr
;
: SEE.GET.INLINE ( -- n )
	see_addr @
;

: SEE.GET.TARGET  ( -- branch-target-addr )
	see_addr @ see_addr +
;

: SEE.SHOW.LIT ( -- )
	see.get.inline .
	see.advance
	see.out+
;

exists? F* [IF]
: SEE.SHOW.FLIT ( -- )
	see_addr f@ f.
	1 floats +-> see_addr
	see.out+
;
[THEN]

: SEE.SHOW.ALIT ( -- )
	see.get.inline >name id. space
	see.advance
	see.out+
;

: SEE.SHOW.STRING ( -- )
	see_addr count 2dup + aligned -> see_addr type
	see.out+
;
: SEE.SHOW.TARGET ( -- )
	see.get.target .hex see.advance
;

: SEE.BRANCH ( -- addr | , handle branch )
	-1 +-> see_level
	see.newline 
	see.get.inline  0>
	IF  \ forward branch
		." ELSE "
		see.get.target \ calculate address of target
		1 +-> see_level
		nip \ remove old address for THEN
	ELSE
		." REPEAT " see.get.target .hex
		drop \ remove old address for THEN
	THEN
	see.advance
	see.cr
;

: SEE.0BRANCH ( -- addr | , handle 0branch )
	see.newline 
	see.get.inline 0>
	IF  \ forward branch
		." IF or WHILE "
		see.get.target \ calculate adress of target
		1 +-> see_level
	ELSE
		." UNTIL=>" see.get.target .hex
	THEN
	see.advance
	see.cr
;

: SEE.XT  { xt -- }
	xt
	CASE
		0 OF see_level 0> IF ." EXIT " see.out+ ELSE ." ;" 0  -> see_addr THEN ENDOF
		['] (LITERAL) OF see.show.lit ENDOF
		['] (ALITERAL) OF see.show.alit ENDOF
[ exists? (FLITERAL) [IF] ]
		['] (FLITERAL) OF see.show.flit ENDOF
[ [THEN] ]
		['] BRANCH    OF see.branch ENDOF
		['] 0BRANCH   OF see.0branch ENDOF
		['] (LOOP)    OF -1 +-> see_level see.newline ." LOOP " see.advance see.cr  ENDOF
		['] (+LOOP)   OF -1 +-> see_level see.newline ." +LOOP" see.advance see.cr  ENDOF
		['] (DO)      OF see.newline ." DO" 1 +-> see_level see.cr ENDOF
		['] (?DO)     OF see.newline ." ?DO " see.advance 1 +-> see_level see.cr ENDOF
		['] (.") OF .' ." ' see.show.string .' " ' ENDOF
		['] (C") OF .' C" ' see.show.string .' " ' ENDOF
		['] (S") OF .' S" ' see.show.string .' " ' ENDOF

		see.cr? xt .xt see.out+
	ENDCASE
;

: (SEE) { cfa | xt  -- }
	0 -> see_level
	cfa -> see_addr
	see.cr
	0 \ fake address for THEN handler
	BEGIN
		see_addr code@ -> xt
		BEGIN
			dup see_addr ( >newline .s ) =
		WHILE
			-1 +-> see_level see.newline 
			." THEN " see.cr
			drop
		REPEAT
		CODE_CELL +-> see_addr
		xt see.xt
		see_addr 0=
	UNTIL
	cr
	0= not abort" SEE conditional analyser nesting failed!"
;

}PRIVATE

: SEE  ( <name> -- , disassemble )
	'
	dup ['] FIRST_COLON >
	IF
		>code (see)
	ELSE
		>name id.
		."  is primitive defined in 'C' kernel." cr
	THEN
;

PRIVATIZE

0 [IF]

: SEE.JOKE
	dup swap drop
;

: SEE.IF
	IF
		." hello" cr
	ELSE
		." bye" cr
	THEN
	see.joke
;
: SEE.DO
	4 0
	DO
		i . cr
	LOOP
;
: SEE."
	." Here are some strings." cr
	c" Forth string." count type cr
	s" Addr/Cnt string" type cr
;

[THEN]

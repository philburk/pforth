\ @(#) clone.fth 97/12/10 1.1
\ Clone for PForth
\
\ Create the smallest dictionary required to run an application.
\
\ Clone decompiles the Forth dictionary starting with the top
\ word in the program.  It then moves all referenced secondaries
\ into a new dictionary.
\
\ This work was inspired by the CLONE feature that Mike Haas wrote
\ for JForth.  Mike's CLONE disassembled 68000 machine code then
\ reassembled it which is much more difficult.
\
\ Copyright Phil Burk & 3DO 1994
\
\ O- trap custom 'C' calls
\ O- investigate ALITERAL, XLITERAL, use XLITERAL in [']

anew task-clone.fth
decimal

\ move to 'C'
: PRIMITIVE? ( xt -- flag , true if primitive )
	['] FIRST_COLON <
;

: 'SELF ( -- xt , return xt of word being compiled )
	?comp
	latest name>
	[compile] literal
; immediate


:struct CL.REFERENCE
	long  clr_OriginalXT    \ original XT of word
	long  clr_NewXT         \ corresponding XT in cloned dictionary
	long  clr_TotalSize     \ size including data in body
;struct

variable CL-INITIAL-REFS \ initial number of refs to allocate
100 cl-initial-refs !
variable CL-REF-LEVEL    \ level of threading while scanning
variable CL-NUM-REFS     \ number of secondaries referenced
variable CL-MAX-REFS     \ max number of secondaries allocated
variable CL-LEVEL-MAX    \ max level reached while scanning
variable CL-LEVEL-ABORT  \ max level before aborting
10 cl-level-abort !
variable CL-REFERENCES   \ pointer to cl.reference array
variable CL-TRACE        \ print debug stuff if true

\ Cloned dictionary builds in allocated memory but XTs are relative
\ to normal code-base, if CL-TEST-MODE true.
variable CL-TEST-MODE
 
variable CL-INITIAL-DICT \ initial size of dict to allocate
20 1024 * cl-initial-dict !
variable CL-DICT-SIZE    \ size of allocated cloned dictionary
variable CL-DICT-BASE    \ pointer to virtual base of cloned dictionary
variable CL-DICT-ALLOC   \ pointer to allocated dictionary memory
variable CL-DICT-PTR     \ rel pointer index into cloned dictionary
0 cl-dict-base !

	
: CL.INDENT ( -- )
	cl-ref-level @ 2* 2* spaces
;
: CL.DUMP.NAME ( xt -- )
	cl.indent
	>name id. cr
;

: CL.DICT[] ( relptr -- addr )
	cl-dict-base @ +
;

: CL,  ( cell -- , comma into clone dictionary )
	cl-dict-ptr @ cl.dict[] !
	cell cl-dict-ptr +!
;


: CL.FREE.DICT ( -- , free dictionary we built into )
	cl-dict-alloc @ ?dup
	IF
		free dup ?error
		0 cl-dict-alloc !
	THEN
;

: CL.FREE.REFS ( -- , free dictionary we built into )
	cl-references @ ?dup
	IF
		free dup ?error
		0 cl-references !
	THEN
;

: CL.ALLOC.REFS ( --  , allocate references to track )
	cl-initial-refs @  \ initial number of references
	dup cl-max-refs ! \ maximum allowed
	sizeof() cl.reference *
	allocate dup ?error
	cl-references !
;

: CL.RESIZE.REFS ( -- , allocate references to track )
	cl-max-refs @   \ current number of references allocated
	5 * 4 / dup cl-max-refs ! \ new maximum allowed
\ cl.indent ." Resize # references to " dup . cr
	sizeof() cl.reference *
	cl-references @ swap resize dup ?error
	cl-references !
;


: CL.ALLOC.DICT ( -- , allocate dictionary to build into )
	cl-initial-dict @  \ initial dictionary size
	dup cl-dict-size !
	allocate dup ?error
	cl-dict-alloc !
\
\ kludge dictionary if testing
	cl-test-mode @
	IF
		cl-dict-alloc @ code-base @ - cl-dict-ptr +!
		code-base @ cl-dict-base !
	ELSE
		cl-dict-alloc @  cl-dict-base !
	THEN
	." CL.ALLOC.DICT" cr
	."   cl-dict-alloc = $" cl-dict-alloc @ .hex cr
	."   cl-dict-base  = $" cl-dict-base @ .hex cr
	."   cl-dict-ptr   = $" cl-dict-ptr @ .hex cr
;

: CODEADDR>DATASIZE { code-addr -- datasize }
\ Determine size of any literal data following execution token.
\ Examples are text following (."), or branch offsets.
	code-addr @
	CASE
	['] (literal) OF cell ENDOF   \ a number
	['] 0branch   OF cell ENDOF   \ branch offset
	['] branch    OF cell ENDOF
	['] (do)      OF    0 ENDOF
	['] (?do)     OF cell ENDOF
	['] (loop)    OF cell ENDOF
	['] (+loop)   OF cell ENDOF
	['] (.")      OF code-addr cell+ c@ 1+ ENDOF  \ text
	['] (s")      OF code-addr cell+ c@ 1+ ENDOF
	['] (c")      OF code-addr cell+ c@ 1+ ENDOF
	0 swap
	ENDCASE
;

: XT>SIZE  ( xt -- wordsize , including code and data )
	dup >code
	swap >name
	dup latest =
	IF
		drop here
	ELSE
		dup c@ 1+ + aligned 8 + \ get next name
		name> >code \ where is next word
	THEN
	swap -
;

\ ------------------------------------------------------------------
: CL.TRAVERSE.SECONDARY { code-addr ca-process | xt dsize --  }
\ scan secondary and pass each code-address to ca-process
\ CA-PROCESS ( code-addr -- , required stack action for vector )
	1 cl-ref-level +!
	cl-ref-level @ cl-level-abort @ > abort" Clone exceeded CL-ABORT-LEVEL"
	BEGIN
		code-addr @ -> xt
\ cl.indent ." CL.TRAVERSE.SECONDARY - code-addr = $" code-addr .hex ." , xt = $" xt .hex cr
		code-addr codeaddr>datasize -> dsize      \ any data after this?
		code-addr ca-process execute              \ process it
		code-addr cell+ dsize + aligned -> code-addr  \ skip past data
\ !!! Bummer! EXIT called in middle of secondary will cause early stop.
		xt  ['] EXIT  =                           \ stop when we get to EXIT
	UNTIL
	-1 cl-ref-level +!
;

\ ------------------------------------------------------------------

: CL.DUMP.XT ( xt -- )
	cl-trace @
	IF
		dup primitive?
		IF   ." PRI:  "
		ELSE ." SEC:  "
		THEN
		cl.dump.name
	ELSE
		drop
	THEN
;

\ ------------------------------------------------------------------
: CL.REF[] ( index -- clref )
	sizeof() cl.reference *
	cl-references @ +
;

: CL.DUMP.REFS ( -- , print references )
	cl-num-refs @ 0
	DO
		i 3 .r ."  : "
		i cl.ref[]
		dup s@ clr_OriginalXT >name id. ."  => "
		dup s@ clr_NewXT .
		." , size = "
		dup s@ clr_TotalSize . cr
		drop \ clref
	loop
;			
		
: CL.XT>REF_INDEX { xt | indx flag -- index flag , true if found }
	BEGIN
\ cl.indent ." CL.XT>REF_INDEX - indx = " indx . cr
		indx cl-num-refs @ >=
		IF
			true
		ELSE
			indx cl.ref[] s@ clr_OriginalXT
\ cl.indent ." CL.XT>REF_INDEX - clr_OriginalXT = " dup . cr
			xt  =
			IF
				true
				dup -> flag
			ELSE
				false
				indx 1+ -> indx
			THEN
		THEN
	UNTIL
	indx flag
\ cl.indent ." CL.XT>REF_INDEX - " xt >name id. space  indx . flag . cr
;			

: CL.ADD.REF  { xt | clref -- , add referenced secondary to list }
	cl-references @ 0= abort" CL.ADD.REF - References not allocated!"
\
\ do we need to allocate more room?
	cl-num-refs @ cl-max-refs @ >=
	IF
		cl.resize.refs
	THEN
\
	cl-num-refs @ cl.ref[] -> clref    \ index into array
	xt clref s! clr_OriginalXT
	0 clref s! clr_NewXT
	xt xt>size clref s! clr_TotalSize
\
	1 cl-num-refs +!
;

\ ------------------------------------------------------------------

\ called by cl.traverse.secondary to compile each piece of secondary
: CL.RECOMPILE.SECONDARY { code-addr | xt clref dsize -- ,  }
\ recompile to new location
\ cl.indent ." CL.RECOMPILE.SECONDARY - enter - " .s cr
	code-addr @ -> xt
\ cl.indent ." CL.RECOMPILE.SECONDARY - xt = $" dup .hex dup >name id. cr
	xt cl.dump.xt
	xt primitive?
	IF
		xt cl,
	ELSE
		xt CL.XT>REF_INDEX
		IF
			cl.ref[] -> clref
			clref s@ clr_NewXT
			dup 0= abort" CL.RECOMPILE.SECONDARY - unresolved NewXT"
			cl,
		ELSE
			cl.indent ." CL.RECOMPILE.SECONDARY - xt not in ref table!" cr
			abort
		THEN
	THEN
\
\ transfer any literal data
	code-addr codeaddr>datasize -> dsize
	dsize 0>
	IF
\ cl.indent ." CL.RECOMPILE.SECONDARY - copy inline data of size" dsize . cr
		code-addr cell+  cl-dict-ptr @ cl.dict[]  dsize  move
		cl-dict-ptr @ dsize + aligned cl-dict-ptr !
	THEN
\ cl.indent ." CL.RECOMPILE.SECONDARY - leave - " .s cr
;

: CL.RECOMPILE.REF { indx | clref codesize datasize -- }
\ all references have been resolved so recompile new secondary
	depth >r
	indx cl.ref[] -> clref
	cl-trace @
	IF
		cl.indent
		clref s@ clr_OriginalXT >name id. ."  recompiled at $"
		cl-dict-ptr @ .hex cr    \ new address
	THEN
	cl-dict-ptr @  clref s! clr_NewXT
\
\ traverse this secondary and compile into new dictionary
	clref s@ clr_OriginalXT
	>code ['] cl.recompile.secondary cl.traverse.secondary
\
\ determine whether there is any data following definition
	cl-dict-ptr @
	clref s@ clr_NewXT - -> codesize \ size of cloned code
	clref s@ clr_TotalSize \ total bytes
	codesize - -> datasize
	cl-trace @
	IF
		cl.indent
		." Move data: data size = " datasize . ." codesize = " codesize . cr
	THEN
\
\ copy any data that followed definition
	datasize 0>
	IF
		clref s@ clr_OriginalXT >code codesize +
		clref s@ clr_NewXT cl-dict-base @ + codesize +
		datasize move
		datasize cl-dict-ptr +!  \ allot space in clone dictionary
	THEN
	
	depth r> - abort" Stack depth change in CL.RECOMPILE.REF"
;

\ ------------------------------------------------------------------
: CL.SCAN.SECONDARY ( code-addr -- , scan word and add referenced secondaries to list )
	depth 1- >r
\ cl.indent ." CL.SCAN.SECONDARY - enter - " .s cr
	cl-ref-level @ cl-level-max @  MAX cl-level-max !
	@ ( get xt )
\ cl.indent ." CL.SCAN.SECONDARY - xt = " dup . dup >name id. cr
	dup cl.dump.xt
	dup primitive?
	IF
		drop
\ cl.indent ." CL.SCAN.SECONDARY - found primitive." cr
	ELSE
		dup CL.XT>REF_INDEX
		IF
			drop \ indx   \ already referenced once so ignore
			drop \ xt
		ELSE
			>r \ indx
			dup cl.add.ref
			>code 'self cl.traverse.secondary   \ use 'self for recursion!
			r> cl.recompile.ref    \ now that all refs resolved, recompile
		THEN
	THEN
\ cl.indent ." CL.SCAN.SECONDARY - leave - " .s cr
	depth r> - abort" Stack depth change in CL.SCAN.SECONDARY"
;

: CL.CLONE.XT ( xt -- , scan top word and add referenced secondaries to list )
	dup primitive? abort" Cannot CLONE a PRIMITIVE word!"
	0 cl-ref-level !
	0 cl-level-max !
	0 cl-num-refs !
	dup cl.add.ref     \ word being cloned is top of ref list
	>code ['] cl.scan.secondary cl.traverse.secondary
	0 cl.recompile.ref
;

\ ------------------------------------------------------------------
: CL.XT>NEW_XT ( xt -- xt' , convert normal xt to xt in cloned dict )
	cl.xt>ref_index 0= abort" not in cloned dictionary!"
	cl.ref[] s@ clr_NewXT
;
: CL.XT>NEW_ADDR ( xt -- addr , addr in cloned dict )
	cl.xt>New_XT
	cl-dict-base @ +
;

: CL.REPORT ( -- )
	." Clone scan went " cl-level-max @ . ." levels deep." cr
	." Clone scanned " cl-num-refs @ . ." secondaries." cr
	." New dictionary size =  " cl-dict-ptr @ cl-dict-base @ - . cr
;


\ ------------------------------------------------------------------
: CL.TERM ( -- , cleanup )
	cl.free.refs
	cl.free.dict
;

: CL.INIT ( -- )
	cl.term
	0 cl-dict-size !
	['] first_colon cl-dict-ptr !
	cl.alloc.dict
	cl.alloc.refs
;

: 'CLONE ( xt -- , clone dictionary from this word )
	cl.init
	cl.clone.xt
	cl.report
	cl.dump.refs
	cl-test-mode @
	IF ." WARNING - CL-TEST-MODE on so we can't save cloned image." cr
	THEN
;

: SAVE-CLONE  ( <filename> -- )
	bl word
	." Save cloned image in " dup count type
	drop ." SAVE-CLONE unimplemented!" \ %Q
;

: CLONE ( <name> -- )
	' 'clone
;

if.forgotten cl.term

\ ---------------------------------- TESTS --------------------


: TEST.CLONE ( -- )
	cl-test-mode @ not abort" CL-TEST-MODE not on!"
	0 cl.ref[] s@ clr_NewXT  execute
;


: TEST.CLONE.REAL ( -- )
	cl-test-mode @ abort" CL-TEST-MODE on!"
	code-base @
	0 cl.ref[] s@ clr_NewXT  \ get cloned execution token
	cl-dict-base @ code-base !
\ WARNING - code-base munged, only execute primitives or cloned code
	execute
	code-base !   \ restore code base for normal 
;


: TCL1
	34 dup +
;

: TCL2
	." Hello " tcl1  . cr
;

: TCL3
	4 0
	DO
		tcl2
		i . cr
		i 100 + . cr
	LOOP
;

create VAR1 567 ,
: TCL4
	345 var1 !
	." VAR1 = " var1 @ . cr
	var1 @ 345 -
	IF
		." TCL4 failed!" cr
	ELSE
		." TCL4 succeded! Yay!" cr
	THEN
;

\ do deferred words get cloned!
defer tcl.vector

: TCL.DOIT ." Hello Fred!" cr ;
' tcl.doit is tcl.vector

: TCL.DEFER
	12 . cr
	tcl.vector
	999 dup + . cr
;

trace-stack on
cl-test-mode on


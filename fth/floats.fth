\ @(#) floats.fth 98/02/26 1.4 17:51:40
\ High Level Forth support for Floating Point
\
\ Author: Phil Burk and Darren Gibbs
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
\ 19970702 PLB Drop 0.0 in REPRESENT to fix  0.0 F.
\ 19980220 PLB Added FG. , fixed up large and small formatting
\ 19980812 PLB Now don't drop 0.0 in REPRESENT to fix  0.0 F.  (!!!)
\              Fixed F~ by using (F.EXACTLY)

ANEW TASK-FLOATS.FTH

: FALIGNED	( addr -- a-addr )
	1 floats 1- +
	1 floats /
	1 floats *
;

: FALIGN	( -- , align DP )
	dp @ faligned dp !
;

\ account for size of create when aligning floats
here
create fp-create-size
fp-create-size swap - constant CREATE_SIZE

: FALIGN.CREATE  ( -- , align DP for float after CREATE )
	dp @
	CREATE_SIZE +
	faligned
	CREATE_SIZE -
	dp !
;

: FCREATE  ( <name> -- , create with float aligned data )
	falign.create
	CREATE
;

: FVARIABLE ( <name> -- ) ( F: -- )
	FCREATE 1 floats allot
;

: FCONSTANT
	FCREATE here   1 floats allot   f! 
	DOES> f@ 
;

: F0SP ( -- ) ( F: ? -- )
	fdepth 0 max  0 ?DO fdrop LOOP 
;

\ Convert between single precision and floating point
: S>F ( s -- ) ( F: -- r )
	s>d d>f
;
: F>S ( -- s ) ( F: r -- )
	f>d d>s
;		

: (F.EXACTLY) ( r1 r2 -f- flag , return true if encoded equally ) { | caddr1 caddr2 fsize fcells }
	1 floats -> fsize
	fsize cell 1- + cell 1- invert and  \ round up to nearest multiple of stack size
	cell / -> fcells ( number of cells per float )
\ make room on data stack for floats data
	fcells 0 ?DO 0 LOOP
	sp@ -> caddr1
	fcells 0 ?DO 0 LOOP
	sp@ -> caddr2
\ compare bit representation
	caddr1 f!
	caddr2 f!
	caddr1 fsize caddr2 fsize compare 0= 
	>r fcells 2* 0 ?DO drop LOOP r>  \ drop float bits
;

: F~ ( -0- flag ) ( r1 r2 r3 -f- )
	fdup F0<
	IF
		frot frot  ( -- r3 r1 r2 )
		fover fover ( -- r3 r1 r2 r1 r2 )
		f- fabs    ( -- r3 r1 r2 |r1-r2| )
		frot frot  ( -- r3  |r1-r2| r1 r2 )
		fabs fswap fabs f+ ( -- r3 |r1-r2|  |r1|+|r2| )
		frot fabs f* ( -- |r1-r2|  |r1|+|r2|*|r3| )
		f<
	ELSE
		fdup f0=
		IF
			fdrop
			(f.exactly)  \ f- f0=  \ 19980812 Used to cheat. Now actually compares bit patterns.
		ELSE
			frot frot  ( -- r3 r1 r2 )
			f- fabs    ( -- r3 |r1-r2| )
			fswap f<
		THEN
	THEN
;

\ FP Output --------------------------------------------------------
fvariable FVAR-REP  \ scratch var for represent
: REPRESENT { c-addr u | n flag1 flag2 --  n flag1 flag2 , FLOATING } ( F: r -- )
	TRUE -> flag2   \ FIXME - need to check range
	fvar-rep f!
\
	fvar-rep f@ f0<
	IF
		-1 -> flag1
		fvar-rep f@ fabs fvar-rep f!   \ absolute value
	ELSE
		0 -> flag1
	THEN
\
	fvar-rep f@ f0=
	IF
\		fdrop \ 19970702 \ 19980812 Remove FDROP to fix "0.0 F."
		c-addr u [char] 0 fill
		0 -> n
	ELSE
		fvar-rep f@ 
		flog
		fdup f0< not
		IF
			1 s>f f+ \ round up exponent
		THEN
		f>s -> n   
\ ." REP - n = " n . cr
\ normalize r to u digits
		fvar-rep f@
		10 s>f u n - s>f f** f*
		1 s>f 2 s>f f/ f+   \ round result
\
\ convert float to double_int then convert to text
		f>d
\ ." REP - d = " over . dup . cr
		<#  u 1- 0 ?DO # loop #s #>  \ ( -- addr cnt )
\ Adjust exponent if rounding caused number of digits to increase.
\ For example from 9999 to 10000.
		u - +-> n  
		c-addr u move
	THEN
\
	n flag1 flag2
;

variable FP-PRECISION

\ Set maximum digits that are meaningful for the precision that we use.
1 FLOATS 4 / 7 * constant FP_PRECISION_MAX

: PRECISION ( -- u )
	fp-precision @
;
: SET-PRECISION ( u -- )
	fp_precision_max min
	fp-precision !
;
7 set-precision

32 constant FP_REPRESENT_SIZE
64 constant FP_OUTPUT_SIZE

create FP-REPRESENT-PAD FP_REPRESENT_SIZE allot  \ used with REPRESENT
create FP-OUTPUT-PAD FP_OUTPUT_SIZE allot     \ used to assemble final output
variable FP-OUTPUT-PTR            \ points into FP-OUTPUT-PAD

: FP.HOLD ( char -- , add char to output )
	fp-output-ptr @ fp-output-pad 64 + <
	IF
		fp-output-ptr @ tuck c!
		1+ fp-output-ptr !
	ELSE
		drop
	THEN
;
: FP.APPEND { addr cnt -- , add string to output }
	cnt 0 max   0
	?DO
		addr i + c@ fp.hold
	LOOP
;

: FP.STRIP.TRAILING.ZEROS ( -- , remove trailing zeros from fp output )
	BEGIN
		fp-output-ptr @ fp-output-pad u>
		fp-output-ptr @ 1- c@ [char] 0 =
		and
	WHILE
		-1 fp-output-ptr +!
	REPEAT
;

: FP.APPEND.ZEROS ( numZeros -- )
	0 max   0
	?DO [char] 0 fp.hold
	LOOP
;

: FP.MOVE.DECIMAL   { n prec -- , append with decimal point shifted }
	fp-represent-pad n prec min fp.append
	n prec - fp.append.zeros
	[char] . fp.hold
	fp-represent-pad n +
	prec n - 0 max fp.append
;

: (EXP.) ( n -- addr cnt , convert exponent to two digit value )
	dup abs 0
	<# # #s
	rot 0<
	IF [char] - HOLD
	ELSE [char] + hold
	THEN
	#>
;

: FP.REPRESENT ( -- n flag1 flag2 ) ( r -f- )
;

: (FS.)  ( -- addr cnt ) ( F: r -- , scientific notation )
	fp-output-pad fp-output-ptr !  \ setup pointer
	fp-represent-pad   precision  represent
\ ." (FS.) - represent " fp-represent-pad precision type cr
	( -- n flag1 flag2 )
	IF
		IF [char] - fp.hold
		THEN
		1 precision fp.move.decimal
		[char] e fp.hold
		1- (exp.) fp.append \ n
	ELSE
		2drop
		s" <FP-OUT-OF-RANGE>" fp.append
	THEN
	fp-output-pad fp-output-ptr @ over -
;

: FS.  ( F: r -- , scientific notation )
	(fs.) type space
;

: (FE.)  ( -- addr cnt ) ( F: r -- , engineering notation ) { | n n3 -- }
	fp-output-pad fp-output-ptr !  \ setup pointer
	fp-represent-pad precision represent
	( -- n flag1 flag2 )
	IF
		IF [char] - fp.hold
		THEN
\ convert exponent to multiple of three
		-> n
		n 1- s>d 3 fm/mod \ use floored divide
		3 * -> n3
		1+ precision fp.move.decimal \ amount to move decimal point
		[char] e fp.hold
		n3 (exp.) fp.append \ n
	ELSE
		2drop
		s" <FP-OUT-OF-RANGE>" fp.append
	THEN
	fp-output-pad fp-output-ptr @ over -
;

: FE.  ( F: r -- , engineering notation )
	(FE.) type space
;

: (FG.)  ( F: r -- , normal or scientific ) { | n n3 ndiff -- }
	fp-output-pad fp-output-ptr !  \ setup pointer
	fp-represent-pad precision represent
	( -- n flag1 flag2 )
	IF
		IF [char] - fp.hold
		THEN
\ compare n with precision to see whether we do scientific display
		dup precision >
		over -3 < OR
		IF  \ use exponential notation
			1 precision fp.move.decimal
			fp.strip.trailing.zeros
			[char] e fp.hold
			1- (exp.) fp.append \ n
		ELSE
			dup 0>
			IF
\ POSITIVE EXPONENT - place decimal point in middle
				precision fp.move.decimal
			ELSE
\ NEGATIVE EXPONENT - use 0.000????
				s" 0." fp.append
\ output leading zeros
				negate fp.append.zeros
				fp-represent-pad precision fp.append
			THEN
			fp.strip.trailing.zeros
		THEN
	ELSE
		2drop
		s" <FP-OUT-OF-RANGE>" fp.append
	THEN
	fp-output-pad fp-output-ptr @ over -
;

: FG.  ( F: r -- )
	(fg.) type space
;

: (F.)  ( F: r -- , normal or scientific ) { | n n3 ndiff prec' -- }
	fp-output-pad fp-output-ptr !  \ setup pointer
	fp-represent-pad  \ place to put number
	fdup flog 1 s>f f+ f>s precision max
	fp_precision_max min dup -> prec'
	represent
	( -- n flag1 flag2 )
	IF
\ add '-' sign if negative
		IF [char] - fp.hold
		THEN
\ compare n with precision to see whether we must do scientific display
		dup fp_precision_max >
		IF  \ use exponential notation
			1 precision fp.move.decimal
			fp.strip.trailing.zeros
			[char] e fp.hold
			1- (exp.) fp.append \ n
		ELSE
			dup 0>
			IF
	\ POSITIVE EXPONENT - place decimal point in middle
				prec' fp.move.decimal
			ELSE
	\ NEGATIVE EXPONENT - use 0.000????
				s" 0." fp.append
	\ output leading zeros
				dup negate precision min
				fp.append.zeros
				fp-represent-pad precision rot + fp.append
			THEN
		THEN
	ELSE
		2drop
		s" <FP-OUT-OF-RANGE>" fp.append
	THEN
	fp-output-pad fp-output-ptr @ over -
;

: F.  ( F: r -- )
	(f.) type space
;

: F.S  ( -- , print FP stack )
	." FP> "
	fdepth 0>
	IF
		fdepth 0
		DO
			cr?
			fdepth i - 1-  \ index of next float
			fpick f. cr?
		LOOP
	ELSE
		." empty"
	THEN
	cr
;

\ FP Input ----------------------------------------------------------
variable FP-REQUIRE-E   \ must we put an E in FP numbers?
false fp-require-e !   \ violate ANSI !!

: >FLOAT { c-addr u | dlo dhi u' fsign flag nshift -- flag }
	u 0= IF false exit THEN
	false -> flag
	0 -> nshift
\
\ check for minus sign
	c-addr c@ [char] - =     dup -> fsign
	c-addr c@ [char] + = OR
	IF   1 +-> c-addr   -1 +-> u   \ skip char
	THEN
\
\ convert first set of digits
	0 0 c-addr u >number -> u' -> c-addr -> dhi -> dlo
	u' 0>
	IF
\ convert optional second set of digits
		c-addr c@ [char] . =
		IF
			dlo dhi c-addr 1+ u' 1- dup -> nshift >number
			dup nshift - -> nshift
			-> u' -> c-addr -> dhi -> dlo
		THEN
\ convert exponent
		u' 0>
		IF
			c-addr c@ [char] E =
			c-addr c@ [char] e =  OR
			IF
				1 +-> c-addr   -1 +-> u'   \ skip E char
				u' 0>
				IF
    				c-addr c@ [char] + = \ ignore + on exponent
    				IF
                        1 +-> c-addr   -1 +-> u'   \ skip char
                    THEN
				    c-addr u' ((number?))
				    num_type_single =
				    IF
					   nshift + -> nshift
					   true -> flag
				    THEN
				ELSE
				    true -> flag   \ allow "1E"
				THEN
			THEN
		ELSE
\ only require E field if this variable is true
			fp-require-e @ not -> flag
		THEN
	THEN
\ convert double precision int to float
	flag
	IF
		dlo dhi d>f
		10 s>f nshift s>f f** f*   \ apply exponent
		fsign
		IF
			fnegate
		THEN
	THEN
	flag
;

3 constant NUM_TYPE_FLOAT   \ possible return type for NUMBER?

: (FP.NUMBER?)   ( $addr -- 0 | n 1 | d 2 | r 3 , convert string to number )
\ check to see if it is a valid float, if not use old (NUMBER?)
	dup count >float
	IF
		drop NUM_TYPE_FLOAT
	ELSE
		(number?)
	THEN
;

defer fp.old.number?
variable FP-IF-INIT

: FP.TERM    ( -- , deinstall fp conversion )
	fp-if-init @
	IF
		what's 	fp.old.number? is number?
		fp-if-init off
	THEN
;

: FP.INIT  ( -- , install FP converion )
	fp.term
	what's number? is fp.old.number?
	['] (fp.number?) is number?
	fp-if-init on
	." Floating point numeric conversion installed." cr
;

FP.INIT
if.forgotten fp.term


0 [IF]

23.8e-9 fconstant fsmall
1.0 fsmall f- fconstant falmost1
." Should be 1.0 = " falmost1 f. cr

: TSEGF  ( r -f- , print in all formats )
." --------------------------------" cr
	34 0
	DO
		fdup fs. 4 spaces  fdup fe. 4 spaces
		fdup fg. 4 spaces  fdup f.  cr
		10.0 f/
	LOOP
	fdrop
;

: TFP
	1.234e+22 tsegf
	1.23456789e+22 tsegf
	0.927 fsin 1.234e+22 f* tsegf
;

[THEN]

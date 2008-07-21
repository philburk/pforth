\ @(#) trace.fth 98/01/28 1.2
\ TRACE ( <name> -- , trace pForth word )
\
\ Single step debugger.
\   TRACE  ( i*x <name> -- , setup trace for Forth word )
\   S      ( -- , step over )
\   SM     ( many -- , step over many times )
\   SD     ( -- , step down )
\   G      ( -- , go to end of word )
\   GD     ( n -- , go down N levels from current level, stop at end of this level )
\
\ This debugger works by emulating the inner interpreter of pForth.
\ It executes code and maintains a separate return stack for the
\ program under test.  Thus all primitives that operate on the return
\ stack, such as DO and R> must be trapped.  Local variables must
\ also be handled specially.  Several state variables are also
\ saved and restored to establish the context for the program being
\ tested.
\    
\ Copyright 1997 Phil Burk
\
\ Modifications:
\      19990930 John Providenza - Fixed stack bugs in GD

anew task-trace.fth

: SPACE.TO.COLUMN  ( col -- )
	out @ - spaces
;

: IS.PRIMITIVE? ( xt -- flag , true if kernel primitive )
	['] first_colon <
;

0 value TRACE_IP         \ instruction pointer
0 value TRACE_LEVEL      \ level of descent for inner interpreter
0 value TRACE_LEVEL_MAX  \ maximum level of descent

private{

\ use fake return stack
128 cells constant TRACE_RETURN_SIZE \ size of return stack in bytes
create TRACE-RETURN-STACK TRACE_RETURN_SIZE 16 + allot
variable TRACE-RSP
: TRACE.>R     ( n -- ) trace-rsp @ cell- dup trace-rsp ! ! ;  \ *(--rsp) = n
: TRACE.R>     ( -- n ) trace-rsp @ dup @ swap cell+ trace-rsp ! ;  \ n = *rsp++
: TRACE.R@     ( -- n ) trace-rsp @ @ ; ; \ n = *rsp
: TRACE.RPICK  ( index -- n ) cells trace-rsp @ + @ ; ; \ n = rsp[index]
: TRACE.0RP    ( -- n ) trace-return-stack trace_return_size + 8 + trace-rsp ! ;
: TRACE.RDROP  ( --  ) cell trace-rsp +! ;
: TRACE.RCHECK ( -- , abort if return stack out of range )
	trace-rsp @ trace-return-stack u<
		abort" TRACE return stack OVERFLOW!"
	trace-rsp @ trace-return-stack trace_return_size + 12 + u>
		abort" TRACE return stack UNDERFLOW!"
;

\ save and restore several state variables
10 cells constant TRACE_STATE_SIZE
create TRACE-STATE-1 TRACE_STATE_SIZE allot
create TRACE-STATE-2 TRACE_STATE_SIZE allot

variable TRACE-STATE-PTR
: TRACE.SAVE++ ( addr -- , save next thing )
	@ trace-state-ptr @ !
	cell trace-state-ptr +!
;

: TRACE.SAVE.STATE  ( -- )
	state trace.save++
	hld   trace.save++
	base  trace.save++
;

: TRACE.SAVE.STATE1  ( -- , save normal state )
	trace-state-1 trace-state-ptr !
	trace.save.state
;
: TRACE.SAVE.STATE2  ( -- , save state of word being debugged )
	trace-state-2 trace-state-ptr !
	trace.save.state
;


: TRACE.RESTORE++ ( addr -- , restore next thing )
	trace-state-ptr @ @ swap !
	cell trace-state-ptr +!
;

: TRACE.RESTORE.STATE  ( -- )
	state trace.restore++
	hld   trace.restore++
	base  trace.restore++
;

: TRACE.RESTORE.STATE1  ( -- )
	trace-state-1 trace-state-ptr !
	trace.restore.state
;
: TRACE.RESTORE.STATE2  ( -- )
	trace-state-2 trace-state-ptr !
	trace.restore.state
;

\ The implementation of these pForth primitives is specific to pForth.

variable TRACE-LOCALS-PTR  \ point to top of local frame

\ create a return stack frame for NUM local variables
: TRACE.(LOCAL.ENTRY)  ( x0 x1 ... xn n -- )  { num | lp -- }
	trace-locals-ptr @ trace.>r
	trace-rsp @ trace-locals-ptr !
	trace-rsp @  num cells - trace-rsp !  \ make room for locals
	trace-rsp @ -> lp
	num 0
	DO
		lp !
		cell +-> lp  \ move data into locals frame on return stack
	LOOP
;
	
: TRACE.(LOCAL.EXIT) ( -- )
	trace-locals-ptr @  trace-rsp !
	trace.r> trace-locals-ptr !
;
: TRACE.(LOCAL@) ( l# -- n , fetch from local frame )
	trace-locals-ptr @  swap cells - @
;
: TRACE.(1_LOCAL@) ( -- n ) 1 trace.(local@) ;
: TRACE.(2_LOCAL@) ( -- n ) 2 trace.(local@) ;
: TRACE.(3_LOCAL@) ( -- n ) 3 trace.(local@) ;
: TRACE.(4_LOCAL@) ( -- n ) 4 trace.(local@) ;
: TRACE.(5_LOCAL@) ( -- n ) 5 trace.(local@) ;
: TRACE.(6_LOCAL@) ( -- n ) 6 trace.(local@) ;
: TRACE.(7_LOCAL@) ( -- n ) 7 trace.(local@) ;
: TRACE.(8_LOCAL@) ( -- n ) 8 trace.(local@) ;

: TRACE.(LOCAL!) ( n l# -- , store into local frame )
	trace-locals-ptr @  swap cells - !
;
: TRACE.(1_LOCAL!) ( -- n ) 1 trace.(local!) ;
: TRACE.(2_LOCAL!) ( -- n ) 2 trace.(local!) ;
: TRACE.(3_LOCAL!) ( -- n ) 3 trace.(local!) ;
: TRACE.(4_LOCAL!) ( -- n ) 4 trace.(local!) ;
: TRACE.(5_LOCAL!) ( -- n ) 5 trace.(local!) ;
: TRACE.(6_LOCAL!) ( -- n ) 6 trace.(local!) ;
: TRACE.(7_LOCAL!) ( -- n ) 7 trace.(local!) ;
: TRACE.(8_LOCAL!) ( -- n ) 8 trace.(local!) ;

: TRACE.(LOCAL+!) ( n l# -- , store into local frame )
	trace-locals-ptr @  swap cells - +!
;
: TRACE.(?DO)  { limit start ip -- ip' }
	limit start =
	IF
		ip @ +-> ip \ BRANCH
	ELSE
		start trace.>r
		limit trace.>r
		cell +-> ip
	THEN
	ip
;

: TRACE.(LOOP)  { ip | limit indx -- ip' }
	trace.r> -> limit
	trace.r> 1+ -> indx
	limit indx =
	IF
		cell +-> ip
	ELSE
		indx trace.>r
		limit trace.>r
		ip @ +-> ip
	THEN
	ip
;

: TRACE.(+LOOP)  { delta ip | limit indx oldindx -- ip' }
	trace.r> -> limit
	trace.r> -> oldindx
	oldindx delta + -> indx
\ /* Do indices cross boundary between LIMIT-1 and LIMIT ? */
\  if( ( (OldIndex - Limit) & ((Limit-1) - NewIndex) & 0x80000000 ) ||
\    ( (NewIndex - Limit) & ((Limit-1) - OldIndex) & 0x80000000 ) )
	oldindx limit -    limit 1-    indx -  AND $ 80000000 AND
	   indx limit -    limit 1- oldindx -  AND $ 80000000 AND OR
	IF
		cell +-> ip
	ELSE
		indx trace.>r
		limit trace.>r
		ip @ +-> ip
	THEN
	ip
;

: TRACE.CHECK.IP  {  ip -- }
	ip ['] first_colon u<
	ip here u> OR
	IF
		." TRACE - IP out of range = " ip .hex cr
		abort
	THEN
;

: TRACE.SHOW.IP { ip -- , print name and offset }
	ip code> >name dup id.
	name> >code ip swap - ."  +" .
;

: TRACE.SHOW.STACK { | mdepth -- }
	base @ >r
	." <" base @ decimal 1 .r ." :"
	depth 1 .r ." > "
	r> base !
	depth 5 min -> mdepth
	depth mdepth  -
	IF
		." ... "  \ if we don't show entire stack
	THEN
	mdepth 0
	?DO
		mdepth i 1+ - pick .  \ show numbers in current base
	LOOP
;

: TRACE.SHOW.NEXT { ip -- }
	>newline
	ip trace.check.ip
\ show word name and offset
	." << "
	ip trace.show.ip
	16 space.to.column
\ show data stack
	trace.show.stack
	40 space.to.column ."  ||"
	trace_level 2* spaces
	ip code@
	cell +-> ip
\ show primitive about to be executed
	dup .xt space
\ trap any primitives that are followed by inline data
	CASE
		['] (LITERAL)  OF ip @  . ENDOF
		['] (ALITERAL) OF ip a@ . ENDOF
[ exists? (FLITERAL) [IF] ]
		['] (FLITERAL) OF ip f@ f. ENDOF
[ [THEN] ]
		['] BRANCH     OF ip @  . ENDOF
		['] 0BRANCH    OF ip @  . ENDOF
		['] (.")       OF ip count type .' "' ENDOF
		['] (C")       OF ip count type .' "' ENDOF
		['] (S")       OF ip count type .' "' ENDOF
	ENDCASE
	65 space.to.column ." >> "
;

: TRACE.DO.PRIMITIVE  { ip xt | oldhere --  ip' , perform code at ip }
	xt
	CASE
		0 OF -1 +-> trace_level  trace.r> -> ip ENDOF \ EXIT
		['] (CREATE)   OF ip cell- body_offset + ENDOF
		['] (LITERAL)  OF ip @ cell +-> ip ENDOF
		['] (ALITERAL) OF ip a@ cell +-> ip ENDOF
[ exists? (FLITERAL) [IF] ]
		['] (FLITERAL) OF ip f@ 1 floats +-> ip ENDOF
[ [THEN] ]
		['] BRANCH     OF ip @ +-> ip ENDOF
		['] 0BRANCH    OF 0= IF ip @ +-> ip ELSE cell +-> ip THEN ENDOF
		['] >R         OF trace.>r ENDOF
		['] R>         OF trace.r> ENDOF
		['] R@         OF trace.r@ ENDOF
		['] RDROP      OF trace.rdrop ENDOF
		['] 2>R        OF trace.>r trace.>r ENDOF
		['] 2R>        OF trace.r> trace.r> ENDOF
		['] 2R@        OF trace.r@ 1 trace.rpick ENDOF
		['] i          OF 1 trace.rpick ENDOF
		['] j          OF 3 trace.rpick ENDOF
		['] (LEAVE)    OF trace.rdrop trace.rdrop  ip @ +-> ip ENDOF
		['] (LOOP)     OF ip trace.(loop) -> ip  ENDOF
		['] (+LOOP)    OF ip trace.(+loop) -> ip  ENDOF
		['] (DO)       OF trace.>r trace.>r ENDOF
		['] (?DO)      OF ip trace.(?do) -> ip ENDOF
		['] (.")       OF ip count type  ip count + aligned -> ip ENDOF
		['] (C")       OF ip  ip count + aligned -> ip ENDOF
		['] (S")       OF ip count  ip count + aligned -> ip ENDOF
		['] (LOCAL.ENTRY) OF trace.(local.entry) ENDOF
		['] (LOCAL.EXIT) OF trace.(local.exit) ENDOF
		['] (LOCAL@)   OF trace.(local@)   ENDOF
		['] (1_LOCAL@) OF trace.(1_local@) ENDOF
		['] (2_LOCAL@) OF trace.(2_local@) ENDOF
		['] (3_LOCAL@) OF trace.(3_local@) ENDOF
		['] (4_LOCAL@) OF trace.(4_local@) ENDOF
		['] (5_LOCAL@) OF trace.(5_local@) ENDOF
		['] (6_LOCAL@) OF trace.(6_local@) ENDOF
		['] (7_LOCAL@) OF trace.(7_local@) ENDOF
		['] (8_LOCAL@) OF trace.(8_local@) ENDOF
		['] (LOCAL!)   OF trace.(local!)   ENDOF
		['] (1_LOCAL!) OF trace.(1_local!) ENDOF
		['] (2_LOCAL!) OF trace.(2_local!) ENDOF
		['] (3_LOCAL!) OF trace.(3_local!) ENDOF
		['] (4_LOCAL!) OF trace.(4_local!) ENDOF
		['] (5_LOCAL!) OF trace.(5_local!) ENDOF
		['] (6_LOCAL!) OF trace.(6_local!) ENDOF
		['] (7_LOCAL!) OF trace.(7_local!) ENDOF
		['] (8_LOCAL!) OF trace.(8_local!) ENDOF
		['] (LOCAL+!)  OF trace.(local+!)  ENDOF
		>r xt EXECUTE r>
	ENDCASE
	ip
;

: TRACE.DO.NEXT  { ip | xt oldhere --  ip' , perform code at ip }
	ip trace.check.ip
\ set context for word under test
	trace.save.state1
	here -> oldhere
	trace.restore.state2
	oldhere 256 + dp !
\ get execution token
	ip code@ -> xt
	cell +-> ip
\ execute token
	xt is.primitive?
	IF  \ primitive
		ip xt trace.do.primitive -> ip
	ELSE \ secondary
		trace_level trace_level_max <
		IF
			ip trace.>r         \ threaded execution
			1 +-> trace_level
			xt codebase + -> ip
		ELSE
			\ treat it as a primitive
			ip xt trace.do.primitive -> ip
		THEN		
	THEN
\ restore original context
	trace.rcheck
	trace.save.state2
	trace.restore.state1
	oldhere dp !
	ip
;

: TRACE.NEXT { ip | xt -- ip' }
	trace_level 0>
	IF
		ip trace.do.next -> ip
	THEN
	trace_level 0>
	IF
		ip trace.show.next
	ELSE
		trace-stack on
		." Finished." cr
	THEN
	ip
;

}private

: TRACE ( i*x <name> -- i*x , setup trace environment )
	' dup is.primitive?
	IF
		drop ." Sorry. You can't trace a primitive." cr
	ELSE
		1 -> trace_level
		trace_level -> trace_level_max
		trace.0rp
		>code -> trace_ip
		trace_ip trace.show.next
		trace-stack off
		trace.save.state2
	THEN
;

: s ( -- , step over )
	trace_level -> trace_level_max
	trace_ip trace.next -> trace_ip
;

: sd ( -- , step down )
	trace_level 1+ -> trace_level_max
	trace_ip trace.next -> trace_ip
;

: sm ( many -- , step many times )
	trace_level -> trace_level_max
	0
	?DO
		trace_ip trace.next -> trace_ip
	LOOP
;

defer trace.user   ( IP -- stop?  )
' 0= is trace.user

: gd { more_levels | stop_level -- }
	here   what's trace.user   u<  \ has it been forgotten?
	IF
		." Resetting TRACE.USER !!!" cr
		['] 0= is trace.user
	THEN

	more_levels 0<
	more_levels 10 >
	or	\ 19990930 - OR was missing
	IF
		." GD level out of range (0-10), = " more_levels . cr
	ELSE
		trace_level more_levels + -> trace_level_max
		trace_level 1- -> stop_level
		BEGIN
			trace_ip trace.user \ call deferred user word
			?dup \ leave flag for UNTIL \ 19990930 - was DUP
			IF
				." TRACE.USER returned " dup . ." so stopping execution." cr
			ELSE
				trace_ip trace.next -> trace_ip
				trace_level stop_level > not
			THEN
		UNTIL
	THEN
;

: g ( -- , execute until end of word )
	0 gd
;

: TRACE.HELP ( -- )
	."   TRACE  ( i*x <name> -- , setup trace for Forth word )" cr
	."   S      ( -- , step over )" cr
	."   SM     ( many -- , step over many times )" cr
	."   SD     ( -- , step down )" cr
	."   G      ( -- , go to end of word )" cr
	."   GD     ( n -- , go down N levels from current level," cr
	."                   stop at end of this level )" cr
;

privatize

0 [IF]
variable var1
100 var1 !
: FOO  dup IF 1 + . THEN 77 var1 @ + . ;
: ZOO 29 foo 99 22 + . ;
: ROO 92 >r 1 r@ + . r> . ;
: MOO  c" hello" count type
	." This is a message." cr
	s" another message" type cr
;
: KOO 7 FOO ." DONE" ;
: TR.DO  4 0 DO i . LOOP ;
: TR.?DO  0 ?DO i . LOOP ;
: TR.LOC1 { aa bb } aa bb + . ;
: TR.LOC2 789 >r 4 5 tr.loc1 r> . ;
	
[THEN]

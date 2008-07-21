\ @(#) member.fth 98/01/26 1.2
\ This files, along with c_struct.fth, supports the definition of
\ structure members similar to those used in 'C'.
\
\ Some of this same code is also used by ODE,
\ the Object Development Environment.
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
\ MOD: PLB 1/16/87 Use abort" instead of er.report.
\ MOD: PLB 2/19/87 Made OB.MEMBER immediate, use literal.
\ MOD: PLB/MDH 6/7/88 Use 16 bit values in member defs.
\ MOD: PLB 7/31/88 Add USHORT and UBYTE.
\ MOD: PLB 1/20/89 Treat LITERAL as state sensitive.
\ MOD: RDG 9/19/90 Add floating point member support.
\ MOD: PLB 6/10/91 Add RPTR
\ 00001 PLB 8/3/92 Make RPTR a -4 for S@ and S!
\ 941102 RDG port to pforth
\ 941108 PLB more porting to pforth. Use ?LITERAL instead os smart literal.
\ 960710 PLB align long members for SUN

ANEW TASK-MEMBER.FTH
decimal

: FIND.BODY   ( -- , pfa true | $name false , look for word in dict. )
\ Return address of parameter data.
     32 word find
     IF  >body true
     ELSE false
     THEN
;

\ Variables shared with object oriented code.
    VARIABLE OB-STATE  ( Compilation state. )
    VARIABLE OB-CURRENT-CLASS  ( ABS_CLASS_BASE of current class )
    1 constant OB_DEF_CLASS   ( defining a class )
    2 constant OB_DEF_STRUCT  ( defining a structure )

4 constant OB_OFFSET_SIZE

: OB.OFFSET@ ( member_def -- offset ) @ ;
: OB.OFFSET, ( value -- ) , ;
: OB.SIZE@ ( member_def -- offset )
        ob_offset_size + @ ;
: OB.SIZE, ( value -- ) , ;

( Members are associated with an offset from the base of a structure. )
: OB.MAKE.MEMBER ( +-bytes -- , make room in an object at compile time)
	dup >r  ( -- +-b , save #bytes )
	ABS     ( -- |+-b| )
	ob-current-class @ ( -- b addr-space)
	tuck @          ( as #b c , current space needed )
	over 3 and 0=        ( multiple of four? )
	IF
		aligned
	ELSE
		over 1 and 0=   ( multiple of two? )
		IF
			even-up
		THEN
	THEN
	swap over + rot !    ( update space needed )
\ Save data in member definition. %M
	ob.offset,    ( save old offset for ivar )
	r> ob.size,   ( store size in bytes for ..! and ..@ )
;

\ Unions allow one to address the same memory as different members.
\ Unions work by saving the current offset for members on
\ the stack and then reusing it for different members.
: UNION{  ( -- offset , Start union definition. )
    ob-current-class @ @
;

: }UNION{ ( old-offset -- new-offset , Middle of union )
    union{     ( Get current for }UNION to compare )
    swap ob-current-class @ !  ( Set back to old )
;

: }UNION ( offset -- , Terminate union definition, check lengths. )
    union{ = NOT
    abort" }UNION - Two parts of UNION are not the same size!"
;

\ Make members compile their offset, for "disposable includes".
: OB.MEMBER  ( #bytes -- , make room in an object at compile time)
           ( -- offset , run time for structure )
    CREATE ob.make.member immediate
    DOES> ob.offset@  ( get offset ) ?literal
;

: OB.FINDIT  ( <thing> -- pfa , get pfa of thing or error )
    find.body not
    IF cr count type ."    ???"
       true abort" OB.FINDIT - Word not found!"
    THEN
;

: OB.STATS ( member_pfa --  offset #bytes )
    dup ob.offset@ swap
    ob.size@
;

: OB.STATS? ( <member> -- offset #bytes )
    ob.findit ob.stats
;

: SIZEOF() ( <struct>OR<class> -- #bytes , lookup size of object )
    ob.findit @
    ?literal
; immediate

\ Basic word for defining structure members.
: BYTES ( #bytes -- , error check for structure only )
    ob-state @ ob_def_struct = not
    abort" BYTES - Only valid in :STRUCT definitions."
    ob.member
;

\ Declare various types of structure members.
\ Negative size indicates a signed member.
: BYTE ( <name> -- , declare space for a byte )
    -1 bytes ;

: SHORT ( <name> -- , declare space for a 16 bit value )
    -2 bytes ;

: LONG ( <name> -- )
    cell bytes ;

: UBYTE ( <name> -- , declare space for signed  byte )
    1 bytes ;

: USHORT ( <name> -- , declare space for signed 16 bit value )
    2 bytes ;


\ Aliases
: APTR    ( <name> -- ) long ;
: RPTR    ( <name> -- ) -4 bytes ; \ relative relocatable pointer 00001
: ULONG   ( <name> -- ) long ;

: STRUCT ( <struct> <new_ivar> -- , define a structure as an ivar )
    [compile] sizeof() bytes
;

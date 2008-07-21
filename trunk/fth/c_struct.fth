\ @(#) c_struct.fth 98/01/26 1.2
\ STRUCTUREs are for interfacing with 'C' programs.
\ Structures are created using :STRUCT and ;STRUCT
\
\ This file must be loaded before loading any .J files.
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
\ MOD: PLB 1/16/87 Use abort" instead of er.report
\      MDH 4/14/87 Added sign-extend words to ..@
\ MOD: PLB 9/1/87 Add pointer to last member for debug.
\ MOD: MDH 4/30/88 Use fast addressing for ..@ and ..!
\ MOD: PLB/MDH 9/30/88 Fixed offsets for 16@+long and 8@+long
\        fixed OB.COMPILE.+@/! for 0 offset
\ MOD: PLB 1/11/89 Added EVEN-UP in case of last member BYTE
\ MOD: RDG 9/19/90 Added floating point member support
\ MOD: PLB 12/21/90 Optimized ..@ and ..!
\ 00001 PLB 11/20/91 Make structures IMMEDIATE with ALITERAL for speed
\           Don't need MOVEQ.L  #0,D0 for 16@+WORD and 8@+WORD
\ 00002 PLB 8/3/92 Added S@ and S!, and support for RPTR
\ 951112 PLB Added FS@ and FS!
\ This version for the pForth system.

ANEW TASK-C_STRUCT

decimal
\ STRUCT ======================================================
: <:STRUCT> ( pfa -- , run time action for a structure)
    [COMPILE] CREATE  
        @ even-up here swap dup ( -- here # # )
        allot  ( make room for ivars )
        0 fill  ( initialize to zero )
\		immediate \ 00001
\	DOES> [compile] aliteral \ 00001
;

\ Contents of a structure definition.
\    CELL 0 = size of instantiated structures
\    CELL 1 = #bytes to last member name in dictionary.
\             this is relative so it will work with structure
\             relocation schemes like MODULE

: :STRUCT (  -- , Create a 'C' structure )
\ Check pairs
   ob-state @
   warning" :STRUCT - Previous :STRUCT or :CLASS unfinished!"
   ob_def_struct ob-state !     ( set pair flags )
\
\ Create new struct defining word.
  CREATE
      here ob-current-class !  ( set current )
      0 ,        ( initial ivar offset )
      0 ,        ( location for #byte to last )
   DOES>  <:STRUCT>
;

: ;STRUCT ( -- , terminate structure )
   ob-state @ ob_def_struct = NOT
   abort" ;STRUCT - Missing :STRUCT above!"
   false ob-state !

\ Point to last member.
   latest ob-current-class @ body> >name -  ( byte difference of NFAs )
   ob-current-class @ cell+ !
\
\ Even up byte offset in case last member was BYTE.
   ob-current-class @ dup @ even-up swap !
;

\ Member reference words.
: ..   ( object <member> -- member_address , calc addr of member )
    ob.stats? drop state @
    IF   ?dup
         IF   [compile] literal compile +
         THEN
    ELSE +
    THEN
; immediate


: (S+C!)  ( val addr offset -- )  + c! ;
: (S+W!)  ( val addr  offset -- )  + w! ;
: (S+!)  ( val addr offset -- )  + ! ;
: (S+REL!)  ( ptr addr offset -- )  + >r if.use->rel r> ! ;

: compile+!bytes ( offset size -- )
\	." compile+!bytes ( " over . dup . ." )" cr
	swap [compile] literal   \ compile offset into word
	CASE
	cell OF compile (s+!)  ENDOF
	2 OF compile (s+w!)      ENDOF
	1 OF compile (s+c!)      ENDOF
	-4 OF compile (s+rel!)   ENDOF \ 00002
	-2 OF compile (s+w!)     ENDOF
	-1 OF compile (s+c!)     ENDOF
	true abort" s! - illegal size!"
	ENDCASE
;

: !BYTES ( value address size -- )
    CASE
    cell OF ! ENDOF
	-4 OF ( aptr addr )  swap if.use->rel swap ! ENDOF \ 00002
	ABS
       2 OF w! ENDOF
       1 OF c! ENDOF
       true abort" s! - illegal size!"
    ENDCASE
;

\ These provide ways of setting and reading members values
\ without knowing their size in bytes.
: (S!) ( offset size -- , compile proper fetch )
	state @
    IF  compile+!bytes 
    ELSE ( -- value addr off size )
        >r + r> !bytes
    THEN
;
: S! ( value object <member> -- , store value in member )
    ob.stats?
	(s!)
; immediate

: @BYTES ( addr +/-size -- value )
    CASE
    cell OF @  ENDOF
       2 OF w@      ENDOF
       1 OF c@      ENDOF
      -4 OF @ if.rel->use      ENDOF \ 00002
      -2 OF w@ w->s     ENDOF
      -1 OF c@ b->s     ENDOF
       true abort" s@ - illegal size!"
    ENDCASE
;

: (S+UC@)  ( addr offset -- val )  + c@ ;
: (S+UW@)  ( addr offset -- val )  + w@ ;
: (S+@)  ( addr offset -- val )  + @ ;
: (S+REL@)  ( addr offset -- val )  + @ if.rel->use ;
: (S+C@)  ( addr offset -- val )  + c@ b->s ;
: (S+W@)  ( addr offset -- val )  + w@ w->s ;

: compile+@bytes ( offset size -- )
\	." compile+@bytes ( " over . dup . ." )" cr
	swap [compile] literal   \ compile offset into word
	CASE
	cell OF compile (s+@)  ENDOF
	2 OF compile (s+uw@)      ENDOF
	1 OF compile (s+uc@)      ENDOF
	-4 OF compile (s+rel@)      ENDOF \ 00002
	-2 OF compile (s+w@)     ENDOF
	-1 OF compile (s+c@)     ENDOF
	true abort" s@ - illegal size!"
	ENDCASE
;

: (S@) ( offset size -- , compile proper fetch )
	state @
	IF compile+@bytes
	ELSE >r + r> @bytes
	THEN
;

: S@ ( object <member> -- value , fetch value from member )
    ob.stats?
	(s@)
; immediate



exists? F* [IF]
\ 951112 Floating Point support
: FLPT  ( <name> -- , declare space for a floating point value. )
     1 floats bytes
;
: (S+F!)  ( val addr offset -- )  + f! ;
: (S+F@)  ( addr offset -- val )  + f@ ;

: FS! ( value object <member> -- , fetch value from member )
    ob.stats?
    1 floats <> abort" FS@ with non-float!"
	state @
	IF
		[compile] literal
		compile (s+f!)
	ELSE (s+f!)
	THEN
; immediate
: FS@ ( object <member> -- value , fetch value from member )
    ob.stats?
    1 floats <> abort" FS@ with non-float!"
	state @
	IF
		[compile] literal
		compile (s+f@)
	ELSE (s+f@)
	THEN
; immediate
[THEN]

0 [IF]
:struct mapper
    long map_l1
    long map_l2
    aptr map_a1
    rptr map_r1
    flpt map_f1
    short map_s1
    ushort map_s2
    byte map_b1
    ubyte map_b2
;struct
mapper map1

: TT
	-500 map1 s! map_s1
	map1 s@ map_s1 -500 - abort" map_s1 failed!"
	-500 map1 s! map_s2
	map1 s@ map_s2 -500 $ FFFF and - abort" map_s2 failed!"
	-89 map1 s! map_b1
	map1 s@ map_b1 -89 - abort" map_s1 failed!"
	here map1 s! map_r1
	map1 s@ map_r1 here - abort" map_r1 failed!"
	-89 map1 s! map_b2
	map1 s@ map_b2 -89 $ FF and - abort" map_s2 failed!"
	23.45 map1 fs! map_f1
	map1 fs@ map_f1 f. ." =?= 23.45" cr
;
." Testing c_struct.fth" cr
TT
[THEN]

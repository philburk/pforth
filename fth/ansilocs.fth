\ @(#) ansilocs.fth 98/01/26 1.3
\ local variable support words
\ These support the ANSI standard (LOCAL) and TO words.
\
\ They are built from the following low level primitives written in 'C':
\    (local@) ( i+1 -- n , fetch from ith local variable )
\    (local!) ( n i+1 -- , store to ith local variable )
\    (local.entry) ( num -- , allocate stack frame for num local variables )
\    (local.exit)  ( -- , free local variable stack frame )
\    local-compiler ( -- addr , variable containing CFA of locals compiler )
\
\ Author: Phil Burk
\ Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
\
\ Permission to use, copy, modify, and/or distribute this
\ software for any purpose with or without fee is hereby granted.
\
\ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
\ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
\ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
\ THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
\ CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
\ FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
\ CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
\ OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
\
\ 10/27/99 Fixed  : foo { -- } 55 ; was entering local frame but not exiting.

anew task-ansilocs.fth

private{

decimal
16 constant LV_MAX_VARS    \ maximum number of local variables
mask_name_size constant LV_MAX_CHARS   \ maximum number of letters in name

lv_max_vars lv_max_chars $array LV-NAMES
variable LV-#NAMES   \ number of names currently defined

\ Search name table for match
: LV.MATCH ( $string -- index true | $string false )
    0 swap
    lv-#names @ 0
    ?DO  i lv-names
        over $=
        IF  2drop true i LEAVE
        THEN
    LOOP swap
;

: LV.COMPILE.FETCH  ( index -- )
    1+  \ adjust for optimised (local@), LocalsPtr points above vars
    CASE
    1 OF compile (1_local@) ENDOF
    2 OF compile (2_local@) ENDOF
    3 OF compile (3_local@) ENDOF
    4 OF compile (4_local@) ENDOF
    5 OF compile (5_local@) ENDOF
    6 OF compile (6_local@) ENDOF
    7 OF compile (7_local@) ENDOF
    8 OF compile (8_local@) ENDOF
    dup [compile] literal compile (local@)
    ENDCASE
;

: LV.COMPILE.STORE  ( index -- )
    1+  \ adjust for optimised (local!), LocalsPtr points above vars
    CASE
    1 OF compile (1_local!) ENDOF
    2 OF compile (2_local!) ENDOF
    3 OF compile (3_local!) ENDOF
    4 OF compile (4_local!) ENDOF
    5 OF compile (5_local!) ENDOF
    6 OF compile (6_local!) ENDOF
    7 OF compile (7_local!) ENDOF
    8 OF compile (8_local!) ENDOF
    dup [compile] literal compile (local!)
    ENDCASE
;

: LV.COMPILE.LOCAL  ( $name -- handled? , check for matching locals name )
\ ." LV.COMPILER.LOCAL name = " dup count type cr
    lv.match
    IF ( index )
        lv.compile.fetch
        true
    ELSE
        drop false
    THEN
;

: LV.CLEANUP ( -- , restore stack frame on exit from colon def )
    lv-#names @
    IF
        compile (local.exit)
    THEN
;
: LV.FINISH ( -- , restore stack frame on exit from colon def )
    lv.cleanup
    lv-#names off
    local-compiler off
;

: LV.SETUP ( -- )
    0 lv-#names !
;

: LV.TERM
    ." Locals turned off" cr
    lv-#names off
    local-compiler off
;

if.forgotten lv.term

}private

: (LOCAL)  ( adr len -- , ANSI local primitive )
    dup
    IF
        lv-#names @ lv_max_vars >= abort" Too many local variables!"
        lv-#names @  lv-names place
\ Warn programmer if local variable matches an existing dictionary name.
        lv-#names @  lv-names find nip
        IF
            ." (LOCAL) - Note: "
            lv-#names @  lv-names count type
            ."  redefined as a local variable in "
            latest id. cr
        THEN
        1 lv-#names +!
    ELSE
\ Last local. Finish building local stack frame.
        2drop
        lv-#names @ dup 0=  \ fixed 10/27/99, Thanks to John Providenza
        IF
            drop ." (LOCAL) - Warning: no locals defined!" cr
        ELSE
            [compile] literal   compile (local.entry)
            ['] lv.compile.local local-compiler !
        THEN
    THEN
;

: VALUE
    CREATE ( n <name> )
        ,
    DOES>
        @
;

: TO  ( val <name> -- )
    bl word
    lv.match
    IF  ( -- index )
        lv.compile.store
    ELSE
        find
        0= abort" not found"
        >body  \ point to data
        state @
        IF  \ compiling  ( -- pfa )
            [compile] aliteral
            compile !
        ELSE \ executing  ( -- val pfa )
            !
        THEN
    THEN
; immediate

: ->  ( -- )  [compile] to  ; immediate

: +->  ( val <name> -- )
    bl word
    lv.match
    IF  ( -- index )
        1+  \ adjust for optimised (local!), LocalsPtr points above vars
        [compile] literal compile (local+!)
    ELSE
        find
        0= abort" not found"
        >body  \ point to data
        state @
        IF  \ compiling  ( -- pfa )
            [compile] aliteral
            compile +!
        ELSE \ executing  ( -- val pfa )
            +!
        THEN
    THEN
; immediate

: :      lv.setup   : ;
: ;      lv.finish  [compile] ;      ; immediate
: exit   lv.cleanup  compile exit   ; immediate
: does>  lv.finish  [compile] does>  ; immediate

privatize

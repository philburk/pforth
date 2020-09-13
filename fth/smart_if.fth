\ @(#) smart_if.fth 98/01/26 1.2
\ Smart Conditionals
\ Allow use of if, do, begin, etc.outside of colon definitions.
\
\ Thanks to Mitch Bradley for the idea.
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

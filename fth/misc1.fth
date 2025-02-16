\ @(#) misc1.fth 98/01/26 1.2
\ miscellaneous words
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

anew task-misc1.fth
decimal

: >> rshift ;
: << lshift ;

: (WARNING")  ( flag $message -- )
    swap
    IF count type
    ELSE drop
    THEN
;

: WARNING" ( flag <message> -- , print warning if true. )
    [compile] "  ( compile message )
    state @
    IF  compile (warning")
    ELSE (warning")
    THEN
; IMMEDIATE

: (ABORT")  ( flag $message -- )
    swap
    IF
        count type cr
        err_abortq throw
    ELSE drop
    THEN
;

: ABORT" ( flag <message> -- , print warning if true. )
    [compile] "  ( compile message )
    state @
    IF  compile (abort")
    ELSE (abort")
    THEN
; IMMEDIATE


: ?PAUSE ( -- , Pause if key hit. )
    ?terminal
    IF  key drop cr ." Hit space to continue, any other key to abort:"
        key dup emit BL = not abort" Terminated"
    THEN
;

60 constant #cols

: CR?  ( -- , do CR if near end )
    OUT @ #cols 16 - 10 max >
    IF cr
    THEN
;

: $ ( <number> -- N , convert next number as hex )
    base @ hex
    bl lword number? num_type_single = not
    abort" Not a single number!"
    swap base !
    state @
    IF [compile] literal
    THEN
; immediate

: .HX   ( nibble -- )
    dup 9 >
    IF    $ 37
    ELSE  $ 30
    THEN  + emit
;

variable TAB-WIDTH  8 TAB-WIDTH !
: TAB  ( -- , tab over to next stop )
    out @ tab-width @ mod
    tab-width @   swap - spaces
;

\ Vocabulary listing
: WORDS  ( -- )
    0 latest
    BEGIN  dup 0<>
    WHILE ( -- count NFA )
        dup c@ flag_smudge and 0=
        IF
            dup id. tab cr? ?pause
            swap 1+ swap
        THEN
        prevname
    REPEAT drop
    cr . ."  words" cr
;

: VLIST words ;

variable CLOSEST-NFA
variable CLOSEST-XT

: >NAME  ( xt -- nfa , scans dictionary for closest nfa, SLOW! )
    0 closest-nfa !
    0 closest-xt !
    latest
    BEGIN  dup 0<>
        IF ( -- addr nfa ) 2dup name> ( addr nfa addr xt ) <
            IF true  ( addr below this cfa, can't be it)
            ELSE ( -- addr nfa )
                2dup name>  ( addr nfa addr xt ) =
                IF ( found it ! ) dup closest-nfa ! false
                ELSE dup name> closest-xt @ >
                    IF dup closest-nfa ! dup name> closest-xt !
                    THEN
                    true
                THEN
            THEN
        ELSE false
        THEN
    WHILE
        prevname
    REPEAT ( -- cfa nfa )
    2drop
    closest-nfa @
;

: @EXECUTE  ( addr -- , execute if non-zero )
    x@ ?dup
    IF execute
    THEN
;

: TOLOWER ( char -- char_lower )
    dup ascii [ <
    IF  dup ascii @ >
        IF ascii A - ascii a +
        THEN
    THEN
;

: EVALUATE ( i*x c-addr num -- j*x , evaluate string of Forth )
\ save current input state and switch to passed in string
    source >r >r
    set-source
    -1 push-source-id
    >in @ >r
    0 >in !
\ interpret the string
    interpret
\ restore input state
    pop-source-id drop
    r> >in !
    r> r> set-source
;

: \S ( -- , comment out rest of file )
    source-id
    IF
        BEGIN \ using REFILL is safer than popping SOURCE-ID
            refill 0=
        UNTIL
    THEN
;

: UNRAVEL  ( -- , show names of words on return stack )
    >newline ." Calling sequence:" cr
    r0 rp@ - cell /
    1-     \ skip call into unravel
    0 max   50 min  \ clip to reasonable range
    0
    ?DO  4 spaces
        rp@ i 2+   \ skip over DO LOOP control and call to UNRAVEL
        cell* + @
        dup code> >name ?dup
        IF id. drop
        ELSE .
        THEN cr?
    LOOP cr
;

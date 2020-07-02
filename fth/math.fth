\ @(#) math.fth 98/01/26 1.2
\ Extended Math routines
\ FM/MOD SM/REM
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

anew task-math.fth
decimal

: FM/MOD { dl dh nn | dlp dhp nnp rem quo -- rem quo , floored }
    dl dh dabs -> dhp -> dlp
    nn abs -> nnp
    dlp dhp nnp um/mod -> quo -> rem
    dh 0<
    IF  \ negative dividend
        nn 0<
        IF   \ negative divisor
            rem negate -> rem
        ELSE  \ positive divisor
            rem 0=
            IF
                quo negate -> quo
            ELSE
                quo 1+ negate -> quo
                nnp rem - -> rem
            THEN
        THEN
    ELSE  \ positive dividend
        nn 0<
        IF  \ negative divisor
            rem 0=
            IF
                quo negate -> quo
            ELSE
                nnp rem - negate -> rem
                quo 1+ negate -> quo
            THEN
        THEN
    THEN
    rem quo
;

: SM/REM { dl dh nn | dlp dhp nnp rem quo -- rem quo , symmetric }
    dl dh dabs -> dhp -> dlp
    nn abs -> nnp
    dlp dhp nnp um/mod -> quo -> rem
    dh 0<
    IF  \ negative dividend
        rem negate -> rem
        nn 0>
        IF   \ positive divisor
            quo negate -> quo
        THEN
    ELSE  \ positive dividend
        nn 0<
        IF  \ negative divisor
            quo negate -> quo
        THEN
    THEN
    rem quo
;


: /MOD ( a b -- rem quo )
    >r s>d r> sm/rem
;

: MOD ( a b -- rem )
    /mod drop
;

: */MOD ( a b c -- rem a*b/c , use double precision intermediate value )
    >r m*
    r> sm/rem
;
: */ ( a b c -- a*b/c , use double precision intermediate value )
    */mod
    nip
;

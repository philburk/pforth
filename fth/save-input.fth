\ SAVE-INPUT and RESTORE-INPUT
\
\ This code is part of pForth.
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

anew task-save-input.fth

private{

: SAVE-BUFFER ( -- column source-id 2 ) >in @ source-id 2 ;

\ Restore >IN from COLUMN unless COLUMN is too large.  Valid values
\ for COLUMN are from 0 to (including) the length of SOURCE plus one.
: RESTORE-COLUMN ( column -- flag )
    source nip 1+ over u<
    IF   drop  true
    ELSE >in ! false
    THEN
;

\ Return the file-position of the beginning of the current line in
\ file SOURCE-ID.  Assume that the current line is stored in SOURCE
\ and that the current file-position is at an end-of-line (or
\ end-of-file).
: LINE-START-POSITION ( -- ud )
    source-id file-position throw
    \ unless at end-of-file, subtract newline
    source-id file-size throw 2over d= 0= IF 1 s>d d- THEN
    \ subtract line length
    source nip s>d d-
;

: SAVE-FILE ( column line filepos:ud source-id 5 -- )
    >in @
    source-line-number@
    line-start-position
    source-id
    5
;

: RESTORE-FILE ( column line filepos:ud -- flag )
    source-id reposition-file  IF 2drop true EXIT THEN
    refill                     0= IF 2drop true EXIT THEN
    source-line-number!
    restore-column
;

: NDROP ( n*x n -- ) 0 ?DO drop LOOP ;

}private

\ Source      Stack
\ EVALUATE    >IN  SourceID=(-1)  2
\ keyboard    >IN  SourceID=(0)   2
\ file        >IN  lineNumber filePos  SourceID=(fileID) 5
: SAVE-INPUT ( -- column {line filepos}? source-id n )
    source-id CASE
	-1 OF save-buffer ENDOF
	0  OF save-buffer ENDOF
	drop save-file EXIT
    ENDCASE
;

: RESTORE-INPUT ( column {line filepos}? source-id n -- flag )
    over source-id <> IF ndrop true EXIT THEN
    drop
    CASE
	-1 OF restore-column ENDOF
	0  OF restore-column ENDOF
	drop restore-file EXIT
    ENDCASE
;

privatize

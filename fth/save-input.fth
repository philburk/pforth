\ SAVE-INPUT and RESTORE-INPUT

anew task-save-input.fth

private{

: save-buffer ( -- column source-id 2 ) >in @ source-id 2 ;

: restore-column ( column -- flag )
    source nip over <
    IF   drop  true
    ELSE >in ! false
    THEN
;

\ Return the file-position of the beginning of the current line in
\ file SOURCE-ID.  Assume that the current line is stored in SOURCE
\ and that the current file-position is at an end-of-line (or
\ end-of-file).
: line-start-position ( -- ud )
    source-id file-position throw
    \ unless at end-of-file, subtract newline
    source-id file-size throw 2over d= 0= IF 1 s>d d- THEN
    \ subtract line length
    source nip s>d d-
;

: save-file ( column line filepos:ud source-id 5 -- )
    >in @
    source-line-number@
    line-start-position
    source-id
    5
;

: restore-file ( column line filepos:ud -- flag )
    source-id reposition-file  IF 2drop true exit THEN
    refill                     0= IF 2drop true exit THEN
    source-line-number!
    restore-column
;

: ndrop ( n*x n -- ) 0 ?do drop loop ;

}private

\ Source      Stack
\ EVALUATE    >IN  SourceID=(-1)  2
\ keyboard    >IN  SourceID=(0)   2
\ file        >IN  lineNumber filePos  SourceID=(fileID) 5
: SAVE-INPUT ( -- column {line filepos}? source-id n )
    source-id case
	-1 of save-buffer endof
	0  of save-buffer endof
	drop save-file exit
    endcase
;

: RESTORE-INPUT ( column {line filepos}? source-id n -- flag )
    over source-id <> IF ndrop true exit THEN
    drop
    case
	-1 of restore-column endof
	0  of restore-column endof
	drop restore-file exit
    endcase
;

privatize

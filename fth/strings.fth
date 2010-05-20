\ @(#) strings.fth 98/01/26 1.2
\ String support for PForth
\
\ Copyright Phil Burk 1994

ANEW TASK-STRINGS.FTH

: -TRAILING  ( c-addr u1 -- c-addr u2 , strip trailing blanks )
	dup 0>
	IF
		BEGIN
			2dup 1- chars + c@ bl =
			over 0> and
		WHILE
			1-
		REPEAT
	THEN
;

\ Structure of string table
: $ARRAY  (  )
    CREATE  ( #strings #chars_max --  ) 
        dup ,
        2+ * even-up allot
    DOES>    ( index -- $addr )
        dup @  ( get #chars )
        rot * + cell+
;

\ Compare two strings
: $= ( $1 $2 -- flag , true if equal )
    -1 -rot
    dup c@ 1+ 0
    DO  dup c@ tolower
        2 pick c@ tolower -
        IF rot drop 0 -rot LEAVE
        THEN
		1+ swap 1+ swap
    LOOP 2drop
;

: TEXT=  ( addr1 addr2 count -- flag )
    >r -1 -rot
	r> 0
    DO  dup c@ tolower
        2 pick c@ tolower -
        IF rot drop 0 -rot LEAVE
        THEN
		1+ swap 1+ swap
    LOOP 2drop
;

: TEXT=?  ( addr1 count addr2 -- flag , for JForth compatibility )
	swap text=
;

: $MATCH?  ( $string1 $string2 -- flag , case INsensitive )
	dup c@ 1+ text=
;


: INDEX ( $string char -- false | address_char true , search for char in string )
    >r >r 0 r> r>
    over c@ 1+ 1
    DO  over i + c@ over =
        IF  rot drop
            over i + rot rot LEAVE
        THEN
    LOOP 2drop
    ?dup 0= 0=
;


: $APPEND.CHAR  ( $string char -- ) \ ugly stack diagram
    over count chars + c!
    dup c@ 1+ swap c!
;

\ ----------------------------------------------
: ($ROM)  ( index address -- $string )
    ( -- index address )
    swap 0
    DO dup c@ 1+ + aligned
    LOOP
;

: $ROM ( packed array of strings, unalterable )
    CREATE ( <name> -- )
    DOES> ( index -- $string )  ($rom)
;

: TEXTROM ( packed array of strings, unalterable )
    CREATE ( <name> -- )
    DOES> ( index -- address count )  ($rom) count
;

\ -----------------------------------------------

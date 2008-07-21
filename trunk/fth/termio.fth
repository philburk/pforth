\ Terminal I/O
\
\ Requires an ANSI compatible terminal.
\
\ To get Windows computers to use ANSI mode in their DOS windows,
\ Add this line to "C:\CONFIG.SYS" then reboot.
\  
\  device=c:\windows\command\ansi.sys
\
\ Author: Phil Burk
\ Copyright 1988 Phil Burk
\ Revised 2001 for pForth

ANEW TASK-TERMIO.FTH
decimal

$ 08 constant ASCII_BACKSPACE
$ 7F constant ASCII_DELETE
$ 1B constant ASCII_ESCAPE
$ 01 constant ASCII_CTRL_A
$ 05 constant ASCII_CTRL_E
$ 18 constant ASCII_CTRL_X

\ ANSI Terminal Control
: ESC[ ( send ESCAPE and [ )
	ASCII_ESCAPE emit
	ascii [ emit
;

: CLS ( -- , clear screen )
	ESC[ ." 2J"
;

: TIO.BACKWARDS ( n -- , move cursor backwards )
	ESC[
	base @ >r decimal
	0 .r
	r> base !
	ascii D emit
;

: TIO.FORWARDS ( n -- , move cursor forwards )
	ESC[
	base @ >r decimal
	0 .r
	r> base !
	ascii C emit
;

: TIO.ERASE.EOL ( -- , erase to the end of the line )
	ESC[
	ascii K emit
;


: BELL ( -- , ring the terminal bell )
	7 emit
;

: BACKSPACE ( -- , backspace action )
	8 emit  space  8 emit
;

0 [IF] \ for testing

: SHOWKEYS  ( -- , show keys pressed in hex )
	BEGIN
		key
		dup .
		." , $ " dup .hex cr
		ascii q =
	UNTIL
;

: AZ ascii z 1+ ascii a DO i emit LOOP ;

: TEST.BACK1
	AZ 5 tio.backwards
	1000 msec
	tio.erase.eol
;
: TEST.BACK2
	AZ 10 tio.backwards
	1000 msec
	." 12345"
	1000 msec
;
[THEN]

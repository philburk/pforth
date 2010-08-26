\ Command Line History
\
\ Author: Phil Burk
\ Copyright 1988 Phil Burk
\ Revised 2001 for pForth

0 [IF]

Requires an ANSI compatible terminal.

To get Windows computers to use ANSI mode in their DOS windows,
Add this line to "C:\CONFIG.SYS" then reboot.
  
  device=c:\windows\command\ansi.sys

When command line history is on, you can use the UP and DOWN arrow to scroll
through previous commands. Use the LEFT and RIGHT arrows to edit within a line.
   CONTROL-A moves to beginning of line.
   CONTROL-E moves to end of line.
   CONTROL-X erases entire line.


HISTORY#       ( -- , dump history buffer with numbers)
HISTORY        ( -- , dump history buffer )
XX             ( line# -- , execute line x of history )
HISTORY.RESET  ( -- , clear history tables )
HISTORY.ON     ( -- , install history vectors )
HISTORY.OFF    ( -- , uninstall history vectors )

[THEN]

include? ESC[ termio.fth

ANEW TASK-HISTORY.FTH
decimal

private{

\ You can expand the history buffer by increasing this constant!!!!!!!!!!
2048 constant KH_HISTORY_SIZE

create KH-HISTORY kh_history_size allot
KH-HISTORY kh_history_size erase

\ An entry in the history buffer consists of
\   byte  - Count byte = N,
\   chars - N chars,
\   short -  line number in Big Endian format,
\   byte  - another Count byte = N, for reverse scan
\
\ The most recent entry is put at the beginning,
\ older entries are shifted up.

4 constant KH_LINE_EXTRA_SIZE ( 2 count bytes plus 2 size bytes )

: KH-END ( -- addr , end of history buffer )
	kh-history kh_history_size +
;

: LINENUM@ ( addr -- w , stores in BigEndian format )
	dup c@ 8 shift
	swap 1+ c@ or
;

: LINENUM! ( w addr -- )
	over -8 shift over c!
	1+ c!
;

variable KH-LOOK      ( cursor offset into history, point to 1st count byte of line )
variable KH-MAX
variable KH-COUNTER       ( 16 bit counter for line # )
variable KH-SPAN          ( total number of characters in line )
variable KH-MATCH-SPAN    ( span for matching on shift-up )
variable KH-CURSOR        ( points to next insertion point )
variable KH-ADDRESS       ( address to store chars )
variable KH-INSIDE        ( true if we are scrolling inside the history buffer )

: KH.MAKE.ROOM ( N -- , make room for N more bytes at beginning)
	>r  ( save N )
	kh-history dup r@ + ( source dest )
	kh_history_size r> - 0 max move
;

: KH.NEWEST.LINE  ( -- addr count , most recent line )
	kh-history count
;

: KH.REWIND ( -- , move cursor to most recent line )
	0 kh-look !
;

: KH.CURRENT.ADDR ( -- $addr , count byte of current line )
	kh-look @ kh-history +
;

: KH.CURRENT.LINE ( -- addr count )
	kh.current.addr count
;

: KH.COMPARE ( addr count -- flag , true if redundant )
	kh.newest.line compare 0=   \ note: ANSI COMPARE is different than JForth days
;

: KH.NUM.ADDR ( -- addr , address of current line's line count )
	kh.current.line +
;

: KH.CURRENT.NUM ( -- # , number of current line )
	kh.num.addr LINENUM@
;

: KH.ADDR++  ( $addr -- $addr' , convert one kh to previous )
	count + 3 +
;
: KH.ADDR--  ( $addr -- $addr' , convert one kh to next )
	dup 1- c@   \ get next lines endcount
	4 +	 \ account for lineNum and two count bytes
	-       \ calc previous address
;

: KH.ENDCOUNT.ADDR ( -- addr , address of current end count )
	kh.num.addr 2+
;

: KH.ADD.LINE ( addr count -- )
	dup 256 >
	IF ." KH.ADD.LINE - Too big for history!" 2drop
	ELSE   ( add to end )
\ Compare with most recent line.
		2dup kh.compare
		IF 2drop
		ELSE
			>r ( save count )
\ Set look pointer to point to first count byte of last string.
			0 kh-look !
\ Make room for this line of text and line header. 
\ PLB20100823 Was cell+ which broke on 64-bit code.
			r@ KH_LINE_EXTRA_SIZE + kh.make.room
\ Set count bytes at beginning and end.
			r@ kh-history c!  ( start count )
			r@ kh.endcount.addr c!
			kh-counter @ kh.num.addr LINENUM!  ( line )
\ Number lines modulo 1024
			kh-counter @ 1+ $ 3FF and kh-counter !
			kh-history 1+   ( calc destination )
			r> cmove  ( copy chars into space )
		THEN
	THEN
;

: KH.BACKUP.LINE  { | cantmove addr' -- cantmove , advance KH-LOOK if in bounds }
	true -> cantmove ( default flag, at end of history )
\ KH-LOOK points to count at start of current line
	kh.current.addr c@       \ do we have any lines?
	IF
		kh.current.addr kh.addr++ -> addr'
		addr' kh-end U<      \ within bounds?
		IF  
			addr' c@     \ older line has chars?
			IF
				addr' kh-history - kh-look !
				false -> cantmove
			THEN
		THEN
	THEN
	cantmove
;

: KH.FORWARD.LINE ( -- cantmove? )
    kh-look @ 0= dup not
    IF  kh.current.addr kh.addr--
	kh-history - kh-look !
    THEN
;

: KH.OLDEST.LINE   ( -- addr count | 0, oldest in buffer )
    BEGIN kh.backup.line
    UNTIL
    kh.current.line dup 0=
    IF
    	nip
    THEN
;

: KH.FIND.LINE ( line# -- $addr )
	kh.rewind
    BEGIN kh.current.num over -
    WHILE kh.backup.line
        IF ." Line not in History Buffer!" cr drop 0 exit
        THEN
    REPEAT
    drop kh.current.addr
;


: KH-BUFFER ( -- buffer )
    kh-address @
;

: KH.RETURN ( -- , move to beginning of line )
    0 out !
    13 emit
;

: KH.REPLACE.LINE  ( addr count -- , make this the current line of input )
    kh.return
    tio.erase.eol
    dup kh-span !
    dup kh-cursor !
    2dup kh-buffer swap cmove
    type
;

: KH.GET.MATCH ( -- , search for line with same start )
    kh-match-span @ 0=  ( keep length for multiple matches )
    IF kh-span @ kh-match-span !
    THEN
    BEGIN
    	kh.backup.line not
    WHILE
    	kh.current.line drop
    	kh-buffer kh-match-span @ text=
        IF kh.current.line kh.replace.line
           exit
        THEN
    REPEAT
;

: KH.FAR.RIGHT
    kh-span @ kh-cursor @ - dup 0>
    IF
    	tio.forwards
        kh-span @ kh-cursor !
    ELSE drop
    THEN
;

: KH.FAR.LEFT ( -- )
    kh.return
    kh-cursor off
;

: KH.GET.OLDER ( -- , goto previous line )
	kh-inside @
	IF kh.backup.line drop
	THEN
	kh.current.line kh.replace.line
	kh-inside on
;

: KH.GET.NEWER ( -- , next line )
	kh.forward.line
	IF
		kh-inside off
		tib 0
	ELSE  kh.current.line
	THEN
	kh.replace.line
;

: KH.CLEAR.LINE ( -- , rewind history scrolling and clear line )
	kh.rewind
	tib 0 kh.replace.line
	kh-inside off
;

: KH.GO.RIGHT  ( -- )
    kh-cursor @ kh-span @ <
    IF 1 kh-cursor +!
       1 tio.forwards
    THEN
;

: KH.GO.LEFT ( -- )
    kh-cursor @ ?dup
    IF 1- kh-cursor !
       1 tio.backwards
    THEN
;

: KH.REFRESH  ( -- , redraw current line as is )
	kh.return
	kh-buffer kh-span @ type
	tio.erase.eol
	
	kh.return
	kh-cursor @ ?dup 
	IF tio.forwards
	THEN
	
	kh-span @ out !
;

: KH.BACKSPACE ( -- , backspace character from buffer and screen )
    kh-cursor @ ?dup  ( past 0? )
    IF  kh-span @ <
        IF  ( inside line )
            kh-buffer kh-cursor @ +  ( -- source )
            dup 1- ( -- source dest )
            kh-span @ kh-cursor @ - cmove
\            ." Deleted!" cr 
        ELSE
            backspace
        THEN
        -1 kh-span +!
        -1 kh-cursor +!
    ELSE bell
    THEN
    kh.refresh
;

: KH.DELETE ( -- , forward delete )
    kh-cursor @ kh-span @ <  ( before end )
    IF  ( inside line )
        kh-buffer kh-cursor @ + 1+ ( -- source )
        dup 1- ( -- source dest )
        kh-span @ kh-cursor @ - 0 max cmove
        -1 kh-span +!
        kh.refresh
    THEN
;
    
: KH.HANDLE.WINDOWS.KEY ( char -- , handle fkeys or arrows used by Windows ANSI.SYS )
	CASE
		$ 8D OF kh.get.match    ENDOF
			0 kh-match-span ! ( reset if any other key )
		$ 48 OF kh.get.older    ENDOF
		$ 50 OF kh.get.newer  ENDOF
		$ 4D OF kh.go.right ENDOF
		$ 4B OF kh.go.left  ENDOF
		$ 91 OF kh.clear.line  ENDOF
		$ 74 OF kh.far.right ENDOF
		$ 73 OF kh.far.left  ENDOF
		$ 53 OF kh.delete  ENDOF
	ENDCASE
;

: KH.HANDLE.ANSI.KEY ( char -- , handle fkeys or arrows used by ANSI terminal )
	CASE
		$ 41 OF kh.get.older    ENDOF
		$ 42 OF kh.get.newer  ENDOF
		$ 43 OF kh.go.right ENDOF
		$ 44 OF kh.go.left  ENDOF
	ENDCASE
;


: KH.SPECIAL.KEY ( char  -- true | false , handle fkeys or arrows, true if handled )
	true >r
	CASE
	
	$ E0 OF key kh.handle.windows.key
	ENDOF
	
	ASCII_ESCAPE OF
		key dup $ 4F = \ for TELNET
		$ 5B = OR \ for regular ANSI terminals
		IF
			key kh.handle.ansi.key
		ELSE
			rdrop false >r
		THEN
	ENDOF
	
        ASCII_BACKSPACE OF kh.backspace ENDOF
        ASCII_DELETE    OF kh.backspace ENDOF
        ASCII_CTRL_X    OF kh.clear.line ENDOF
        ASCII_CTRL_A    OF kh.far.left ENDOF
        ASCII_CTRL_E    OF kh.far.right ENDOF
	
	rdrop false >r
	
	ENDCASE
	r>
;
		
: KH.SMART.KEY ( -- char )
    BEGIN
    	key dup kh.special.key
    WHILE
    	drop
    REPEAT
;
        
: KH.INSCHAR  { charc | repaint -- }
	false -> repaint
	kh-cursor @ kh-span @ <
	IF 
\ Move characters up
		kh-buffer kh-cursor @ +  ( -- source )
		dup 1+ ( -- source dest )
		kh-span @ kh-cursor @ - cmove>
		true -> repaint
	THEN
\ write character to buffer
	charc kh-buffer kh-cursor @ + c!
	1 kh-cursor +!
	1 kh-span +!
	repaint
	IF kh.refresh
	ELSE charc emit
	THEN
;

: EOL? ( char -- flag , true if an end of line character )
	dup 13 =
	swap 10 = OR
;

: KH.GETLINE ( max -- )
	kh-max !
	kh-span off
	kh-cursor off
	kh-inside off
	kh.rewind
	0 kh-match-span !
	BEGIN
		kh-max @ kh-span @ >
		IF  kh.smart.key
			dup EOL? not  ( <cr?> )
		ELSE 0 false
		THEN  ( -- char flag )
	WHILE ( -- char )
		kh.inschar
	REPEAT drop
	kh-span @ kh-cursor @ - ?dup
	IF tio.forwards  ( move to end of line )
	THEN
	space
	flushemit
;

: KH.ACCEPT ( addr max -- numChars )
	swap kh-address !
	kh.getline
	kh-span @ 0>
	IF kh-buffer kh-span @ kh.add.line
	THEN
	kh-span @
;

: TEST.HISTORY
	4 0 DO
		pad 128 kh.accept
		cr pad swap type cr
	LOOP
;

}private


: HISTORY# ( -- , dump history buffer with numbers)
	cr kh.oldest.line ?dup
	IF
		BEGIN kh.current.num 3 .r ." ) " type ?pause cr
			kh.forward.line 0=
		WHILE kh.current.line
		REPEAT
	THEN
;

: HISTORY ( -- , dump history buffer )
	cr kh.oldest.line ?dup
	IF
		BEGIN type ?pause cr
			kh.forward.line 0=
		WHILE kh.current.line
		REPEAT
	THEN
;

: XX  ( line# -- , execute line x of history )
	kh.find.line ?dup
	IF count evaluate
	THEN
;


: HISTORY.RESET  ( -- , clear history tables )
	kh-history kh_history_size erase
	kh-counter off
;

: HISTORY.ON ( -- , install history vectors )
	history.reset
	what's accept ['] (accept) =
	IF ['] kh.accept is accept
	THEN
;

: HISTORY.OFF ( -- , uninstall history vectors )
	what's accept ['] kh.accept =
	IF ['] (accept) is accept
	THEN
;


: AUTO.INIT
	auto.init
	history.on
;
: AUTO.TERM
	history.off
	auto.init
;

if.forgotten history.off

0 [IF]
history.reset
history.on
[THEN]

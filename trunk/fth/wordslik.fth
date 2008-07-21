\ @(#) wordslik.fth 98/01/26 1.2
\
\ WORDS.LIKE  ( <string> -- , search for words that contain string )
\
\ Enter:   WORDS.LIKE +
\ Enter:   WORDS.LIKE EMIT
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

anew task-wordslik.fth
decimal


: PARTIAL.MATCH.NAME  ( $str1 nfa  -- flag , is $str1 in nfa ??? )
	count $ 1F and
	rot count
	search
	>r 2drop r>
;

: WORDS.LIKE  ( <name> -- , print all words containing substring )
	BL word latest
	>newline
	BEGIN
		prevname dup 0<> \ get previous name in dictionary
	WHILE
		2dup partial.match.name
		IF
			dup id. tab
			cr?
		THEN
	REPEAT 2drop
	>newline
;

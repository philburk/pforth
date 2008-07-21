\ @(#) make_all256.fth 97/12/10 1.1
\ Make a file with all possible 256 bytes in random order.
\
\ Author: Phil Burk
\ Copyright 1987 Phil Burk
\ All Rights Reserved.

ANEW TASK-MAKE_ALL256

variable RAND8-SEED
19 rand8-seed !
: RANDOM8 ( -- r8 , generate random bytes, repeat every 256 )
	RAND8-SEED @
	77 * 55 +
	$ FF and
	dup RAND8-SEED !
;

create rand8-pad 256 allot
: make.256.data
	256 0
	DO
		random8 rand8-pad i + c!
	LOOP
;

: SHUFFLE.DATA { num | ind1 ind2 -- }
	num 0
	DO
		256 choose -> ind1
		256 choose -> ind2
		ind1 rand8-pad + c@
		ind2 rand8-pad + c@
		ind1 rand8-pad + c!
		ind2 rand8-pad + c!
	LOOP
;
	
: WRITE.256.FILE   { | fid -- }
	p" all256.raw" count r/w create-file
	IF
		drop ." Could not create file." cr
	ELSE
		-> fid
		fid . cr
		rand8-pad 256 fid write-file abort" write failed!"
		fid close-file drop
	THEN
;

: MAKE.256.FILE
	make.256.data
	1000 shuffle.data
	write.256.file
;

MAKE.256.FILE

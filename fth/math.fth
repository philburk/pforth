\ @(#) math.fth 98/01/26 1.2
\ Extended Math routines
\ FM/MOD SM/REM
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

\ @(#) bench.fth 97/12/10 1.1
\ Benchmark Forth
\ by Phil Burk
\ 11/17/95
\
\ pForthV9 on Indy, compiled with gcc
\  bench1  took 15 seconds
\  bench2  took 16 seconds
\  bench3  took 17 seconds
\  bench4  took 17 seconds
\  bench5  took 19 seconds
\  sieve   took  4 seconds
\
\ Darren Gibbs reports that on an SGI Octane loaded with multiple users:
\  bench1  took 2.8sec
\  bench2  took 2.7
\  bench3  took 2.9
\  bench4  took 2.1
\  bench 5 took 2.5
\  seive   took .6
\
\ HForth on Mac Quadra 800, 68040
\  bench1  took 1.73 seconds
\  bench2  took 6.48 seconds
\  bench3  took 2.65 seconds
\  bench4  took 2.50 seconds
\  bench5  took 1.91 seconds
\  sieve   took 0.45 seconds
\
\ pForthV9 on Mac Quadra 800
\  bench1  took 40 seconds
\  bench2  took 43 seconds
\  bench3  took 43 seconds
\  bench4  took 44 seconds
\  bench5  took 42 seconds
\  sieve   took 20 seconds
\
\ pForthV9 on PB5300, 100 MHz PPC 603 based Mac Powerbook
\  bench1  took 8.6 seconds
\  bench2  took 9.0 seconds
\  bench3  took 9.7 seconds
\  bench4  took 8.8 seconds
\  bench5  took 10.3 seconds
\  sieve   took 2.3 seconds
\
\ HForth on PB5300
\  bench1  took 1.1 seconds
\  bench2  took 3.6 seconds
\  bench3  took 1.7 seconds
\  bench4  took 1.2 seconds
\  bench5  took 1.3 seconds
\  sieve   took 0.2 seconds

anew task-bench.fth

decimal

\ benchmark primitives
create #do 2000000   ,

: t1           #do @ 0      do                     loop ;
: t2  23 45    #do @ 0      do  swap               loop   2drop ;
: t3  23       #do @ 0      do  dup drop           loop drop ;
: t4  23 45    #do @ 0      do  over drop          loop 2drop ;
: t5           #do @ 0      do  23 45 + drop       loop ;
: t6  23       #do @ 0      do  >r r>              loop drop ;
: t7  23 45 67 #do @ 0      do  rot                loop 2drop drop ;
: t8           #do @ 0      do  23 2* drop         loop  ;
: t9           #do @ 10 / 0 do  23 5 /mod 2drop    loop ;
: t10     #do  #do @ 0      do  dup @ drop         loop drop ;

: foo ( noop ) ;
: t11          #do @ 0      do  foo                loop ;

\ more complex benchmarks -----------------------

\ BENCH1 - sum data ---------------------------------------
create data1 23 , 45 , 67 , 89 , 111 , 222 , 333 , 444 ,
: sum.cells ( addr num -- sum )
	0 swap \ sum
	0 DO
		over \ get address
		i cells + @ +
	LOOP
	swap drop
;

: bench1 ( -- )
	200000 0
	DO
		data1 8 sum.cells drop
	LOOP
;

\ BENCH2 - recursive factorial --------------------------
: factorial ( n -- n! )
	dup 1 >
	IF
		dup 1- recurse *
	ELSE
		drop 1
	THEN
;

: bench2 ( -- )
	200000 0
	DO
		10 factorial drop
	LOOP
;

\ BENCH3 - DEFER ----------------------------------
defer calc.answer
: answer ( n -- m )
	dup +
	$ a5a5 xor
	1000 max
;
' answer is calc.answer
: bench3
	1500000 0
	DO
		i calc.answer drop
	LOOP
;
	
\ BENCH4 - locals ---------------------------------
: use.locals { x1 x2 | aa bb -- result }
	x1 2* -> aa
	x2 2/ -> bb
	x1 aa *
	x2 bb * +
;

: bench4
	400000 0
	DO
		234 567 use.locals drop
	LOOP
;

\ BENCH5 - string compare -------------------------------
: match.strings { $s1 $s2 | adr1 len1 adr2 len2 -- flag }
	$s1 count -> len1 -> adr1
	$s2 count -> len2 -> adr2
	len1 len2 -
	IF
		FALSE
	ELSE
		TRUE
		len1 0
		DO
			adr1 i + c@
			adr2 i + c@ -
			IF
				drop FALSE
				leave
			THEN
		LOOP
	THEN
;

: bench5 ( -- )
	60000 0
	DO
		" This is a string. X foo"
		" This is a string. Y foo" match.strings drop
	LOOP
;

\ SIEVE OF ERATOSTHENES from BYTE magazine -----------------------

DECIMAL 8190 CONSTANT TSIZE

VARIABLE FLAGS TSIZE ALLOT

: <SIEVE>  ( --- #primes )  FLAGS TSIZE 1 FILL
 0  TSIZE 0
 DO   ( n )  I FLAGS + C@
      IF    I  DUP +  3 +   DUP I +  (  I2*+3 I3*+3 )
           BEGIN  DUP TSIZE <  ( same flag )
           WHILE  0 OVER FLAGS + C! (  i' i'' )   OVER +
           REPEAT 2DROP  1+
      THEN
 LOOP       ;

: SIEVE  ." 10 iterations " CR  0   10 0 
  DO     <SIEVE> swap drop 
  LOOP   . ." primes " CR ;

: SIEVE50  ." 50 iterations " CR  0   50 0 
  DO     <SIEVE> swap drop 
  LOOP   . ." primes " CR ;

\ 10 iterations
\ 21.5 sec  Amiga Multi-Forth  Indirect Threaded
\ 8.82 sec  Amiga 1000 running JForth
\ ~5 sec  SGI Indy running pForthV9

anew task-tut.fth

: SUM.OF.N.A ( N -- SUM[N] , calculate sum of N integers )
           0  \ starting value of SUM
           BEGIN
               OVER 0>   \ Is N greater than zero?
           WHILE
               OVER +  \ add N to sum
               SWAP 1- SWAP  \ decrement N
           REPEAT
           SWAP DROP  \ get rid on N
       ;

: SUM.OF.N.B  ( N -- SUM[N] )
    0 SWAP  \ starting value of SUM
    1+ 0    \ set indices for DO LOOP
    ?DO     \ safer than DO if N=0
        I +
    LOOP
;

: SUM.OF.N.C  ( N -- SUM[N] )
    0  \ starting value of SUM
    BEGIN   ( -- N' SUM )
    	OVER +
    	SWAP 1- SWAP
    	OVER 0<
    UNTIL
    SWAP DROP
;

: SUM.OF.N.D  ( N -- SUM[N] )
	>R  \ put NUM on return stack
    0  \ starting value of SUM
    BEGIN   ( -- SUM )
    	R@ +  \ add num to sum
    	R> 1- DUP >R
    	0<
    UNTIL
    RDROP  \ get rid of NUM
;

: SUM.OF.N.E  { NUM | SUM -- SUM[N] , use return stack }
    BEGIN  
    	NUM +-> SUM \ add NUM to SUM
    	-1 +-> NUM  \ decrement NUM
    	NUM 0<
    UNTIL
    SUM  \ return SUM
;

: SUM.OF.N.F  ( NUM -- SUM[N] , Gauss' method )
    DUP 1+ * 2/
;


: TTT
	10 0
	DO
		I SUM.OF.N.A .
		I SUM.OF.N.B .
		I SUM.OF.N.C .
		I SUM.OF.N.D .
		I SUM.OF.N.E .
		I SUM.OF.N.F .
		CR
	LOOP
;
TTT


\ test CASE
anew test-case
: TCASE  ( N -- )
	CASE
	0 OF ." is zero" ENDOF
	1 OF
		2 choose
		CASE
		0 OF ." chose zero" ENDOF
		1 OF ." chose one" ENDOF
		[ .s cr ." of-depth = " of-depth @ . cr ]
		ENDCASE
	ENDOF
	[ .s cr ." of-depth = " of-depth @ . cr ]
	ENDCASE
;

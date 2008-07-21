\ @(#) t_floats.fth 98/02/26 1.1 17:46:04
\ Test ANS Forth FLOAT words.
\
\ Copyright 1994 3DO, Phil Burk

INCLUDE? }T{  t_tools.fth

ANEW TASK-T_FLOATS.FTH

DECIMAL
3.14159265 fconstant PI

TEST{
\ ==========================================================
T{ 1 2 3 }T{ 1 2 3 }T
\  ----------------------------------------------------- D>F F>D
\ test some basic floating point <> integer conversion
T{   4  0 D>F F>D  }T{   4  0 }T
T{ 835  0 D>F F>D  }T{ 835  0 }T
T{ -57 -1 D>F F>D  }T{ -57 -1 }T
T{ 15 S>F 2 S>F F/ F>S }T{ 7 }T  \ 15.0/2.0 -> 7.5

\  ----------------------------------------------------- input
T{ 79.2 F>S }T{ 79 }T
T{ 0.003 F>S }T{ 0 }T

\ ------------------------------------------------------ F~
T{  23.4  23.5  0.2   f~ }T{  true  }T
T{  23.4  23.7  0.2   f~ }T{  false }T
T{ 922.3 922.3  0.0   f~ }T{  true  }T
T{ 922.3 922.31 0.0   f~ }T{  false }T
T{   0.0   0.0  0.0   f~ }T{  true  }T
T{   0.0  -0.0  0.0   f~ }T{  false }T
T{  50.0  51.0 -0.02  f~ }T{  true  }T
T{  50.0  51.0 -0.002 f~ }T{  false }T
T{ 500.0 510.0 -0.02  f~ }T{  true  }T
T{ 500.0 510.0 -0.002 f~ }T{  false }T

\ convert number to text representation and then back to float
: T_F. ( -- ok? ) ( r ftol -f- )
	fover (f.) >float fswap f~
	AND
;
: T_FS. ( -- ok? ) ( r -f- )
	fover (fs.) >float fswap f~
	AND
;
: T_FE. ( -- ok? ) ( r -f- )
	fover (fe.) >float fswap f~
	AND
;

: T_FG. ( -- ok? ) ( r -f- )
	fover (f.) >float fswap f~
	AND
;

: T_F>D ( -- ok? ) ( r -f- )
	fover f>d d>f fswap f~
;

T{ 0.0  0.00001 T_F.  }T{  true  }T
T{ 0.0  0.00001 T_FS.  }T{  true  }T
T{ 0.0  0.00001 T_FE.  }T{  true  }T
T{ 0.0  0.00001 T_FG.  }T{  true  }T
T{ 0.0  0.00001 T_F>D  }T{  true  }T

T{ 12.34  -0.0001 T_F.  }T{  true  }T
T{ 12.34  -0.0001 T_FS.  }T{  true  }T
T{ 12.34  -0.0001 T_FE.  }T{  true  }T
T{ 12.34  -0.0001 T_FG.  }T{  true  }T
T{ 1234.0  -0.0001 T_F>D  }T{  true  }T

T{ 2345 S>F  79 S>F  F/  -0.0001 T_F.  }T{  true  }T
T{ 511 S>F  -294 S>F  F/  -0.0001 T_F.  }T{  true  }T

: T.SERIES { N matchCFA | flag -- ok? } (  fstart fmult -f- )
	fswap  ( -- fmust fstart )
	true -> flag
	N 0
	?DO
		fdup -0.0001 matchCFA execute not
		IF
			false -> flag
			." T_F_SERIES failed for " i . fdup f. cr
			leave
		THEN
\		i . fdup f. cr
		fover f*
	LOOP
	matchCFA >name id. ."  T.SERIES final = " fs. cr
	flag
;

: T.SERIES_F.    ['] t_f.  t.series ;
: T.SERIES_FS.   ['] t_fs. t.series ;
: T.SERIES_FG.   ['] t_fg. t.series ;
: T.SERIES_FE.   ['] t_fe. t.series ;
: T.SERIES_F>D   ['] t_f>d t.series ;

T{  1.0     1.3       150 t.series_f.    }T{  true  }T
T{  1.0    -1.3       150 t.series_f.    }T{  true  }T
T{  2.3456789 1.3719  150 t.series_f.    }T{  true  }T

T{  3000.0  1.298     120 t.series_f>d   }T{  true  }T

T{  1.2     1.27751   150 t.series_fs.   }T{  true  }T
T{  7.43    0.812255  200 t.series_fs.   }T{  true  }T

T{  1.195   1.30071   150 t.series_fe.   }T{  true  }T
T{  5.913   0.80644   200 t.series_fe.   }T{  true  }T

T{  1.395   1.55071   120 t.series_fe.   }T{  true  }T
T{  5.413   0.83644   160 t.series_fe.   }T{  true  }T

\  ----------------------------------------------------- FABS
T{  0.0   FABS  0.0         0.00001 F~    }T{  true  }T
T{  7.0   FABS  7.0         0.00001 F~    }T{  true  }T
T{ -47.3  FABS  47.3        0.00001 F~    }T{  true  }T

\  ----------------------------------------------------- FSQRT
T{  49.0  FSQRT  7.0       -0.0001 F~    }T{  true  }T
T{  2.0   FSQRT  1.414214  -0.0001 F~    }T{  true  }T

\  ----------------------------------------------------- FSIN
T{  0.0   FSIN  0.0         0.00001 F~    }T{  true  }T
T{  PI    FSIN  0.0         0.00001 F~    }T{  true  }T
T{  PI 2.0 F*  FSIN   0.0   0.00001 F~    }T{  true  }T
T{  PI 0.5 F*  FSIN   1.0   0.00001 F~    }T{  true  }T
T{  PI 6.0 F/  FSIN   0.5   0.00001 F~    }T{  true  }T

\  ----------------------------------------------------- \
}TEST


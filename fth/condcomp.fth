\ @(#) condcomp.fth 98/01/26 1.2
\ Conditional Compilation support
\
\ Words: STRINGS= [IF] [ELSE] [THEN] EXISTS?
\
\ Lifted from X3J14 dpANS-6 document.

anew task-condcomp.fth

: [ELSE]  ( -- )
    1
    BEGIN                                 \ level
      BEGIN
        BL WORD                           \ level $word
        COUNT  DUP                        \ level adr len len
      WHILE                               \ level adr len
        2DUP  S" [IF]"  COMPARE 0=
        IF                                \ level adr len
          2DROP 1+                        \ level'
        ELSE                              \ level adr len
          2DUP  S" [ELSE]"
          COMPARE 0=                      \ level adr len flag
          IF                              \ level adr len
             2DROP 1- DUP IF 1+ THEN      \ level'
          ELSE                            \ level adr len
            S" [THEN]"  COMPARE 0=
            IF
              1-                          \ level'
            THEN
          THEN
        THEN
        ?DUP 0=  IF EXIT THEN             \ level'
      REPEAT  2DROP                       \ level
    REFILL 0= UNTIL                       \ level
    DROP
;  IMMEDIATE

: [IF]  ( flag -- )
	0=
	IF POSTPONE [ELSE]
	THEN
;  IMMEDIATE

: [THEN]  ( -- )
;  IMMEDIATE

: EXISTS? ( <name> -- flag , true if defined )
    bl word find
    swap drop
; immediate

\ @(#) t_alloc.fth 97/01/28 1.4
\ Test PForth ALLOCATE
\
\ Copyright 1994 3DO, Phil Burk

anew task-t_alloc.fth
decimal

64 constant NUM_TAF_SLOTS

variable TAF-MAX-ALLOC
variable TAF-MAX-SLOT

\ hold addresses and sizes
NUM_TAF_SLOTS array TAF-ADDRESSES
NUM_TAF_SLOTS array TAF-SIZES

: TAF.MAX.ALLOC? { | numb addr ior maxb -- max }
        0 -> maxb
\ determine maximum amount we can allocate
        1024 40 * -> numb
        BEGIN
                numb 0>
        WHILE
                numb allocate -> ior -> addr
                ior 0=
                IF  \ success
                        addr free abort" Free failed!"
                        numb -> maxb
                        0 -> numb
                ELSE
                        numb 1024 - -> numb
                THEN
        REPEAT
        maxb
;

: TAF.INIT  ( -- )
        NUM_TAF_SLOTS 0
        DO
                0 i taf-addresses !
        LOOP
\
        taf.max.alloc? ." Total Avail = " dup . cr
        dup taf-max-alloc !
        NUM_TAF_SLOTS / taf-max-slot !
;

: TAF.ALLOC.SLOT { slotnum | addr size -- }
\ allocate some RAM
        taf-max-slot @ 8 -
        choose 8 + 
        dup allocate abort" Allocation failed!"
        -> addr
        -> size
        addr slotnum taf-addresses !
        size slotnum taf-sizes !
\
\ paint RAM with slot number
        addr size slotnum fill
;

: TAF.FREE.SLOT { slotnum | addr size -- }
        slotnum taf-addresses @  -> addr
\ something allocated so check it and free it.
        slotnum taf-sizes @  0
        DO
                addr i + c@  slotnum -
                IF
                        ." Error at " addr i + .
                        ." , slot# " slotnum . cr
                        abort
                THEN
        LOOP
        addr free abort" Free failed!"
        0 slotnum taf-addresses !
;

: TAF.DO.SLOT { slotnum  -- }
        slotnum taf-addresses @ 0=
        IF
                slotnum taf.alloc.slot
        ELSE
                slotnum taf.free.slot
        THEN
;

: TAF.TERM
        NUM_TAF_SLOTS 0
        DO
                i taf-addresses @
                IF
                        i taf.free.slot
                THEN
        LOOP
\
        taf.max.alloc? dup ." Final    MAX = " . cr
        ." Original MAX = " taf-max-alloc @ dup . cr
        = IF ." Test PASSED." ELSE ." Test FAILED!" THEN cr
        
;

: TAF.TEST ( NumTests -- )
        1 max
        dup . ." tests" cr \ flushemit
        taf.init
        ." Please wait for test to complete..." cr
        0
        DO  NUM_TAF_SLOTS choose taf.do.slot
        LOOP
        taf.term
;

.( Testing ALLOCATE and FREE) cr
10000 taf.test


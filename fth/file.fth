\ READ-LINE and WRITE-LINE
\
\ This code is part of pForth.
\
\ The pForth software code is dedicated to the public domain,
\ and any third party may reproduce, distribute and modify
\ the pForth software code or any derivative works thereof
\ without any compensation or license.  The pForth software
\ code is provided on an "as is" basis without any warranty
\ of any kind, including, without limitation, the implied
\ warranties of merchantability and fitness for a particular
\ purpose and their equivalents under the laws of any jurisdiction.

private{

10 constant \N
13 constant \R

\ Unread one char from file FILEID.
: UNREAD { fileid -- ior }
    fileid file-position          ( ud ior )
    ?dup
    IF   nip nip \ IO error
    ELSE 1 s>d d- fileid reposition-file
    THEN
;

\ Read the next available char from file FILEID and if it is a \n then
\ skip it; otherwise unread it.  IOR is non-zero if an error occured.
\ C-ADDR is a buffer that can hold at least one char.
: SKIP-\N { c-addr fileid -- ior }
    c-addr 1 fileid read-file     ( u ior )
    ?dup
    IF \ Read error?
        nip
    ELSE                          ( u )
        0=
        IF \ End of file?
            0
        ELSE
            c-addr c@ \n =        ( is-it-a-\n? )
            IF   0
            ELSE fileid unread
            THEN
        THEN
    THEN
;

\ This is just s\" \n" but s\" isn't yet available.
create (LINE-TERMINATOR) \n c,
: LINE-TERMINATOR ( -- c-addr u ) (line-terminator) 1 ;

-72 constant THROW_RENAME_FILE

}private

\ This treats \n, \r\n, and \r as line terminator.  Reading is done
\ one char at a time with READ-FILE hence READ-FILE should probably do
\ some form of buffering for good efficiency.
: READ-LINE ( c-addr u1 fileid -- u2 flag ior )
    { a u f }
    u 0 ?DO
        a i chars + 1 f read-file                                  ( u ior' )
        ?dup IF nip i false rot UNLOOP EXIT THEN \ Read error?     ( u )
        0= IF i i 0<> 0 UNLOOP EXIT THEN         \ End of file?    ( )
        a i chars + c@
        CASE
            \n OF i true 0 UNLOOP EXIT ENDOF
            \r OF
                \ Detect \r\n
                a i chars + f skip-\n                              ( ior )
                ?dup IF i false rot UNLOOP EXIT THEN \ IO Error?   ( )
                i true 0 UNLOOP EXIT
	    ENDOF
        ENDCASE
    LOOP
    \ Line doesn't fit in buffer
    u true 0
;

: WRITE-LINE ( c-addr u fileid -- ior )
    { f }
    f write-file                  ( ior )
    ?dup
    IF \ IO error
    ELSE line-terminator f write-file
    THEN
;

: RENAME-FILE ( c-addr1 u1 c-addr2 u2 -- ior )
    { a1 u1 a2 u2 | new }
    \ Convert the file-names to C-strings by copying them after HERE
    \ with trailing zeros added.
    a1 here u1 move
    0 here u1 chars + c!
    here u1 1+ chars + to new
    a2 new u2 move
    0 new u2 chars + c!
    here new (rename-file) 0=
    IF 0
    ELSE throw_rename_file
    THEN
;

privatize

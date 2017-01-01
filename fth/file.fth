\ READ-LINE and WRITE-LINE
\
\ This file is in the public domain.
\

private{

10 constant \n
13 constant \r

\ Unread one char from file FILEID.
: UNREAD ( fileid -- ior )
    { f }
    f file-position          ( ud ior )
    ?dup
    IF   nip nip \ IO error
    ELSE 1 s>d d- f reposition-file
    THEN
;

\ Read the next available char from file FILEID and if it is a \n then
\ skip it; otherwise unread it.  IOR is non-zero if an error occured.
\ C-ADDR is a buffer that can hold at least on char.
: SKIP-\n ( c-addr fileid -- ior )
  { a f }
  a 1 f read-file               ( u ior )
  ?dup
  IF \ Read error?
      nip
  ELSE                          ( u )
      0=
      IF \ End of file?
          0
      ELSE
          a c@ \n =             ( is-it-a-\n? )
          IF   0
          ELSE f unread
          THEN
      THEN
  THEN
;

\ This is just s\" \n" but s\" isn't yet available.
create (LINE-TERMINATOR) \n c,
: LINE-TERMINATOR ( -- c-addr u ) (line-terminator) 1 ;

}private


\ This treats \n, \r\n, and \r as line terminator.  Reading is done
\ one char at a time with READ-FILE hence READ-FILE should probably do
\ some form of buffering for good efficiency.
: READ-LINE ( c-addr u1 fileid -- u2 flag ior )
  { a u f }
  u 0 ?DO
      a i chars + 1 f read-file                                  ( u ior' )
      ?dup IF nip i false rot UNLOOP EXIT THEN \ Read error?     ( u )
      0= IF i i 0> 0 UNLOOP EXIT THEN          \ End of file?    ( )
      a i chars + c@
      CASE
          \n OF i true 0 UNLOOP EXIT ENDOF
          \r OF
              \ Detect \r\n
              a i 1+ chars + f skip-\n                           ( ior )
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

privatize

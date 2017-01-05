\ REQUIRE and REQUIRED
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

\ Has the file with name C-ADDR/U already been included?
\
\ This searches the "::::<filename>" marker created by INCLUDED.  This
\ works for now, but may break if pForth ever receives wordlists.
: INCLUDED? ( c-addr u -- flag )
    s" ::::" here place         ( c-addr u )
    here $append                ( )
    here find nip 0<>           ( found? )
;

\ FIXME: use real PARSE-NAME when available
: (PARSE-NAME) ( "word" -- c-addr u ) bl parse-word ;

}private

: REQUIRED ( i*x c-addr u -- j*x ) 2dup included? IF 2drop ELSE included THEN ;
: REQUIRE ( i*x "name" -- i*x ) (parse-name) required ;

privatize

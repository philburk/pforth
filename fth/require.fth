\ REQUIRE and REQUIRED
\
\ This code is part of pForth.
\
\ Permission to use, copy, modify, and/or distribute this
\ software for any purpose with or without fee is hereby granted.
\
\ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
\ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
\ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
\ THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
\ CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
\ FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
\ CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
\ OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

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

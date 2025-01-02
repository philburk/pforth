\ @(#) wordslik.fth 98/01/26 1.2
\
\ WORDS.LIKE  ( <string> -- , search for words that contain string )
\
\ Enter:   WORDS.LIKE +
\ Enter:   WORDS.LIKE EMIT
\
\ Author: Phil Burk
\ Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
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

anew task-wordslik.fth
decimal


: PARTIAL.MATCH.NAME  ( $str1 nfa  -- flag , is $str1 in nfa ??? )
    count mask_name_size and
    rot count
    search
    >r 2drop r>
;

: WORDS.LIKE  ( <name> -- , print all words containing substring )
    BL word latest
    >newline
    BEGIN
        prevname dup 0<> \ get previous name in dictionary
    WHILE
        2dup partial.match.name
        IF
            dup id. tab
            cr?
        THEN
    REPEAT 2drop
    >newline
;

\ Load history and save new dictionary.
\ This is not part of the standard build because some computers
\ do not support ANSI terminal I/O.

include? ESC[ termio.fth
include? HISTORY history.fth
c" pforth.dic" save-forth

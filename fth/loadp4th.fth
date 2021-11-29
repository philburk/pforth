\ @(#) loadp4th.fth 98/01/28 1.3
\ Load various files needed by PForth
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

include? forget  forget.fth
include? >number numberio.fth
include? task-misc1.fth   misc1.fth
include? case    case.fth
include? $=      strings.fth
include? privatize   private.fth
include? (local) ansilocs.fth
include? {       locals.fth
include? fm/mod  math.fth
include? task-misc2.fth misc2.fth
include? [if]    condcomp.fth
include? save-input save-input.fth
include? read-line  file.fth
include? require    require.fth
include? s\"     slashqt.fth

\ load floating point support if basic support is in kernel
exists? F*
   [IF]  include? task-floats.fth floats.fth
   [THEN]

\ useful but optional stuff follows --------------------

include? task-member.fth   member.fth
include? :struct c_struct.fth
include? smif{   smart_if.fth
include? file?   filefind.fth
include? see     see.fth
include? words.like wordslik.fth
include? trace   trace.fth
include? ESC[    termio.fth
include? HISTORY history.fth

map

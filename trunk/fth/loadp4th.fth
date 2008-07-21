\ @(#) loadp4th.fth 98/01/28 1.3
\ Load various files needed by PForth
\
\ Author: Phil Burk
\ Copyright 1994 3DO, Phil Burk, Larry Polansky, Devid Rosenboom
\
\ The pForth software code is dedicated to the public domain,
\ and any third party may reproduce, distribute and modify
\ the pForth software code or any derivative works thereof
\ without any compensation or license.  The pForth software
\ code is provided on an "as is" basis without any warranty
\ of any kind, including, without limitation, the implied
\ warranties of merchantability and fitness for a particular
\ purpose and their equivalents under the laws of any jurisdiction.

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
include? SDAD    savedicd.fth

map

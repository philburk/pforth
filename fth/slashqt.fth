\ S\" implementation for pForth
\
\ Copied from ANS reference implementation at:
\    http://www.forth200x.org/escaped-strings.html
\
\ The code was not modified except for the use of private{ }private
\
\ Added November 2021 by Phil Burk

ANEW TASK-SLASHQT.FTH

private{

decimal

: c+!           \ c c-addr --
\ *G Add character C to the contents of address C-ADDR.
  tuck c@ + swap c!
;

: addchar       \ char string --
\ *G Add the character to the end of the counted string.
  tuck count + c!
  1 swap c+!
;

: append        \ c-addr u $dest --
\ *G Add the string described by C-ADDR U to the counted string at
\ ** $DEST. The strings must not overlap.
  >r
  tuck  r@ count +  swap cmove          \ add source to end
  r> c+!                                \ add length to count
;

: extract2H	\ c-addr len -- c-addr' len' u
\ *G Extract a two-digit hex number in the given base from the
\ ** start of the string, returning the remaining string
\ ** and the converted number.
  base @ >r  hex
  0 0 2over drop 2 >number 2drop drop
  >r  2 /string  r>
  r> base !
;

create EscapeTable      \ -- addr
\ *G Table of translations for \a..\z.
        7 c,	\ \a BEL (Alert)
        8 c,	\ \b BS  (Backspace)
   char c c,    \ \c
   char d c,    \ \d
       27 c,	\ \e ESC (Escape)
       12 c,	\ \f FF  (Form feed)
   char g c,    \ \g
   char h c,    \ \h
   char i c,    \ \i
   char j c,    \ \j
   char k c,    \ \k
       10 c,	\ \l LF  (Line feed)
   char m c,    \ \m
       10 c,    \ \n (Unices only)
   char o c,    \ \o
   char p c,    \ \p
   char " c,    \ \q "   (Double quote)
       13 c,	\ \r CR  (Carriage Return)
   char s c,    \ \s
        9 c,	\ \t HT  (horizontal tab}
   char u c,    \ \u
       11 c,	\ \v VT  (vertical tab)
   char w c,    \ \w
   char x c,    \ \x
   char y c,    \ \y
        0 c,	\ \z NUL (no character)

create CRLF$    \ -- addr ; CR/LF as counted string
  2 c,  13 c,  10 c,

: addEscape	\ c-addr len dest -- c-addr' len'
\ *G Add an escape sequence to the counted string at dest,
\ ** returning the remaining string.
  over 0=                               \ zero length check
  if  drop  exit  then
  >r                                    \ -- caddr len ; R: -- dest
  over c@ [char] x = if                 \ hex number?
    1 /string extract2H r> addchar  exit
  then
  over c@ [char] m = if                 \ CR/LF pair
    1 /string  13 r@ addchar  10 r> addchar  exit
  then
  over c@ [char] n = if                 \ CR/LF pair? (Windows/DOS only)
    1 /string  crlf$ count r> append  exit
  then
  over c@ [char] a [char] z 1+ within if
    over c@ [char] a - EscapeTable + c@  r> addchar
  else
    over c@ r> addchar
  then
  1 /string
;

: parse\"	\ c-addr len dest -- c-addr' len'
\ *G Parses a string up to an unescaped '"', translating '\'
\ ** escapes to characters. The translated string is a
\ ** counted string at *\i{dest}.
\ ** The supported escapes (case sensitive) are:
\ *D \a      BEL          (alert)
\ *D \b      BS           (backspace)
\ *D \e      ESC (not in C99)
\ *D \f      FF           (form feed)
\ *D \l      LF (ASCII 10)
\ *D \m      CR/LF pair - for HTML etc.
\ *D \n      newline - CRLF for Windows/DOS, LF for Unices
\ *D \q      double-quote
\ *D \r      CR (ASCII 13)
\ *D \t      HT (tab)
\ *D \v      VT
\ *D \z      NUL (ASCII 0)
\ *D \"      double-quote
\ *D \xAB    Two char Hex numerical character value
\ *D \\      backslash itself
\ *D \       before any other character represents that character
  dup >r  0 swap c!                     \ zero destination
  begin                                 \ -- caddr len ; R: -- dest
    dup
   while
    over c@ [char] " <>                 \ check for terminator
   while
    over c@ [char] \ = if               \ deal with escapes
      1 /string r@ addEscape
    else                                \ normal character
      over c@ r@ addchar  1 /string
    then
  repeat then
  dup                                   \ step over terminating "
  if 1 /string  then
  r> drop
;

create pocket  \ -- addr
\ *G A tempory buffer to hold processed string.
\    This would normally be an internal system buffer.

s" /COUNTED-STRING" environment? 0= [if] 256 [then]
1 chars + allot

: readEscaped	\ "ccc" -- c-addr
\ *G Parses an escaped string from the input stream according to
\ ** the rules of *\fo{parse\"} above, returning the address
\ ** of the translated counted string in *\fo{POCKET}.
  source >in @ /string tuck             \ -- len caddr len
  pocket parse\" nip
  - >in +!
  pocket
;

}private

: S\"           \ "string" -- caddr u
\ *G As *\fo{S"}, but translates escaped characters using
\ ** *\fo{parse\"} above.
  readEscaped count  state @
  if  postpone sliteral  then
; IMMEDIATE

privatize   \ hide the internal words


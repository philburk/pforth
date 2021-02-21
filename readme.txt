README for pForth - a Portable ANS-like Forth written in ANSI 'C'

by Phil Burk
with Larry Polansky, David Rosenboom and Darren Gibbs.
Support for 64-bit cells by Aleksej Saushev.

Last updated: April 24, 2018 V28

Code for pForth is maintained on GitHub at:
  https://github.com/philburk/pforth

Documentation for pForth at:
  http://www.softsynth.com/pforth/

For technical support please use the pForth forum at:
  http://groups.google.com/group/pforthdev

-- LEGAL NOTICE -----------------------------------------

Permission to use, copy, modify, and/or distribute this
software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-- Contents of SDK --------------------------------------

    platforms - tools for building pForth on various platforms
    platforms/unix - Makefile for unix

    csrc - pForth kernel in ANSI 'C'
    csrc/pf_main.c - main() application for a standalone Forth
    csrc/stdio - I/O code using basic stdio for generic platforms
    csrc/posix - I/O code for Posix platform
    csrc/win32 - I/O code for basic WIN32 platform
    csrc/win32_console - I/O code for WIN32 console that supports command line history

    fth - Forth code
    fth/util - utility functions

-- How to build pForth ------------------------------------

See pForth reference manual at:

  http://www.softsynth.com/pforth/pf_ref.php

-- How to run pForth ------------------------------------

Once you have compiled and built the dictionary, just enter:
     pforth

To compile source code files use:    INCLUDE filename

To create a custom dictionary enter in pForth:
    c" newfilename.dic" SAVE-FORTH
The name must end in ".dic".

To run PForth with the new dictionary enter in the shell:
    pforth -dnewfilename.dic

To run PForth and automatically include a forth file:
    pforth myprogram.fth

-- How to Test PForth ------------------------------------

You can test the Forth without loading a dictionary
which might be necessary if the dictionary can't be built.

Enter:   pforth -i
In pForth, enter:    3 4 + .
In pForth, enter:    loadsys
In pForth, enter:    10  0  do i . loop

PForth comes with a small test suite.  To test the Core words,
you can use the coretest developed by John Hayes.

Enter:  pforth
Enter:  include tester.fth
Enter:  include coretest.fth

To run the other tests, enter:

    pforth t_corex.fth
    pforth t_strings.fth
    pforth t_locals.fth
    pforth t_alloc.fth

They will report the number of tests that pass or fail.

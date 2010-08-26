README for pForth - a Portable ANS-like Forth written in ANSI 'C'

by Phil Burk
with Larry Polansky, David Rosenboom and Darren Gibbs.
Support for 64-bit cells by Aleksej Saushev.

Last updated: May 20, 2010 V26

Code for pForth is maintained on Google at:
   http://code.google.com/p/pforth/
   
Documentation for pForth at:
  http://www.softsynth.com/pforth/

For technical support please use the pForth forum at:
  http://groups.google.com/group/pforthdev
  
The author is available for customization of pForth, porting to new
platforms, or developing pForth applications on a contractual basis.
If interested, contact Phil Burk at:
  http://www.softsynth.com/contacts.php

-- LEGAL NOTICE -----------------------------------------

The pForth software code is dedicated to the public domain,
and any third party may reproduce, distribute and modify
the pForth software code or any derivative works thereof
without any compensation or license.  The pForth software
code is provided on an "as is" basis without any warranty
of any kind, including, without limitation, the implied
warranties of merchantability and fitness for a particular
purpose and their equivalents under the laws of any jurisdiction.

-- Contents of SDK --------------------------------------

	build - tools for building pForth on various platforms
	build/win32/vs2005 - Visual Studio 2005 Project and Solution
	build/unix - Makefile for unix
	
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

  http://www.softsynth.com/pforth/pf_ref.htm
  
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

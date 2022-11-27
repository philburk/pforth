# Release History for pForth - a Portable ANS-like Forth written in ANSI 'C'

PForth hosted at https://github.com/philburk/pforth

Documentation at http://www.softsynth.com/pforth/

## V2.0.0 #29 - unreleased

* Fixed FROUND, was leaving result on data stack instead of float stack, [#69](https://github.com/philburk/pforth/issues/69)
* Added standard version numbering, eg. "2.0.0".
* Added VERSION_CODE for software version checks.
* Added S\"
* Terminal is unbuffered on posix systems
* Added CMAKE build, (thanks Robin Rowe)
* Improve unix/Makefile, moved to "platforms" folder
* Added GitHub actions for CI
* Add compiler warnings about precision loss.
* Improve 64-bit CELL support.
* Allow header and code size to be more easily controlled.
* Fixed definition of PF_DEFAULT_HEADER_SIZE
* Change license to 0BSD
* Added privatize to history.fth

## V28 - 4/24/2018

* remove off_t
* too many changes to list, see commit history (TODO add changes)
* fix $ROM
* fix HISTORY
* fixes for MinGW build

## V27 - 11/22/2010

* Fixed REPOSITION-FILE FILE-SIZE and FILE-POSITION.
      They used to use single precision offset. Now use double as specified.
* Delete object directories in Makefile clean.
* Fixed "Issue 4: Filehandle remains locked upon INCLUDE error".
      http://code.google.com/p/pforth/issues/detail?id=4&can=1
* Fixed scrambled HISTORY on 64-bit systems. Was using CELL+ but really needed 4 +.
* Fixed floating point input. Now accepts "1E" as 1.0. Was Issue #2.
* Fixed lots of warning and made code compatible with C89 and ANSI. Uses -pedantic.
* Use fseek and ftell on WIN32 instead of fseeko and ftello.
* Makefile is now more standard. Builds in same dir as Makefile. Uses CFLAGS etc.
* Add support for console IO with _WATCOMC_
* Internal CStringToForth and ForthStringToC now take a destination size for safety.
* Run units tests for CStringToForth and ForthStringToC if PF_UNIT_TESTS is defined.

## V26  5/20/2010

* 64-bit support for M* UM/MOD etc by Aleksej Saushev. Thanks Aleksej!

## V25  5/19/2010

* Added 64-bit CELL support contributed by Aleksej Saushev. Thanks Aleksej!
* Added "-x c" to Makefile CCOPTS to prevent confusion with C++
* Allow space after -d command line option.
* Restore normal tty mode if pForth dictionary loading fails.

## V24 2/20/09

* Fixed Posix IO on Mac. ?TERMINAL was always returning true.
* ACCCEPT now emits a space at end of line before output.
* Fixed RESIZE because it was returning the wrong address.

## V23 8/4/2008

* Removed -v option from mkdir in build/unix/Makefile. It was not supported on FreeBSD.
      Thank you Alexsej Saushev for reporting this.

## V23  7/20/2008

* Reorganized for Google Code project.

## V22  (unreleased)

* Added command line history and cursor control words.
* Sped up UM* and M* by a factor of 3. Thanks to Steve Green for suggested algorithm.
* Modified ACCEPT so that a line at the end of a file that does NOT have a line
        terminator will now be processed.
* Use _getch(), _putch(), and _kbhit() so that KEY, EMIT and ?TERMINAL will work on PC.
* Fixed  : foo { -- } 55 ;  - Was entering local frame but not exiting. Now prints error.
* Redefined MAKE_ID to protect it from 16 bit ints
* John Providenza says "If you split local variables onto 2 lines, PForth crashes." Fixed. Also allow \
* Fixed float evaluation in EVALUATE in "quit.fth".
* Flush register cache for ffColon and ffSemiColon to prevent stack warnings from ;

## V21 - 9/16/1998

* Fixed some compiler warnings.

## V20

* Expand PAD for ConvertNumberToText so "-1 binary .s" doesn't crash.
      Thank you Michael Connor of Vancouver for reporting this bug.

* Removed FDROP in REPRESENT to fix stack underflow after "0.0 F.".
      Thank you Jim Rosenow of Minnesota for reporting this bug.
* Changed pfCharToLower to function to prevent macro expansion bugs under VXWORKS
      Thank you Jim Rosenow of Minnesota for reporting this bug.

* "0.0 F~" now checks actual binary encoding of floats. Before this it used to just
      compare value which was incorrect. Now "0.0 -0.0 0.0 F~" returns FALSE.

* Fixed definition of INPUT$ in tutorial.
      Thank you Hampton Miller of California for reporting this bug.

* Added support for producing a target dictionary with a different
      Endian-ness than the host CPU.  See PF_BIG_ENDIAN_DIC and PF_LITTLE_ENDIAN_DIC.

* PForth kernel now comes up in a mode that uses BASE for numeric input when
      started with "-i" option.  It used to always consider numeric input as HEX.
      Initial BASE is decimal.

## V19  4/1998

* Warn if local var name matches dictionary, : foo { count -- } ;
* TO -> and +-> now parse input stream. No longer use to-flag.
* TO -> and +-> now give error if used with non-immediate word.
* Added (FLITERAL) support to SEE.
* Aded TRACE facility for single step debugging of Forth words.
* Added stub for ?TERMINAL and KEY? for embedded systems.
* Added PF_NO_GLOBAL_INIT for no reliance on global initialization.
* Added PF_USER_FLOAT for customization of FP support.
* Added floating point to string conversion words (F.) (FS.) (FE.)
        For example:   : F.   (F.)  TYPE  SPACE  ;
* Reversed order that values are placed on return stack in 2>R
      so that it matches ANS standard.  2>R is now same as SWAP >R >R
      Thank you Leo Wong for reporting this bug.

* Added PF_USER_INIT and PF_USER_TERM for user definable init and term calls.

* FIXED memory leak in pfDoForth()

## V18

* Make FILL a 'C' primitive.
* optimized locals with (1_LOCAL@)
* optimized inner interpreter by 15%
* fix tester.fth failures
* Added define for PF_KEY_ECHOS which turns off echo in ACCEPT if defined.
* Fixed MARKER. Was equivalent to ANEW instead of proper ANS definition.
* Fixed saving and restoring of TIB when nesting include files.

## V17

* Fixed input of large floats.  0.7071234567 F.  used to fail.

## V16

* Define PF_USER_CUSTOM if you are defining your own custom 'C' glue routines.  This will ifndef the published example.
* Fixed warning in pf_cglue.c.
* Fixed SDAD in savedicd.fth.  It used to generate bogus 'C' code
      if called when (BASE != 10), as in HEX mode.
* Fixed address comparisons in forget.fth and private.fth for
      addresses above 0x80000000. Must be unsigned.
* Call FREEZE at end of system.fth to initialize rfence.
* Fixed 0.0 F. which used to leave 0.0 on FP stack.
* Added FPICK ( n -- ) ( i*f -- i*f f[n] )
* .S now prints hex numbers as unsigned.
* Fixed internal number to text conversion for unsigned nums.

## V15 - 2/15/97

* If you use PF_USER_FILEIO, you must now define PF_STDIN and PF_STDOUT among other additions. See "pf_io.h".
* COMPARE now matches ANS STRING word set!
* Added PF_USER_INC1 and PF_USER_INC2 for optional includes and host customization. See "pf_all.h".
* Fixed more warnings.
* Fixed >NAME and WORDS for systems with high "negative" addresses.
* Added WORDS.LIKE utility.  Enter:   WORDS.LIKE EMIT
* Added stack check after every word in high level interpreter.
      Enter QUIT to enter high level interpreter which uses this feature.
* THROW will no longer crash if not using high level interpreter.
* Isolated all host dependencies into "pf_unix.h", "pf_win32.h",
      "pf_mac.h", etc.  See "pf_all.h".
* Added tests for CORE EXT, STRINGS words sets.
* Added SEARCH
* Fixed WHILE and REPEAT for multiple WHILEs.
* Fixed .( ) for empty strings.
* Fixed FATAN2 which could not compile on some systems (Linux gcc).

## V14 - 12/23/96
* pforth command now requires -d before dictionary name.
            Eg.   pforth -dcustom.dic test.fth
* PF_USER_* now need to be defined as include file names.
* PF_USER_CHARIO now requires different functions to be defined.
        See "csrc/pf_io.h".
* Moved pfDoForth() from pf_main.c to pf_core.c to simplify
      file with main().
* Fix build with PF_NO_INIT
* Makefile now has target for embedded dictionary, "gmake pfemb".

## V13 - 12/15/9

* Add "extern 'C' {" to pf_mem.h for C++
* Separate PF_STATIC_DIC from PF_NO_FILEIO so that we can use a static
      dictionary but also have file I/O.
* Added PF_USER_FILEIO, PF_USER_CHARIO, PF_USER_CLIB.
* INCLUDE now aborts if file not found.
* Add +-> which allows you to add to a local variable, like +! .
* VALUE now works properly as a self fetching constant.
* Add CODE-SIZE and HEADERS-SIZE which lets you resize
      dictionary saved using SAVE-FORTH.
* Added FILE?. Enter "FILE? THEN" to see what files THEN is defined in.
* Fixed bug in local variables that caused problems if compilation
      aborted in a word with local variables.
* Added SEE which "disassembles" Forth words. See "see.fth".
* Added PRIVATE{ which can be used to hide low level support
      words.  See "private.fth".

## V12 - 12/1/96

* Advance pointers in pfCopyMemory() and pfSetMemory()
      to fix PF_NO_CLIB build.
* Increase size of array for PF_NO_MALLOC
* Eliminate many warnings involving type casts and (const char *)
* Fix error recovery in dictionary creation.
* Conditionally eliminate some include files for embedded builds.
* Cleanup some test files.

## V11 - 11/14/96

* Added support for AUTO.INIT and AUTO.TERM.  These are called
      automagically when the Forth starts and quits.
* Change all int to int32.
* Changed DO LOOP to ?DO LOOP in ENDCASE and LV.MATCH
      to fix hang when zero local variables.
* Align long word members in :STRUCT to avoid bus errors.

## V10 - 3/21/96

* Close nested source files when INCLUDE aborts.
* Add PF_NO_CLIB option to reduce OS dependencies.
* Add CREATE-FILE, fix R/W access mode for OPEN-FILE.
* Use PF_FLOAT instead of FLOAT to avoid DOS problem.
* Add PF_HOST_DOS for compilation control.
* Shorten all long file names to fit in the 8.3 format
      required by some primitive operating systems. My
      apologies to those with modern computers who suffer
      as a result.  ;-)

## V9 - 10/13/95

* Cleaned up and documented for alpha release.
* Added EXISTS?
* compile floats.fth if F* exists
* got PF_NO_SHELL working
* added TURNKEY to build headerless dictionary apps
* improved release script and rlsMakefile
* added FS@ and FS! for FLPT structure members

## V8 - 5/1/95

* Report line number and line dump when INCLUDE aborts
* Abort if stack depth changes in colon definition. Helps
      detect unbalanced conditionals (IF without THEN).
* Print bytes added by include.  Helps determine current file.
* Added RETURN-CODE which is returned to caller, eg. UNIX shell.
* Changed Header and Code sizes to 60000 and 150000
* Added check for overflowing dictionary when creating secondaries.

## V8 - 5/1/95

* Report line number and line dump when INCLUDE aborts
* Abort if stack depth changes in colon definition. Helps
      detect unbalanced conditionals (IF without THEN).
* Print bytes added by include.  Helps determine current file.
* Added RETURN-CODE which is returned to caller, eg. UNIX shell.
* Changed Header and Code sizes to 60000 and 150000
* Added check for overflowing dictionary when creating secondaries.

## V7 - 4/12/95

* Converted to 3DO Teamware environment
* Added conditional compiler [IF] [ELSE] [THEN], use like #if
* Fixed W->S B->S for positive values
* Fixed ALLOCATE FREE validation.  Was failing on some 'C' compilers.
* Added FILE-SIZE
* Fixed ERASE, now fills with zero instead of BL

## V6 - 3/16/95

* Added floating point
* Changed NUMBER? to return a numeric type
* Support double number entry, eg.   234.  -> 234 0

## V5 - 3/9/95

* Added pfReportError()
* Fixed problem with NumPrimitives growing and breaking dictionaries
* Reduced size of saved dictionaries, 198K -> 28K in one instance
* Funnel all terminal I/O through ioKey() and ioEmit()
* Removed dependencies on printf() except for debugging

## V4 - 3/6/95

* Added smart conditionals to allow IF THEN DO LOOP etc.
      outside colon definitions.
* Fixed RSHIFT, made logical.
* Added ARSHIFT for arithmetic shift.
* Added proper M*
* Added <> U> U<
* Added FM/MOD SM/REM /MOD MOD */ */MOD
* Added +LOOP EVALUATE UNLOOP EXIT
* Everything passes "coretest.fth" except UM/MOD FIND and WORD

## V3 - 3/1/95

* Added support for embedded systems: PF_NO_FILEIO
    and PF_NO_MALLOC.
* Fixed bug in dictionary loader that treated HERE as name relative.

## V2 - 8/94

* made improvements necessary for use with M2 Verilog testing

## V1 - 5/94

* built pForth from my Forth used in HMSL

----------------------------------------------------------


Enjoy,
Phil Burk

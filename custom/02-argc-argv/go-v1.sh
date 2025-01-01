#!/bin/sh

# Compile pForth with custom code and show that this works. 
# We assume a posix shell and system (but adaption should be easy to others).
# This improved version only compiles the custom code defined in CF_SOURCES.
# Warning: This patches the existing source tree and might create confusion when not used on separate Git branch in case an error occurs.
# Tested on MSYS2-Cygwin, Linux, FreeBSD (X86_64 architecture each), NetBSD (i386 architecture)

# copy demo sources. Thus we do not need to change the make file.

cp cf_demo2.c  ../../csrc/
patch -bcN ../../csrc/pf_main.c diff-pf_main.c
# patch was created via: diff -c old-pf_main.c new-pf_main.c > diff-pf_main.c
# patch options: -b=backup, -c=context-diff (same as in diff), -N=ignore reversed or already applied patches

echo
echo "----------------------------------------"
echo "make pforth (skip standalone executable)"
echo "----------------------------------------"
MAKE_CMD=`../get-make-cmd.sh`
cd ../../platforms/unix/
CF_SOURCES="cf_demo2.c" $MAKE_CMD pforth.dic

echo
echo "---------------------------"
echo "show that custom code works"
echo "---------------------------"
(./pforth -q ../../custom/02-argc-argv/demo.fth -- "01: icke dette" 02:kieke 03:mal) | tee cf-demo.output
echo "- - - - - - - - - - - - - -"
echo "is this the expected output?"
diff -s cf-demo.output ../../custom/02-argc-argv/demo.correct_output

echo
echo "----------------------------"
echo "restore original source tree"
echo "----------------------------"
rm cf-demo.output
rm ../../csrc/cf_demo2.c
mv ../../csrc/pf_main.c.orig ../../csrc/pf_main.c
CF_SOURCES="cf_demo2.c" $MAKE_CMD clean

echo
echo "-----------------"
echo "That's all folks!"
echo "-----------------"

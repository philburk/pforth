#!/bin/sh

# Compile pForth with custom code and show that this works. 
# We assume a posix shell and system (but adaption should be easy to others).
# This improved version only compiles the custom code defined in CF_SOURCES.
# Warning: This patches the existing source tree and might create confusion when not used on separate Git branch in case an error occurs.
# Tested on MSYS2-Cygwin, Linux, FreeBSD (X86_64 architecture each), NetBSD (i386 architecture)

# copy demo sources. Thus we do not need to change the make file.

cp ../cf_helpers.h  ../../csrc/
cp cf_demo1.c       ../../csrc/

echo
echo "----------------------------------------"
echo "make pforth (skip standalone executable)"
echo "----------------------------------------"
MAKE_CMD=`../get-make-cmd.sh`
cd ../../platforms/unix/

CF_SOURCES="cf_demo1.c" $MAKE_CMD clean pforth.dic

# create a nuisance to delete
mv ../../csrc/cf_helpers.h ./terrible_nuisance.asm

echo
echo "---------------------------"
echo "show that custom code works"
echo "---------------------------"
./pforth -q ../../custom/01-be-gone/demo.fth

echo
echo "----------------------------"
echo "restore original source tree"
echo "----------------------------"
rm ../../csrc/cf_demo1.c
CF_SOURCES="cf_demo1.c" $MAKE_CMD clean

echo
echo "-----------------"
echo "That's all folks!"
echo "-----------------"

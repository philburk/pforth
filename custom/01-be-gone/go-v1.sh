#!/bin/sh

# Compile pForth with custom code and show that this works. 
# We assume a posix shell and system (but adaption should be easy to others).
# This improved version only compiles the custom code defined in CF_SOURCES.
# Warning: This patches the existing source tree and might create confusion when not used on separate Git branch in case an error occurs.
# Tested on MSYS2-Cygwin, Linux, FreeBSD (X86_64 architecture each), NetBSD (i386 architecture)

os=`uname -o 2>/dev/null`
if test -z "$os" ; then
  # NetBSD-uname does not implement '-o' option
  os=`uname -s`
fi
case "$os" in
  "FreeBSD" | "NetBSD")
    MAKE_CMD="gmake"
    ;;
  *) # e.g. "Msys" | "GNU/Linux"
    MAKE_CMD="make"
    ;;
esac

# copy demo sources. Thus we do not need to change the make file.
cp ../cf_helpers.h  ../../csrc/
cp cf_demo1.c       ../../csrc/

# make pforth (skip standalone executable)
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

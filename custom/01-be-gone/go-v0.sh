#!/bin/sh

# Compile pForth with custom code and show that this works. 
# We assume a posix shell and system (but adaption should be easy to others).
# Note: This is the easiest solution but ignores PForths best practices set up in pfcustom.c
# Warning: This patches the existing source tree and might create confusion when not used on separate Git branch in case an error occurs.
# Tested on MSYS2-Cygwin, Linux, FreeBSD (X86_64 architecture each), NetBSD (i386 architecture)

# save original C sources and copy demo sources. Thus we do not need to change the make file.
mv ../../csrc/pfcustom.c ../../csrc/pfcustom_c.original
cp ../cf_helpers.h  ../../csrc/
cp cf_demo1.c       ../../csrc/pfcustom.c

echo
echo "----------------------------------------"
echo "make pforth (skip standalone executable)"
echo "----------------------------------------"
MAKE_CMD=`./get-make-cmd.sh`
cd ../../platforms/unix/
# We would not even need to define DPF_USER_CUSTOM since it is only used in the original pfcustom.c we overwrote.
PF_USER_CUSTOM="1" $MAKE_CMD clean pforth.dic

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
mv ../../csrc/pfcustom_c.original ../../csrc/pfcustom.c 
$MAKE_CMD clean

echo
echo "-----------------"
echo "That's all folks!"
echo "-----------------"

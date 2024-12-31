#!/bin/sh

MAKE_CMD="make"    # on Linux, MSYS2-Cygwin
#MAKE_CMD="gmake"  # on FreeBSD?, NetBSD


# Compile pForth with custom code and show that this works.
# We assume a posix shell and system (but adaption should be easy to others).
# This patches the existing source tree and might create confusion when not used on separate Git branch in case an error occurs.

# save original C sources and copy demo sources. Thus we do not need to change the make file.
mv ../../csrc/pfcustom.c ../../csrc/pfcustom_c.original
cp cf_helpers.h ../../csrc/
cp cf_demo1.c   ../../csrc/pfcustom.c # 

# make pforth (skip standalone executable)
# We would not even need to define DPF_USER_CUSTOM since it is only used in the original pfcustom.c we overwrote.
cd ../../platforms/unix/
DPF_USER_CUSTOM="1" $MAKE_CMD clean pforth.dic

# create a nuisance to delete
mv ../../csrc/cf_helpers.h ./terrible_nuisance.asm

echo
echo "---------------------------"
echo "show that custom code works"
echo "---------------------------"
cat >demo1.fth << EOF
." f4711( 0, 100 ) = "
0 f4711 .
100 f4711 .
CR

." be-gone: "
s" terrible_nuisance.asm" be-gone 0= dup 
if 
  ." works."
  drop
else
  ." returns error=" .
then CR
EOF
./pforth -q demo1.fth

# restore original source tree
rm demo1.fth
mv ../../csrc/pfcustom_c.original ../../csrc/pfcustom.c 
$MAKE_CMD clean

echo
echo "-----------------"
echo "That's all folks!"
echo "-----------------"

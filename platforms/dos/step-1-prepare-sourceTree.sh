#!/bin/sh

echo
echo "0) clean previous build folder"
echo "------------------------------"
rm -rf ./src/

echo
echo "1) copy sources here"
echo "--------------------"
mkdir src
mkdir src/csrc
mkdir src/fth
cp -r ../../csrc/ ./src/
cp -r ../../fth/  ./src/
cp    missing.*   ./src/csrc/
cp    Makefile.mk ./src/csrc/
find . | grep -i cmake | xargs rm  # Nobody likes CMake!!!
mv ./src/csrc/win32_console/ ./src/csrc/win32csl/

echo
echo "2a) rename files to 8+3 filename convention"
echo "-------------------------------------------"
cat rename.txt | awk '{if( NF==2 ) print "mv " $1 " " $2}' | sh
echo "2b) verify that 8+3 filename convention is kept."
echo "-----------------------------------------------"
find src/ -type f | awk -f too-long-filenames.awk | tee yMissingRenames
echo "-----------------------------------------------"
if test -s yMissingRenames
then
  echo "ABORT: more files need to be renamed!"
else # yMissingRenames

  echo
  echo "3) extract list of filenames to substitute from 'rename.txt'"
  echo "-------------------------------------------------------------"
  cat rename.txt | awk '{if( NF==2 ) print "echo s#`basename " $1 "`#`basename " $2 "`#g"  }' | sh | sed -e 's#\.#\\\.#g' > yFileNames

  echo
  echo "4) patch file contents"
  echo "----------------------"
  for curFile in `find src/ -type f`
  do
    sed -f yFileNames -i $curFile
  done
  rm yFileNames

  echo
  echo "4) include missing.h in pf_inner.c"
  echo "----------------------------------"
  mv ./src/csrc/pf_inner.c tmp.c
  cat - tmp.c > ./src/csrc/pf_inner.c << EOF
#include "missing.h"
EOF
  rm tmp.c

  echo
  echo "99) done"
  echo "--------"
  rm yMissingRenames

fi   # yMissingRenames

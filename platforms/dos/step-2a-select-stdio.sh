#!/bin/sh

ioSources="fiostdio.c iostdio.c"

cd ./src/csrc/

# 1) copy ioSources to parent dir (so all sources are in 1 folder) and patch the include statements accordingly
for srcFile in $ioSources
do
  cat "./stdio/$srcFile" | sed -e 's#\.\./##g' > $srcFile
done

# 2) include ioSources in Makefile
objFiles=`echo $ioSources | sed -e 's#\.c#\\\.obj#g'  `
cat Makefile.mk | sed -e "s/ioObjFiles     =/ioObjFiles     = $objFiles/g" > Makefile
rm Makefile.mk

cd ../../
echo "done"

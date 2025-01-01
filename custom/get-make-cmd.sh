#!/bin/sh

os=`uname -o 2>/dev/null`
if test -z "$os" ; then
  # NetBSD-uname does not implement '-o' option
  os=`uname -s`
fi

case "$os" in
  "FreeBSD" | "NetBSD")
    echo "gmake"
    ;;
  *) # e.g. "Msys" | "GNU/Linux"
    echo "make"
    ;;
esac

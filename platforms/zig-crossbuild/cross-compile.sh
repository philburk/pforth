#!/bin/sh

# example for crosscompiling
# note-1: Please make sure zig is available on the path!
# depending on the target you may need to set IO_SOURCE as well:
#   target="x86_64-windows-gnu" IO_SOURCE="pf_fileio_stdio.c pf_io_win32.c" ./cross-compile.sh pforth
# note-2: above compiles a Windows executable which may fail at runtime!

if test -z "$target"
then
  # this builds a static executable with MUSL
  target=x86_64-linux-musl
fi

CC="zig cc --target=$target" make -f ../unix/Makefile "$@"

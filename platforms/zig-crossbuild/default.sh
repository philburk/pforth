#!/bin/sh

# note: Please make sure zig is available on the path!

CC="zig cc" make -f ../unix/Makefile "$@"

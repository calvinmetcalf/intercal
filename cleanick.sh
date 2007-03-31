#!/bin/sh
# make cleanick as a shell-script, for the benefit of DJGPP systems with broken
# versions of make.
# This batch file removes all files that weren't in the distribution.
rm -f bin/*.*
rm -f lib/*.*
rm -f include/*.*
rm -f temp/*.h
rm -f temp/*.c
rm -f temp/*.o
rm -f temp/*.a
rm -f temp/*.y
rm -f temp/*.l
rm -f temp/*.bat
rm -f temp/*.sh
rm -f temp/ickmain.exe
rm -f temp/*~
rm -f src/*~
rm -f *~
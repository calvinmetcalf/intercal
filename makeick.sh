#!/bin/sh

# C-INTERCAL 0.25 compilation batch file for sh and bash.

# This code will attempt to recompile the parser and lexer using
# bison and flex. If you don't have them, you will get an error
# message and the parser and lexer included with the distribution
# will be used. My DJGPP distribution seems to require that you
# copy DJDIR/bin/m4.exe to the temp subdir of the INTERCAL
# distribution or bison won't work; YMMV.

# Using configure.sh and make is recommended; this script is a fallback for
# when you can't get configure.sh and/or make to work on your system; make
# is broken on my DJGPP distribution, so I use this file instead.

# This file is meant to contain the same content as makeick.bat; it's simply
# written for sh rather than COMMAND.COM (and doesn't make use of all its
# features, as it's meant to be exactly analogous).

# Run this file from the directory in which this script is installed.

cp src/* temp
cd temp
bison -d -oparser.c parser.y
flex -olexer.c lexer.l

# If you don't care about being able to debug ick, use
# CFLAGS=-s -O2 -DICKNOSEARCH -W -Wall
CFLAGS='-gstabs+ -O2 -DICKNOSEARCH -W -Wall'

# Set this to a location of the GNU GPL on your system.
# Please use forward-slash notation!
COPYLOC=c:/djgpp/copying

echo If the following line fails, edit makeick.sh to point COPYLOC at a
echo location of the GNU GPL on your system. You can point it to the copy of
echo COPYING.txt provided.
cp ${COPYLOC} /dev/null || exit
echo Compiling...
gcc -c ${CFLAGS} parser.c
gcc -c ${CFLAGS} lexer.c
gcc -c ${CFLAGS} feh2.c
gcc -c ${CFLAGS} dekludge.c
gcc -c ${CFLAGS} perpet.c
gcc -c ${CFLAGS} cesspool.c
gcc -c ${CFLAGS} numerals.c
gcc -c ${CFLAGS} unravel.c
gcc -c ${CFLAGS} fiddle.c
gcc -c ${CFLAGS} arrgghh.c
gcc -c ${CFLAGS} lose.c
gcc -c ${CFLAGS} "-DYUKCOPYLOC=\"${COPYLOC}\"" yuk.c
ar cr libick.a cesspool.o numerals.o fiddle.o arrgghh.o lose.o
ranlib libick.a
ar cr libickmt.a cesspool.o numerals.o fiddle.o arrgghh.o lose.o unravel.o
ranlib libickmt.a
ar cr libyuk.a yuk.o
ranlib libyuk.a
gcc ${CFLAGS} -oick.exe parser.o lexer.o perpet.o feh2.o dekludge.o lose.o fiddle.o
cp ick.exe ../bin
cp *.a ../lib
cd ..
cp src/ick-wrap.c lib
cp src/pickwrap.c lib
cp pit/lib/syslib.i lib
cp pit/lib/syslib.?i lib
cp src/fiddle.h include
cp src/abcess.h include
cp src/lose.h include
cp src/yuk.h include
echo Compilation complete unless there were errors.
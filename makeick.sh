#!/bin/sh

# C-INTERCAL 0.25 compilation batch file for sh and bash.

# This code will attempt to recompile the parser and lexer using
# bison and flex. If you don't have them, you will get an error
# message and the parser and lexer included with the distribution
# will be used.

# Using configure.sh and make is recommended; this script is a fallback for
# when you can't get configure.sh and/or make to work on your system.

# This file is meant to contain the same content as makeick.bat; it's simply
# written for sh rather than COMMAND.COM (and doesn't make use of all its
# features, as it's meant to be exactly analogous).

# Run this file from the directory in which this script is installed.

cp src/* temp
cd temp
cp configdj.h config.h
echo 'Trying to build parsers and lexers (if this fails, e.g. if you do not'
echo 'have bison/flex, this will produce errors but the compiler will build'
echo 'anyway using prebuilt versions)...'
bison -d -oparser.c parser.y
flex -olexer.c lexer.l
bison -ooil.c oil.y

# If you don't care about being able to debug ick, use
# CFLAGS=-s -O2 -DICKNOSEARCH -W -Wall
CFLAGS='-gstabs+ -O2 -DICKNOSEARCH -W -Wall'

echo Compiling...
gcc ${CFLAGS} -o oil.exe oil.c
oil.exe < idiotism.oil > idiotism.c
gcc -c ${CFLAGS} idiotism.c
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
gcc -c ${CFLAGS} uncommon.c
gcc -c ${CFLAGS} yuk.c
ar cr libick.a cesspool.o numerals.o fiddle.o arrgghh.o lose.o
ranlib libick.a
ar cr libickmt.a cesspool.o numerals.o fiddle.o arrgghh.o lose.o unravel.o
ranlib libickmt.a
ar cr libyuk.a yuk.o uncommon.o
ranlib libyuk.a
gcc ${CFLAGS} -oick.exe parser.o lexer.o perpet.o feh2.o idiotism.o dekludge.o lose.o fiddle.o uncommon.o
cp ick.exe ../bin
cp *.a ../lib
cd ..
cp src/ick-wrap.c lib
cp src/pickwrap.c lib
cp pit/lib/syslib.i lib
cp pit/lib/syslib.?i lib
cp COPYING.txt lib
cp src/fiddle.h include
cp src/abcess.h include
cp src/lose.h include
cp src/yuk.h include
echo Compilation complete unless there were errors.

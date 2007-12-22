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
gcc ${CFLAGS} -o bin2c.exe bin2c.c
bin2c.exe clc_cset_atari < atari.bin > cset-a.c
bin2c.exe clc_cset_baudot < baudot.bin > cset-b.c
bin2c.exe clc_cset_ebcdic < ebcdic.bin > cset-e.c
bin2c.exe clc_cset_latin1 < latin1.bin > cset-l.c
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
gcc -c ${CFLAGS} convickt.c
gcc -c ${CFLAGS} clc-cset.c
gcc -c ${CFLAGS} cset-a.c
gcc -c ${CFLAGS} cset-b.c
gcc -c ${CFLAGS} cset-e.c
gcc -c ${CFLAGS} cset-l.c
ar cr libick.a cesspool.o numerals.o fiddle.o arrgghh.o lose.o clc-cset.o cset-?.o uncommon.o
ranlib libick.a
ar cr libickmt.a cesspool.o numerals.o fiddle.o arrgghh.o lose.o unravel.o clc-cset.o cset-?.o uncommon.o
ranlib libickmt.a
ar cr libyuk.a yuk.o uncommon.o
ranlib libyuk.a
gcc ${CFLAGS} -oick.exe parser.o lexer.o perpet.o feh2.o idiotism.o dekludge.o lose.o fiddle.o uncommon.o
gcc ${CFLAGS} -oconvickt.exe convickt.o clc-cset.o uncommon.o
cp ick.exe ../bin
cp convickt.exe ../bin
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
cp src/*.bin include
echo Compilation complete unless there were errors.

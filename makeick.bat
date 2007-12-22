@echo off

rem C-INTERCAL 0.25 compilation batch file for DJGPP.

rem This code will attempt to recompile the parser and lexer using
rem bison and flex. If you don't have them, you will get an error
rem message and the parser and lexer included with the distribution
rem will be used.

if not exist makeick.bat goto :direrror

copy /y src\*.* temp
cd temp
copy /y configdj.h config.h
echo Trying to build parsers and lexers (if this fails, e.g. if you do not
echo have bison/flex, this will produce errors but the compiler will build
echo anyway using prebuilt versions)...
bison -d -oparser.c parser.y
flex -olexer.c lexer.l
bison -ooil.c oil.y

rem If you don't care about being able to debug ick, use
rem set CFLAGS=-s -O2 -DICKNOSEARCH -W -Wall
set CFLAGS=-gstabs+ -O2 -DICKNOSEARCH -W -Wall

echo Compiling...
gcc %CFLAGS% -o oil.exe oil.c
oil.exe < idiotism.oil > idiotism.c
gcc %CFLAGS% -o bin2c.exe bin2c.c
bin2c.exe clc_cset_atari < atari.bin > cset-a.c
bin2c.exe clc_cset_baudot < baudot.bin > cset-b.c
bin2c.exe clc_cset_ebcdic < ebcdic.bin > cset-e.c
bin2c.exe clc_cset_latin1 < latin1.bin > cset-l.c
gcc -c %CFLAGS% idiotism.c
gcc -c %CFLAGS% parser.c
gcc -c %CFLAGS% lexer.c
gcc -c %CFLAGS% feh2.c
gcc -c %CFLAGS% dekludge.c
gcc -c %CFLAGS% perpet.c
gcc -c %CFLAGS% cesspool.c
gcc -c %CFLAGS% numerals.c
gcc -c %CFLAGS% unravel.c
gcc -c %CFLAGS% fiddle.c
gcc -c %CFLAGS% arrgghh.c
gcc -c %CFLAGS% lose.c
gcc -c %CFLAGS% uncommon.c
gcc -c %CFLAGS% yuk.c
gcc -c %CFLAGS% convickt.c
gcc -c %CFLAGS% clc-cset.c
gcc -c %CFLAGS% cset-a.c
gcc -c %CFLAGS% cset-b.c
gcc -c %CFLAGS% cset-e.c
gcc -c %CFLAGS% cset-l.c

ar cr libick.a cesspool.o numerals.o fiddle.o arrgghh.o lose.o clc-cset.o cset-?.o uncommon.o
ranlib libick.a
ar cr libickmt.a cesspool.o numerals.o fiddle.o arrgghh.o lose.o unravel.o clc-cset.o cset-?.o uncommon.o
ranlib libickmt.a
ar cr libyuk.a yuk.o uncommon.o
ranlib libyuk.a
echo parser.o lexer.o perpet.o feh2.o idiotism.o dekludge.o lose.o fiddle.o uncommon.o > ickmgcc.rsp
gcc %CFLAGS% -oick.exe @ickmgcc.rsp
del ickmgcc.rsp
gcc %CFLAGS% -oconvickt.exe convickt.o clc-cset.o uncommon.o
copy /y ick.exe ..\bin
copy /y convickt.exe ..\bin
copy /y *.a ..\lib
cd ..
copy /y src\ick-wrap.c lib
copy /y src\pickwrap.c lib
copy /y pit\lib\syslib.i lib
copy /y pit\lib\syslib.?i lib
copy /y COPYING.txt lib
copy /y src\fiddle.h include
copy /y src\abcess.h include
copy /y src\lose.h include
copy /y src\yuk.h include
copy /y src\*.bin include
echo Compilation complete unless there were errors.
goto :end
:direrror
echo Please run this batch file from its own directory.
:end

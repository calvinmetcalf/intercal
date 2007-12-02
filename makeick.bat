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
ar cr libick.a cesspool.o numerals.o fiddle.o arrgghh.o lose.o
ranlib libick.a
ar cr libickmt.a cesspool.o numerals.o fiddle.o arrgghh.o lose.o unravel.o
ranlib libickmt.a
ar cr libyuk.a yuk.o uncommon.o
ranlib libyuk.a
gcc %CFLAGS% -oick.exe parser.o lexer.o perpet.o feh2.o idiotism.o dekludge.o lose.o fiddle.o uncommon.o
copy /y ick.exe ..\bin
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
echo Compilation complete unless there were errors.
goto :end
:direrror
echo Please run this batch file from its own directory.
:end

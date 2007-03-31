@echo off

rem C-INTERCAL 0.25 compilation batch file for DJGPP.

rem This code will attempt to recompile the parser and lexer using
rem bison and flex. If you don't have them, you will get an error
rem message and the parser and lexer included with the distribution
rem will be used. My DJGPP distribution seems to require that you
rem copy DJDIR\bin\m4.exe to the temp subdir of the INTERCAL
rem distribution or bison won't work; YMMV.

copy /y src\*.* temp
cd temp
bison -d -oparser.c parser.y
flex -olexer.c lexer.l

rem If you don't care about being able to debug ick, use
rem set CFLAGS=-s -O2 -DICKNOSEARCH -W -Wall
set CFLAGS=-gstabs+ -O2 -DICKNOSEARCH -W -Wall

rem Set this to a location of the GNU GPL on your system...
set COPYLOC=C:\DJGPP\COPYING
rem ...and this to the same thing, but within "s and with backslashes doubled.
set COPYLOCC="C:\\DJGPP\\COPYING"

if not exist %COPYLOC% goto :error
echo Compiling...

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
gcc -c %CFLAGS% -DYUKCOPYLOC=\"%COPYLOCC%\" yuk.c
ar cr libick.a cesspool.o numerals.o fiddle.o arrgghh.o lose.o
ranlib libick.a
ar cr libickmt.a cesspool.o numerals.o fiddle.o arrgghh.o lose.o unravel.o
ranlib libickmt.a
ar cr libyuk.a yuk.o
ranlib libyuk.a
gcc %CFLAGS% -oickmain.exe parser.o lexer.o perpet.o feh2.o dekludge.o lose.o fiddle.o
copy /y ickmain.exe ..\bin
copy /y *.a ..\lib
cd ..
copy /y src\ick-wrap.c lib
copy /y src\pickwrap.c lib
copy /y pit\lib\syslib.i lib
copy /y pit\lib\syslib.?i lib
copy /y src\fiddle.h include
copy /y src\abcess.h include
copy /y src\lose.h include
copy /y src\yuk.h include
copy /y src\ick.bat bin
echo Compilation complete unless there were errors.
goto :end
:error
echo You need to tell this batch file where the GNU GPL is on your system.
echo Please edit this batch file and set COPYLOC to a correct location.
echo (You can point it to the copy of COPYING.txt provided.)
:end
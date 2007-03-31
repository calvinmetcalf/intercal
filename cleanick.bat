@echo off
rem This batch file removes all files that aren't needed to recompile.
del /q bin\*.*
del /q lib\*.*
del /q include\*.*
del /q temp\*.h
del /q temp\*.c
del /q temp\*.o
del /q temp\*.a
del /q temp\*.y
del /q temp\*.l
del /q temp\*.bat
del /q temp\*.sh
del /q temp\ickmain.exe

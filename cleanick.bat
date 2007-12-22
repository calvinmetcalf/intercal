@echo off
rem This batch file removes all files that aren't needed to recompile.
del /Q bin\*.*
del /Q lib\*.*
del /Q include\*.*
del /Q temp\*.h
del /Q temp\*.c
del /Q temp\*.o
del /Q temp\*.a
del /Q temp\*.y
del /Q temp\*.l
del /Q temp\*.bat
del /Q temp\*.sh
del /Q temp\*.exe
del /Q temp\*.bin
del /Q temp\*.in
del /Q temp\*.oil

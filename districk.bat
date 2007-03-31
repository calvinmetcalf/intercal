@echo off
rem This batch file removes all files that weren't in the distribution.
ren temp\parser.c parser.tmp
ren temp\parser.h parserh.tmp
ren temp\lexer.c lexer.tmp
call cleanick
ren temp\parser.tmp parser.c
ren temp\parserh.tmp parser.h
ren temp\lexer.tmp lexer.c

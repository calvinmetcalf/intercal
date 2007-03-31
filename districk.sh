#!/bin/sh
# make districk as a shell-script, for the benefit of DJGPP systems with broken
# versions of make.
# This batch file removes all files that weren't in the distribution.
mv temp/parser.c temp/parser.tmp
mv temp/parser.h temp/parserh.tmp
mv temp/lexer.c temp/lexer.tmp
sh cleanick.sh
mv temp/parser.tmp temp/parser.c
mv temp/parserh.tmp temp/parser.h
mv temp/lexer.tmp temp/lexer.c

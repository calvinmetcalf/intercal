#!/bin/sh
# This is a script to remove trailing ^Ms that might otherwise get into files
# when running under DJGPP, to prevent the files being unusable on Unix.

dtou *.txt
dtou etc/intercal.el
dtou src/*.c
dtou src/*.h
dtou src/*.l
dtou src/*.y
dtou src/*.sh
dtou pit/*.doc
dtou pit/*.i
dtou pit/tests/*.doc
dtou pit/tests/*.i
dtou pit/lib/*.doc
dtou pit/lib/*.i

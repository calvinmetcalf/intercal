#!/bin/sh
# This is a script to remove trailing ^Ms that might otherwise get into files
# when running under DJGPP, to prevent the files being unusable on Unix.
# Likewise, it adds trailing ^Ms to .bat files so they run under DOS shells.


dtou *.*
dtou ickspec
dtou etc/intercal.el
dtou src/*.c
dtou src/*.h
dtou src/*.l
dtou src/*.y
dtou src/*.in
dtou pit/*.doc
dtou pit/*.i
dtou pit/tests/*.doc
dtou pit/tests/*.i
dtou pit/lib/*.doc
dtou pit/lib/*.i
utod *.bat

#!/bin/sh
if test ! -f ./Makefile.am
then
    echo Please run this script from its own directory.
    echo It will build into a '"build"' subdirectory of the top directory,
    echo be capable of running without installing, and if installed,
    echo install into your main DJGPP installation directory.
    exit 1
fi
if test ! -f /dev/env/DJDIR/bin/sh.exe
then
    echo Please set DJDIR to your main DJGPP installation directory.
    echo For instance, '"export DJDIR=/dev/c/djgpp".'
    exit 1
fi
mkdir -p ../build
rmdir ./-p > /dev/null 2>&1
cd ../build
siteconfig=`echo ../buildaux/djgpp-*.sh`
# DJGPP overrides CONFIG_SITE somehow, so change it with sed.
echo Adjusting build scripts for a DJGPP build...
sed -i "s=\$CONFIG_SITE=$siteconfig=g" ../configure
export CONFIG_SHELL=/dev/env/DJDIR/bin/sh.exe
export SHELL=$CONFIG_SHELL
echo Configuring...
if ../configure; then
    echo Compiling...
    make
fi

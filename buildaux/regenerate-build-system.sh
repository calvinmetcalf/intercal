#!/bin/sh
set -e
if test ! -f regenerate-build-system.sh
then
    echo Please run this script from its own directory.
    exit 1
fi
cd ..
autoreconf -i || echo autoreconf requested rerunning Automake, doing that now...
autoreconf
autoconf buildaux/buildconfig.ac > buildaux/buildconfig
chmod a+x buildaux/buildconfig
cd buildaux
autoheader buildconfig.ac

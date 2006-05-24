#!/bin/sh

version=$1

cat <<EOF
Name: intercal
Version: ${version}
Release: 1
URL: http://www.catb.org/~esr/intercal/
Source: intercal-${version}.tar.gz
License: GPL
Group: Development/Languages
Summary: the language that kills the weak and drives mad the strong

%description
An implementation of the language INTERCAL, legendary for
its perversity and horribleness (this version adds COME FROM
for extra flavor).  Comes with language manual and examples
including possibly the entire extant body of INTERCAL code.
Now supports i18n and l14n (to Ancient Roman locale only)
Now with fix patch by Donald Knuth.

%prep
%setup -q

%build
configure --prefix=/usr
make

%install
rm -fr /usr/share/intercal-${version}/*
make install

%files
/usr/bin/ick
/usr/lib/libick.a
/usr/lib/libickmt.a
/usr/lib/libyuk.a
/usr/include/intercal-${version}/lose.h
/usr/include/intercal-${version}/abcess.h
/usr/include/intercal-${version}/fiddle.h
/usr/include/intercal-${version}/yuk.h
/usr/share/intercal-${version}/ick-wrap.c
/usr/share/intercal-${version}/
%doc README BUGS NEWS
EOF
#find pit -type f -print | sed -e "/^/s//\/usr\/share\/intercal-${version}\//" -e '/RCS/d'



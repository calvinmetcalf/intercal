Name: intercal
Version: 0.22
Release: 1
URL: http://www.catb.org/~esr/intercal/
Source: intercal-0.22.tar.gz
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
rm -fr /usr/share/intercal-0.22/*
make install

%files
/usr/bin/ick
/usr/lib/libick.a
/usr/include/intercal-0.22/lose.h
/usr/include/intercal-0.22/abcess.h
/usr/include/intercal-0.22/fiddle.h
/usr/share/intercal-0.22/ick-wrap.c
/usr/share/intercal-0.22/
%doc README BUGS NEWS

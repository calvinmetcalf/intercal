# Generated automatically from Makefile.in by configure.
#
# Productions for the INTERCAL distribution
#
VERSION=0.28

# Common prefix for machine-independent installed files.
prefix = /usr/local
# Common prefix for machine-dependent installed files.
exec_prefix = ${prefix}

# Debian doesn't like versioned subdirs; give them a way to unversion
# the subdir.

ick_specific_subdir = ick-0.28

# Modern versions of autoconf use an overridable datarootdir, and this
# doesn't do any harm with older versions.
datarootdir = @datarootdir@
# Directory in which to install binaries.
bindir = ${exec_prefix}/bin
# Directory in which to install libraries.
libdir = ${exec_prefix}/lib
# Directory to install data files.
datadir = ${prefix}/share/$(ick_specific_subdir)
# Directory to install the include files in.
incdir = ${prefix}/include/$(ick_specific_subdir)
# Directory to install Info files in.
infodir = ${prefix}/info
# Directory to install manpages in.
mandir = ${prefix}/man
# Directory file for Info installation.
infodirfile = /usr/share/info/dir

# This can be overridden from the command line
DESTDIR=

CC = gcc
CFLAGS = -g -O2 -DICKINCLUDEDIR=\"$(incdir)\" -DICKDATADIR=\"$(datadir)\" -DICKBINDIR=\"$(bindir)\" -DICKLIBDIR=\"$(libdir)\" -DYYDEBUG -O2 -W -Wall -I./src -I./temp
LDFLAGS = 
LEX = flex
LEXFLAGS=
YACC = bison -y
YACCFLAGS =

LOADLIBS =  -lfl

CSOURCES = src/parser.y src/lexer.l src/feh2.c src/dekludge.c src/ick_lose.c src/fiddle.c src/perpet.c src/unravel.c src/uncommon.c src/convickt.c src/clc-cset.c src/bin2c.c
ISOURCES = src/cesspool.c src/arrgghh.c src/ick-wrap.c src/pickwrap.c src/numerals.c
YSOURCES = src/yuk.c
HEADERS = src/ick.h src/ick_lose.h src/sizes.h src/abcess.h src/fiddle.h src/feh.h src/yuk.h src/uncommon.h src/ick_ec.h
BUILD = Makefile.in config.in config.sh src/configh.in ickspec.in ickspec src/cooptsh.in
MISC = etc/intercal.el src/ctrlmfix.sh src/ctrlmfix.bat src/*.bin
DOCS = BUGS.txt COPYING.txt NEWS.txt README.txt
SOURCES = $(CSOURCES) $(ISOURCES) $(YSOURCES) $(HEADERS) $(BUILD) $(MISC) $(DOCS) pit doc

all: bin/ick bin/convickt lib/libick.a lib/libickmt.a lib/libyuk.a lib/libickec.a ial

ial: include/ick_ec.h include/fiddle.h include/abcess.h include/ick_lose.h include/yuk.h include/syslib.i include/syslib.3i include/syslib.4i include/syslib.5i include/syslib.6i include/syslib.7i lib/ick-wrap.c lib/pickwrap.c lib/coopt.sh lib/COPYING.txt lib/syslibc.c include/atari.bin include/baudot.bin include/ebcdic.bin include/latin1.bin

include/ick_ec.h: src/ick_ec.h
	-cp src/ick_ec.h include/ick_ec.h
include/fiddle.h: src/fiddle.h
	-cp src/fiddle.h include/fiddle.h
include/abcess.h:src/abcess.h
	-cp src/abcess.h include/abcess.h
include/ick_lose.h:src/ick_lose.h
	-cp src/ick_lose.h include/ick_lose.h
include/yuk.h:src/yuk.h
	-cp src/yuk.h include/yuk.h

include/syslib.i: pit/lib/syslib.i
	-cp pit/lib/syslib.i include/syslib.i
include/syslib.3i: pit/lib/syslib.3i
	-cp pit/lib/syslib.3i include/syslib.3i
include/syslib.4i: pit/lib/syslib.4i
	-cp pit/lib/syslib.4i include/syslib.4i
include/syslib.5i: pit/lib/syslib.5i
	-cp pit/lib/syslib.5i include/syslib.5i
include/syslib.6i: pit/lib/syslib.6i
	-cp pit/lib/syslib.6i include/syslib.6i
include/syslib.7i: pit/lib/syslib.7i
	-cp pit/lib/syslib.7i include/syslib.7i

lib/ick-wrap.c:src/ick-wrap.c
	-cp src/ick-wrap.c lib/ick-wrap.c

lib/pickwrap.c:src/pickwrap.c
	-cp src/pickwrap.c lib/pickwrap.c

lib/coopt.sh:temp/coopt.sh
	-cp temp/coopt.sh lib/coopt.sh

lib/COPYING.txt:COPYING.txt
	-cp COPYING.txt lib/COPYING.txt

lib/syslibc.c:pit/lib/syslibc.c
	-cp pit/lib/syslibc.c lib/syslibc.c

include/atari.bin:src/atari.bin
	-cp src/atari.bin include/atari.bin
include/baudot.bin:src/baudot.bin
	-cp src/baudot.bin include/baudot.bin
include/ebcdic.bin:src/ebcdic.bin
	-cp src/ebcdic.bin include/ebcdic.bin
include/latin1.bin:src/latin1.bin
	-cp src/latin1.bin include/latin1.bin

bin/ick: temp/parser.o temp/lexer.o temp/feh2.o temp/dekludge.o temp/idiotism.o temp/ick_lose.o temp/fiddle.o temp/perpet.o temp/uncommon.o
	$(CC) temp/perpet.o temp/parser.o temp/lexer.o temp/feh2.o temp/dekludge.o temp/idiotism.o temp/ick_lose.o temp/fiddle.o temp/uncommon.o $(LDFLAGS) -o bin/ick

bin/convickt: temp/convickt.o temp/clc-cset.o temp/uncommon.o
	$(CC) temp/convickt.o temp/clc-cset.o temp/uncommon.o $(LDFLAGS) -o bin/convickt

temp/oil.c: src/oil.y temp/config.h
	$(YACC) $(YACCFLAGS) src/oil.y
	-rm -f temp/oil.c
	mv y.tab.c temp/oil.c

temp/parser.h temp/parser.c: src/parser.y src/ick.h src/sizes.h src/feh.h src/ick_lose.h temp/config.h
	$(YACC) -d $(YACCFLAGS) src/parser.y
	-rm -f temp/parser.c
	mv y.tab.c temp/parser.c
	-rm -f temp/parser.h
	mv y.tab.h temp/parser.h

temp/lexer.c: src/lexer.l src/ick.h src/ick_lose.h temp/config.h
	$(LEX) $(LEXFLAGS) src/lexer.l
	-rm -f temp/lexer.c
	mv lex.yy.c temp/lexer.c

bin/lextest: temp/parser.h src/lexer.c src/ick.h temp/ick_lose.o
	$(CC) -DMAIN src/lexer.c src/ick_lose.o -lfl -o bin/lextest

temp/oil: temp/oil.c temp/config.h
	$(CC) $(CFLAGS) -o temp/oil temp/oil.c
temp/bin2c: src/bin2c.c
	$(CC) $(CFLAGS) -o temp/bin2c src/bin2c.c
temp/parser.o: temp/parser.c src/ick.h src/feh.h src/ick_lose.h src/sizes.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/parser.o temp/parser.c
temp/lexer.o: temp/lexer.c temp/parser.h src/ick.h src/ick_lose.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/lexer.o temp/lexer.c
temp/feh2.o: src/feh2.c src/feh.h temp/parser.h src/ick.h src/fiddle.h src/ick_lose.h src/sizes.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/feh2.o src/feh2.c
temp/idiotism.c: src/idiotism.oil temp/oil
	temp/oil < src/idiotism.oil > temp/idiotism.c
# There is a situation that some operating systems can get into known as memory
# overcommit. Linux (talking specifically about the kernel) can reach this
# situation in its default configuration, where it has been reporting to malloc
# that memory is available when in fact it isn't, and reaches the situation when
# the computer is actually completely out of memory but programs on it believe
# that they aren't. For some reason, using the default options, gcc can somehow
# cause memory overcommit when acting on a file as big as idiotism.c. The
# solution here, therefore, is to put two protections to prevent this happening;
# first, turn off optimisation specifically for that file, and second, put an
# artificial memory limit on the compilation so that no more than 128 megabytes
# can be allocated, using the ulimit feature on bash. As memory overcommit can
# make a system unusable, and as it can be preceded by 'thrashing' (hugely
# rapid swaps to disk in an attempt to get more memory) that can also make a
# system unusable, steps to avoid this are necessary.
temp/idiotism.o: temp/idiotism.c src/feh.h temp/parser.h src/ick.h src/fiddle.h src/ick_lose.h src/sizes.h temp/config.h
	bash -c 'ulimit -Sv 131072 && ulimit -Sd 131072 && $(CC) $(CFLAGS) -c -o temp/idiotism.o -O0 temp/idiotism.c'
temp/dekludge.o: src/dekludge.c src/feh.h temp/parser.h src/ick.h src/fiddle.h src/ick_lose.h src/sizes.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/dekludge.o src/dekludge.c
temp/perpet.o: src/perpet.c temp/parser.h src/ick.h src/ick_lose.h src/sizes.h src/uncommon.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/perpet.o src/perpet.c
temp/cesspool.o: src/cesspool.c src/abcess.h src/ick_lose.h src/sizes.h
	$(CC) $(CFLAGS) -c -o temp/cesspool.o src/cesspool.c
temp/numerals.o: src/numerals.c src/abcess.h src/ick_lose.h src/sizes.h
	$(CC) $(CFLAGS) -c -o temp/numerals.o src/numerals.c
temp/unravel.o: src/unravel.c src/abcess.h src/ick_lose.h src/sizes.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/unravel.o src/unravel.c
temp/ick_lose.o: src/ick_lose.c src/ick_lose.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/ick_lose.o src/ick_lose.c
temp/fiddle.o: src/fiddle.c src/fiddle.h src/sizes.h
	$(CC) $(CFLAGS) -c -o temp/fiddle.o src/fiddle.c
temp/arrgghh.o: src/arrgghh.c
	$(CC) $(CFLAGS) -c -o temp/arrgghh.o src/arrgghh.c
temp/uncommon.o: src/uncommon.c src/uncommon.h
	$(CC) $(CFLAGS) -c -o temp/uncommon.o src/uncommon.c
temp/ick_ec.o: src/ick_ec.c src/ick_ec.h src/abcess.h
	$(CC) $(CFLAGS) -c -o temp/ick_ec.o src/ick_ec.c
temp/yuk.o: src/yuk.c src/yuk.h src/ick_lose.h src/sizes.h src/abcess.h src/uncommon.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/yuk.o src/yuk.c
temp/clc-cset.o: src/clc-cset.c src/uncommon.h
	$(CC) $(CFLAGS) -c -o temp/clc-cset.o src/clc-cset.c
temp/convickt.o: src/convickt.c
	$(CC) $(CFLAGS) -c -o temp/convickt.o src/convickt.c

temp/cset-a.c: temp/bin2c src/atari.bin
	temp/bin2c ick_clc_cset_atari < src/atari.bin > temp/cset-a.c
temp/cset-b.c: temp/bin2c src/baudot.bin
	temp/bin2c ick_clc_cset_baudot < src/baudot.bin > temp/cset-b.c
temp/cset-e.c: temp/bin2c src/ebcdic.bin
	temp/bin2c ick_clc_cset_ebcdic < src/ebcdic.bin > temp/cset-e.c
temp/cset-l.c: temp/bin2c src/latin1.bin
	temp/bin2c ick_clc_cset_latin1 < src/latin1.bin > temp/cset-l.c
temp/cset-a.o: temp/cset-a.c
	$(CC) $(CFLAGS) -c -o temp/cset-a.o temp/cset-a.c
temp/cset-b.o: temp/cset-b.c
	$(CC) $(CFLAGS) -c -o temp/cset-b.o temp/cset-b.c
temp/cset-e.o: temp/cset-e.c
	$(CC) $(CFLAGS) -c -o temp/cset-e.o temp/cset-e.c
temp/cset-l.o: temp/cset-l.c
	$(CC) $(CFLAGS) -c -o temp/cset-l.o temp/cset-l.c

lib/libick.a: temp/cesspool.o temp/numerals.o temp/ick_lose.o temp/fiddle.o temp/arrgghh.o temp/clc-cset.o temp/cset-a.o temp/cset-b.o temp/cset-e.o temp/cset-l.o temp/uncommon.o
	ar cr lib/libick.a temp/cesspool.o temp/numerals.o temp/ick_lose.o temp/fiddle.o temp/arrgghh.o temp/clc-cset.o temp/cset-a.o temp/cset-b.o temp/cset-e.o temp/cset-l.o temp/uncommon.o
	ranlib lib/libick.a

lib/libickmt.a: temp/cesspool.o temp/numerals.o temp/unravel.o temp/ick_lose.o temp/fiddle.o temp/arrgghh.o temp/clc-cset.o temp/cset-a.o temp/cset-b.o temp/cset-e.o temp/cset-l.o temp/uncommon.o
	ar cr lib/libickmt.a temp/cesspool.o temp/numerals.o temp/unravel.o temp/ick_lose.o temp/fiddle.o temp/arrgghh.o temp/clc-cset.o temp/cset-a.o temp/cset-b.o temp/cset-e.o temp/cset-l.o temp/uncommon.o
	ranlib lib/libickmt.a

lib/libickec.a: temp/cesspool.o temp/numerals.o temp/ick_ec.o temp/ick_lose.o temp/fiddle.o temp/arrgghh.o temp/clc-cset.o temp/cset-a.o temp/cset-b.o temp/cset-e.o temp/cset-l.o temp/uncommon.o
	ar cr lib/libickec.a temp/cesspool.o temp/numerals.o temp/ick_ec.o temp/ick_lose.o temp/fiddle.o temp/arrgghh.o temp/clc-cset.o temp/cset-a.o temp/cset-b.o temp/cset-e.o temp/cset-l.o temp/uncommon.o
	ranlib lib/libickec.a

lib/libyuk.a: temp/yuk.o
	ar cr lib/libyuk.a temp/yuk.o
	ranlib lib/libyuk.a

install-common: all
	cp bin/ick bin/convickt $(DESTDIR)$(bindir)
	cp lib/libick.a $(DESTDIR)$(libdir)
	cp lib/libickmt.a $(DESTDIR)$(libdir)
	cp lib/libickec.a $(DESTDIR)$(libdir)
	cp lib/libyuk.a $(DESTDIR)$(libdir)
	-mkdir -p $(DESTDIR)$(incdir)
	cp src/ick_lose.h src/abcess.h src/fiddle.h src/yuk.h $(DESTDIR)$(incdir)
	cp src/ick_ec.h $(DESTDIR)$(incdir)
	-mkdir -p $(DESTDIR)$(datadir)
	cp include/*.bin $(DESTDIR)$(datadir)
	cp src/ick-wrap.c src/pickwrap.c $(DESTDIR)$(datadir)
	cp include/syslib.i include/syslib.?i $(DESTDIR)$(datadir)
	cp lib/*.c $(DESTDIR)$(datadir)
	cp temp/coopt.sh $(DESTDIR)$(datadir)
	cp COPYING.txt $(DESTDIR)$(datadir)
	-mkdir -p $(DESTDIR)$(mandir)/man1
	cp doc/ick.1 doc/convickt.1 $(DESTDIR)$(mandir)/man1
	-mkdir -p $(DESTDIR)$(infodir)
	rm -f $(DESTDIR)$(infodir)/ick.info
	cp doc/ick.in* $(DESTDIR)$(infodir)
	mv $(DESTDIR)$(infodir)/ick.inf $(DESTDIR)$(infodir)/ick.info
# Note that it's necessary to copy the info file to .inf on an 8.3 system
# and .info otherwise; this is done by copying it to .inf, and then renaming
# to .info; on an 8.3 system, this leaves the extension as .inf because mv
# will signal an (unimportant) error. Also, the info directory has to
# be created first if it doesn't exist because /usr/local/share/info
# (the default) doesn't exist by default in Debian.

# install, and gzip the docs after installing.
# The pattern i*[fo] matches ick.info only when uncompressed, even on
# an 8.3 system. ick.i*gz matches compressed Info files, but some DOS
# command interpreters buggily also match it to uncompressed Info files
# even though gz appears nowhere in the filename, so care must be taken
# only to use that pattern with sh.
install: install-common
	sh -c "rm -f $(DESTDIR)$(infodir)/ick.i*gz"
	-gzip -9 -f $(DESTDIR)$(infodir)/ick.in*
	-gzip -9 -f $(DESTDIR)$(mandir)/man1/ick.1
	-gzip -9 -f $(DESTDIR)$(mandir)/man1/convickt.1
	sh -c "mandb -q"
	sh -c "(test -f $(DESTDIR)$(infodir)/ick.info.gz && install-info --quiet --dir-file=$(infodirfile) $(DESTDIR)$(infodir)/ick.info.gz) || true"
	sh -c "(test -f $(DESTDIR)$(infodir)/ick.igz && install-info --quiet --dir-file=$(infodirfile) $(DESTDIR)$(infodir)/ick.igz) || true"

# install without gzipping the docs
install-nozip: install-common
	sh -c "mandb -q"
	sh -c "(test -f $(DESTDIR)$(infodir)/ick.info && install-info --quiet --dir-file=$(infodirfile) $(DESTDIR)$(infodir)/ick.info) || true"
	sh -c "(test -f $(DESTDIR)$(infodir)/ick.inf && install-info --quiet --dir-file=$(infodirfile) $(DESTDIR)$(infodir)/ick.inf) || true"


uninstall:
	sh -c "install-info --quiet --remove --dir-file=$(infodirfile) ick"
	rm -f $(DESTDIR)$(bindir)/ick $(DESTDIR)$(libdir)/libick.a
	rm -f $(DESTDIR)$(bindir)/convickt
	rm -f $(DESTDIR)$(libdir)/libickmt.a $(DESTDIR)$(libdir)/libyuk.a
	rm -f ${DESTDIR}${infodir}/ick.i*
	rm -f $(DESTDIR)$(mandir)/man1/ick.1*
	rm -f $(DESTDIR)$(mandir)/man1/convickt.1*
	rm -fr $(DESTDIR)$(incdir) $(DESTDIR)$(datadir)
	sh -c "mandb -q"

TAGS: $(SOURCES)
	etags $(SOURCES)

dist: ick-0.28.tgz

ick-0.28.tgz: $(SOURCES)
	@find $(SOURCES) -type f -print | grep -v RCS | sed s:^:ick-0.28/: >MANIFEST.txt
	@(cd ..; ln -s intercal ick-0.28)
	(cd ..; tar -czvf intercal/ick-0.28.tgz `cat intercal/MANIFEST.txt`)
	@(cd ..; rm ick-0.28)

clean:
	-cp temp/config.h temp/coopt.sh .
	-rm -f temp/* bin/* lib/* include/*
	mv config.h coopt.sh temp

distr:
	-rm -f temp/*.o temp/parser.h temp/oil
	-rm -f temp/idiotism.c temp/bin2c temp/cset-?.c
	-rm -f bin/* lib/* include/*

distclean: distr
	-rm -f config.status config.cache config.log Makefile
	-rm -f temp/config.h temp/coopt.sh

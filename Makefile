# Generated automatically from Makefile.in by configure.
#
# Productions for the INTERCAL distribution
#
VERSION=0.27

# Common prefix for machine-independent installed files.
prefix = /dev/env/DJDIR
# Common prefix for machine-dependent installed files.
exec_prefix = ${prefix}

# Directory in which to install binaries.
bindir = ${exec_prefix}/bin
# Directory in which to install libraries.
libdir = ${exec_prefix}/lib
# Directory to install data files.
datadir = ${prefix}/share/ick-0.27
# Directory to install the include files in.
incdir = ${prefix}/include/ick-0.27
# Directory to install Info files in.
infodir = ${prefix}/info

# This can be overridden from the command line
DESTDIR=

CC = gcc
CFLAGS = -g -O2 -DICKINCLUDEDIR=\"$(incdir)\" -DICKLIBDIR=\"$(datadir)\" -DICKBINDIR=\"$(bindir)\" -DYYDEBUG -O2 -W -Wall -I./src -I./temp
LDFLAGS = 
LEX = flex
LEXFLAGS=
YACC = bison -y
YACCFLAGS = 

LOADLIBS =  -lfl

CSOURCES = src/parser.y src/lexer.l src/feh2.c src/dekludge.c src/lose.c src/fiddle.c src/perpet.c src/unravel.c src/uncommon.c
ISOURCES = src/cesspool.c src/arrgghh.c src/ick-wrap.c src/numerals.c
YSOURCES = src/yuk.c
HEADERS = src/ick.h src/lose.h src/sizes.h src/abcess.h src/fiddle.h src/feh.h src/yuk.h src/uncommon.h
BUILD = Makefile.in config.in config.sh src/configh.in ickspec.in ickspec src/cooptsh.in
MISC = etc/intercal.el src/ctrlmfix.sh src/ctrlmfix.bat
DOCS = BUGS.txt COPYING.txt NEWS.txt README.txt
SOURCES = $(CSOURCES) $(ISOURCES) $(YSOURCES) $(HEADERS) $(BUILD) $(MISC) $(DOCS) pit doc

all: bin/ick.exe lib/libick.a lib/libickmt.a lib/libyuk.a ial

ial: src/fiddle.h src/abcess.h src/lose.h src/yuk.h src/ick-wrap.c src/pickwrap.c temp/coopt.sh pit/lib/syslib.i COPYING.txt
	-cp src/fiddle.h include/fiddle.h
	-cp src/abcess.h include/abcess.h
	-cp src/lose.h include/lose.h
	-cp src/yuk.h include/yuk.h
	-cp pit/lib/syslib.i pit/lib/syslib.?i include
	-cp src/ick-wrap.c lib/ick-wrap.c
	-cp src/pickwrap.c lib/pickwrap.c
	-cp temp/coopt.sh lib/coopt.sh
	-cp COPYING.txt lib/COPYING.txt

bin/ick.exe: temp/parser.o temp/lexer.o temp/feh2.o temp/dekludge.o temp/idiotism.o temp/lose.o temp/fiddle.o temp/perpet.o temp/uncommon.o
	$(CC) temp/perpet.o temp/parser.o temp/lexer.o temp/feh2.o temp/dekludge.o temp/idiotism.o temp/lose.o temp/fiddle.o temp/uncommon.o $(LDFLAGS) -o bin/ick.exe

temp/oil.c: src/oil.y temp/config.h
	$(YACC) $(YACCFLAGS) src/oil.y
	-rm -f temp/oil.c
	mv y.tab.c temp/oil.c

temp/parser.h temp/parser.c: src/parser.y src/ick.h src/sizes.h src/feh.h src/lose.h temp/config.h
	$(YACC) -d $(YACCFLAGS) src/parser.y
	-rm -f temp/parser.c
	mv y.tab.c temp/parser.c
	-rm -f temp/parser.h
	mv y.tab.h temp/parser.h

temp/lexer.c: src/lexer.l src/ick.h src/lose.h temp/config.h
	$(LEX) $(LEXFLAGS) src/lexer.l
	-rm -f temp/lexer.c
	mv lexyy.c temp/lexer.c 

bin/lextest: temp/parser.h src/lexer.c src/ick.h temp/lose.o
	$(CC) -DMAIN src/lexer.c src/lose.o -lfl -o bin/lextest

temp/oil.exe: temp/oil.c temp/config.h
	$(CC) $(CFLAGS) -o temp/oil.exe temp/oil.c
temp/parser.o: temp/parser.c src/ick.h src/feh.h src/lose.h src/sizes.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/parser.o temp/parser.c
temp/lexer.o: temp/lexer.c temp/parser.h src/ick.h src/lose.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/lexer.o temp/lexer.c
temp/feh2.o: src/feh2.c src/feh.h temp/parser.h src/ick.h src/fiddle.h src/lose.h src/sizes.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/feh2.o src/feh2.c
temp/idiotism.c: src/idiotism.oil temp/oil.exe
	temp/oil.exe < src/idiotism.oil > temp/idiotism.c
temp/idiotism.o: temp/idiotism.c src/feh.h temp/parser.h src/ick.h src/fiddle.h src/lose.h src/sizes.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/idiotism.o temp/idiotism.c
temp/dekludge.o: src/dekludge.c src/feh.h temp/parser.h src/ick.h src/fiddle.h src/lose.h src/sizes.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/dekludge.o src/dekludge.c
temp/perpet.o: src/perpet.c temp/parser.h src/ick.h src/lose.h src/sizes.h src/uncommon.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/perpet.o src/perpet.c
temp/cesspool.o: src/cesspool.c src/abcess.h src/lose.h src/sizes.h
	$(CC) $(CFLAGS) -c -o temp/cesspool.o src/cesspool.c
temp/numerals.o: src/numerals.c src/abcess.h src/lose.h src/sizes.h
	$(CC) $(CFLAGS) -c -o temp/numerals.o src/numerals.c
temp/unravel.o: src/unravel.c src/abcess.h src/lose.h src/sizes.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/unravel.o src/unravel.c
temp/lose.o: src/lose.c src/lose.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/lose.o src/lose.c
temp/fiddle.o: src/fiddle.c src/fiddle.h src/sizes.h
	$(CC) $(CFLAGS) -c -o temp/fiddle.o src/fiddle.c
temp/arrgghh.o: src/arrgghh.c
	$(CC) $(CFLAGS) -c -o temp/arrgghh.o src/arrgghh.c
temp/uncommon.o: src/uncommon.c src/uncommon.h
	$(CC) $(CFLAGS) -c -o temp/uncommon.o src/uncommon.c
temp/yuk.o: src/yuk.c src/yuk.h src/lose.h src/sizes.h src/abcess.h src/uncommon.h temp/config.h
	$(CC) $(CFLAGS) -c -o temp/yuk.o src/yuk.c

lib/libick.a: temp/cesspool.o temp/numerals.o temp/lose.o temp/fiddle.o temp/arrgghh.o
	ar cr lib/libick.a temp/cesspool.o temp/numerals.o temp/lose.o temp/fiddle.o temp/arrgghh.o
	ranlib lib/libick.a

lib/libickmt.a: temp/cesspool.o temp/numerals.o temp/unravel.o temp/lose.o temp/fiddle.o temp/arrgghh.o
	ar cr lib/libickmt.a temp/cesspool.o temp/numerals.o temp/unravel.o temp/lose.o temp/fiddle.o temp/arrgghh.o
	ranlib lib/libickmt.a

lib/libyuk.a: temp/yuk.o temp/uncommon.o
	ar cr lib/libyuk.a temp/yuk.o temp/uncommon.o
	ranlib lib/libyuk.a

install: all
	cp bin/ick.exe $(DESTDIR)$(bindir)
	cp lib/libick.a $(DESTDIR)$(libdir)
	cp lib/libickmt.a $(DESTDIR)$(libdir)
	cp lib/libyuk.a $(DESTDIR)$(libdir)
	-mkdir -p $(DESTDIR)$(incdir)
	cp src/lose.h src/abcess.h src/fiddle.h src/yuk.h $(DESTDIR)$(incdir)
	-mkdir $(DESTDIR)$(datadir)
	cp src/ick-wrap.c $(DESTDIR)$(datadir)
	cp include/syslib.i include/syslib.?i $(DESTDIR)$(datadir)
	cp temp/coopt.sh $(DESTDIR)$(libdir)
	cp COPYING.txt $(DESTDIR)$(libdir)
	rm -f $(DESTDIR)$(infodir)/ick.info
	cp doc/ick.inf $(DESTDIR)$(infodir)
	-mv $(DESTDIR)$(infodir)/ick.inf $(DESTDIR)$(infodir)/ick.info
# Note that it's necessary to copy the info file to .inf on an 8.3 system
# and .info otherwise; this is done by copying it to .inf, and then renaming
# to .info; on an 8.3 system, this leaves the extension as .inf because mv
# will signal an (unimportant) error.

uninstall:
	rm -f $(DESTDIR)$(bindir)/ick.exe $(DESTDIR)$(libdir)/libick.a
	rm -f $(DESTDIR)$(libdir)/coopt.sh $(DESTDIR)$(libdir)/COPYING.txt
	rm -f $(DESTDIR)$(libdir)/libickmt.a $(DESTDIR)$(libdir)/libyuk.a
	rm -f ${DESTDIR}${infodir}/ick.inf $(DESTDIR)$(infodir)/ick.info
	rm -fr $(DESTDIR)$(incdir) $(DESTDIR)$(datadir)

TAGS: $(SOURCES)
	etags $(SOURCES)

dist: ick-0.27.tgz

ick-0.27.tgz: $(SOURCES)
	@find $(SOURCES) -type f -print | grep -v RCS | sed s:^:ick-0.27/: >MANIFEST.txt
	@(cd ..; ln -s intercal ick-0.27)
	(cd ..; tar -czvf intercal/ick-0.27.tgz `cat intercal/MANIFEST.txt`)
	@(cd ..; rm ick-0.27)

clean:
	-cp temp/config.h .
	-rm -f temp/* bin/* lib/* include/*
	mv config.h temp

distr:
	-rm -f temp/*.o temp/parser.h temp/oil.exe temp/idiotism.c
	-rm -f bin/* lib/* include/*

distclean: distr
	-rm -f config.status config.cache config.log Makefile
	-rm -f temp/config.h temp/coopt.sh

#
# Productions for the INTERCAL distribution
#

# Common prefix for machine-independent installed files.
prefix = @prefix@
# Common prefix for machine-dependent installed files.
exec_prefix = @exec_prefix@

# Directory in which to install binaries.
bindir = @bindir@
# Directory in which to install libraries.
libdir = @libdir@
# Directory to install data files.
datadir = @datadir@/intercal-@PACKAGE_VERSION@
# Directory to install the include files in.
incdir = @includedir@/intercal-@PACKAGE_VERSION@

# Program to install binaries
INSTALL_PROGRAM = @INSTALL_PROGRAM@
# Program to install the man page.
INSTALL_DATA = @INSTALL_DATA@
# Generic install program.
INSTALL = @INSTALL@

CC = @CC@
CFLAGS = @CFLAGS@ -DICKINCLUDEDIR=\"$(incdir)\" -DICKLIBDIR=\"$(datadir)\" -DYYDEBUG
LDFLAGS = @LDFLAGS@
LEX = @LEX@
LEXFLAGS=
YACC = @YACC@
YACCFLAGS = -d # -t

LOADLIBS = @LIBS@ @INTLLIBS@ @LEXLIB@

CSOURCES = ick.y lexer.l feh.c lose.c fiddle.c perpetrate.c
ISOURCES = cesspool.c arrgghh.c ick-wrap.c
HEADERS = ick.h lose.h sizes.h abcess.h fiddle.h feh.h
MISC = configure.ac config.h.in specgen.sh
SOURCES = $(CSOURCES) $(ISOURCES) $(HEADERS) $(MISC)

all: ick libick.a

ick: ick.o lexer.o feh.o lose.o fiddle.o perpetrate.o
	$(CC) perpetrate.o ick.o lexer.o feh.o lose.o fiddle.o $(LDFLAGS) -o ick

y.tab.h ick.c: ick.y ick.h sizes.h feh.h lose.h
	$(YACC) -d $(YACCFLAGS) ick.y
	mv y.tab.c ick.c

lexer.c: lexer.l ick.h lose.h
	$(LEX) $(LEXFLAGS) lexer.l
	mv lex.yy.c lexer.c 

# Uncomment the following if using a stock lex
# LEXLIBS = -ll
lextest: y.tab.h lexer.c ick.h lose.o
	$(CC) -DMAIN lexer.c lose.o $(LEXLIBS) -o lextest

ick.o: ick.c ick.h feh.h lose.h sizes.h
lexer.o: lexer.c y.tab.h ick.h lose.h
feh.o: feh.c feh.h y.tab.h ick.h fiddle.h lose.h sizes.h
perpetrate.o: perpetrate.c y.tab.h ick.h lose.h sizes.h
cesspool.o: cesspool.c numerals.c abcess.h lose.h sizes.h
lose.o: lose.h
fiddle.o: fiddle.h sizes.h

libick.a: cesspool.o lose.o fiddle.o arrgghh.o
	ar cr libick.a cesspool.o lose.o fiddle.o arrgghh.o
	-ranlib libick.a

install: all
	cp ick $(bindir)
	cp libick.a $(libdir)
	-mkdir -p $(incdir)
	cp lose.h abcess.h fiddle.h $(incdir)
	-mkdir $(datadir)
	(cp -r pit $(datadir)/pit)
	(cp -r lib $(datadir)/lib)
	cp ick-wrap.c $(datadir)

uninstall:
	rm -f $(bindir)/ick $(libdir)/libick.a
	rm -f $(incdir) $(datadir)

TAGS: $(SOURCES)
	etags $(SOURCES)

dist: intercal-@PACKAGE_VERSION@.tar.gz

intercal-@PACKAGE_VERSION@.tar.gz:
	sed "s:^:intercal-@PACKAGE_VERSION@/:" <MANIFEST >manifest
	(cd ..; ln -s intercal intercal-@PACKAGE_VERSION@)
	(cd ..; tar -czvf intercal/intercal-@PACKAGE_VERSION@.tar.gz `cat intercal/manifest`)
	rm ../intercal-@PACKAGE_VERSION@ manifest

intercal.spec:
	specgen.sh @PACKAGE_VERSION@ >intercal.spec

RPMROOT=/usr/src/redhat
RPM = rpmbuild
RPMFLAGS = -ba
rpm: dist
	cp intercal-@PACKAGE_VERSION@.tar.gz $(RPMROOT)/SOURCES;
	cp intercal.spec $(RPMROOT)/SPECS
	cd $(RPMROOT)/SPECS; $(RPM) $(RPMFLAGS) intercal.spec	
	cp $(RPMROOT)/RPMS/`arch|sed 's/i[4-9]86/i386/'`/intercal-@PACKAGE_VERSION@*.rpm .
	cp $(RPMROOT)/SRPMS/intercal-@PACKAGE_VERSION@*.src.rpm .

clean:
	rm -f yacc.* y.tab.* y.output *~ *.[oa] lexer.c ick.c lextest ick
	rm -f intercal-@PACKAGE_VERSION@.shar intercal-@PACKAGE_VERSION@.tar intercal-@PACKAGE_VERSION@.tar.gz intercal.spec


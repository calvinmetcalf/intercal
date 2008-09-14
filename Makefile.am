ICK_SPECIFIC_SUBDIR = ick-@PACKAGE_VERSION@
ickincludedir = $(includedir)/$(ICK_SPECIFIC_SUBDIR)
ickdatadir = $(datadir)/$(ICK_SPECIFIC_SUBDIR)

AM_CFLAGS = -DICKINCLUDEDIR=\"$(ickincludedir)\" \
            -DICKDATADIR=\"$(ickdatadir)\" \
            -DICKBINDIR=\"$(bindir)\" -DICKLIBDIR=\"$(libdir)\" \
            -DYYDEBUG -DICK_HAVE_STDINT_H=@HAVE_STDINT_H@
AM_CPPFLAGS = -I$(srcdir)/src

bin_PROGRAMS = ick convickt
ick_SOURCES = src/feh2.c src/dekludge.c src/ick_lose.c \
              src/fiddle.c src/perpet.c src/uncommon.c
nodist_ick_SOURCES = parser.y lexer.l
parser.y : src/parser.y
	$(LN_S) `test -f 'src/parser.y' || echo '$(srcdir)/'`src/parser.y parser.y
lexer.l : src/lexer.l
	$(LN_S) `test -f 'src/lexer.l' || echo '$(srcdir)/'`src/lexer.l lexer.l
ick_LDADD = libidiot.a
AM_YFLAGS = -d
BUILT_SOURCES = parser.h
noinst_LIBRARIES = libidiot.a
libidiot_a_SOURCES = src/idiotism.oil
libidiot.a: src/idiotism.oil oil$(EXEEXT) src/oil.h src/sizes.h src/ick.h \
	    parser.h src/fiddle.h src/ick_lose.h src/feh.h
	./oil$(EXEEXT) < `test -f 'src/idiotism.oil' || echo '$(srcdir)/'`src/idiotism.oil
	for oo in oilout*.c; do echo $$oo; $(COMPILE) -c $$oo; done
	$(AR) $(ARFLAGS) libidiot.a oilout*.o
	$(RANLIB) libidiot.a
convickt_SOURCES = src/convickt.c src/clc-cset.c src/uncommon.c

noinst_PROGRAMS = oil bin2c
oil_SOURCES = src/oil.y
oil_YFLAGS =
bin2c_SOURCES = src/bin2c.c

lib_LIBRARIES = libick.a libickmt.a libickec.a libyuk.a
libick_a_SOURCES = src/cesspool.c src/numerals.c src/ick_lose.c src/fiddle.c \
                   src/arrgghh.c src/clc-cset.c src/uncommon.c
libick_a_LIBADD = atari.o baudot.o ebcdic.o latin1.o
libickmt_a_SOURCES = src/cesspool.c src/numerals.c src/ick_lose.c src/fiddle.c \
                     src/arrgghh.c src/clc-cset.c src/uncommon.c src/unravel.c
libickmt_a_LIBADD = atari.o baudot.o ebcdic.o latin1.o
libickec_a_SOURCES = src/cesspool.c src/numerals.c src/ick_lose.c src/fiddle.c \
                     src/arrgghh.c src/clc-cset.c src/uncommon.c src/ick_ec.c
libickec_a_LIBADD = atari.o baudot.o ebcdic.o latin1.o
libyuk_a_SOURCES = src/yuk.c
atari.o: bin2c$(EXEEXT) src/atari.bin
	./bin2c$(EXEEXT) ick_clc_cset_atari < `test -f 'src/atari.bin' || echo '$(srcdir)/'`src/atari.bin > atari.c
	$(COMPILE) -c atari.c
baudot.o: bin2c$(EXEEXT) src/baudot.bin
	./bin2c$(EXEEXT) ick_clc_cset_baudot < `test -f 'src/baudot.bin' || echo '$(srcdir)/'`src/baudot.bin > baudot.c
	$(COMPILE) -c baudot.c
ebcdic.o: bin2c$(EXEEXT) src/ebcdic.bin
	./bin2c$(EXEEXT) ick_clc_cset_ebcdic < `test -f 'src/ebcdic.bin' || echo '$(srcdir)/'`src/ebcdic.bin > ebcdic.c
	$(COMPILE) -c ebcdic.c
latin1.o: bin2c$(EXEEXT) src/latin1.bin
	./bin2c$(EXEEXT) ick_clc_cset_latin1 < `test -f 'src/latin1.bin' || echo '$(srcdir)/'`src/latin1.bin > latin1.c
	$(COMPILE) -c latin1.c

ickinclude_HEADERS = src/ick_ec.h src/fiddle.h src/abcess.h src/ick_lose.h \
                     src/yuk.h
dist_ickdata_DATA = src/atari.bin src/baudot.bin src/ebcdic.bin src/latin1.bin \
                    src/ick-wrap.c src/pickwrap.c COPYING
# These necessary pit files are in nodist_ here because they're in EXTRA_DIST
# later along with the rest of the pit.
nodist_ickdata_DATA = pit/lib/syslib.i pit/lib/syslib.3i pit/lib/syslib.4i \
                      pit/lib/syslib.5i pit/lib/syslib.6i pit/lib/syslib.7i \
                      coopt.sh pit/explib/syslibc.c pit/explib/compunex.c \
                      pit/explib/ecto_b98.c
dist_man1_MANS = doc/ick.1 doc/convickt.1
info_TEXINFOS = doc/ick.txi

EXTRA_DIST = pit etc BUGS src/abcess.h src/feh.h src/fiddle.h src/ick_ec.h \
             src/ick.h src/ick_lose.h src/oil.h src/pick1.h src/pick2.h \
             src/sizes.h src/uncommon.h src/yuk.h src/parser.y src/lexer.l \
             doc/Makefile doc/fdl-1-2.txi doc/fix83.pl doc/fixtoc.pl \
             doc/ickstyle.css doc/ick.txt doc/intercal.mm doc/READ.ME \
             doc/THEORY.txt doc/tidy.cfg parser.c parser.h \
             lexer.c doc/ick.txi doc/ick.txt NEWS README

# It's ugly to distribute the prebuilt files in the root of the
# distribution.  configure will move these files back again into the
# root of the /build tree/, which makes a lot more sense than putting
# them in the root of the source tree.
dist-hook:
	$(MKDIR_P) $(distdir)/prebuilt
	mv $(distdir)/parser.c $(distdir)/prebuilt
	mv $(distdir)/parser.h $(distdir)/prebuilt
	mv $(distdir)/lexer.c $(distdir)/prebuilt
	mv $(distdir)/oil-oil.c $(distdir)/prebuilt
	(cd $(distdir); ls -1R > MANIFEST)

# N.B. the parser.y and lexer.l are the links made to simplify the
# build, not the originals
CLEANFILES = oilout*.c parser.y lexer.l \
             atari.c baudot.c ebcdic.c latin1.c ick.dvi
# Distclean deletes the Makefile and thus requires reconfiguring,
# and therefore this is a safe moment to delete the build tree
# versions of the prebuilt files. (The source tree versions will
# be safe in the prebuilt dir, which Automake doesn't even know
# about.)
DISTCLEANFILES = parser.c parser.h lexer.c oil-oil.c
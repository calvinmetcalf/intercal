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
ick-wrap.c : src/ick-wrap.c
	$(LN_S) `test -f 'src/ick-wrap.c' || echo '$(srcdir)/'`src/ick-wrap.c ick-wrap.c
pickwrap.c : src/pickwrap.c
	$(LN_S) `test -f 'src/pickwrap.c' || echo '$(srcdir)/'`src/pickwrap.c pickwrap.c
syslib.i : pit/lib/syslib.i
	$(LN_S) `test -f 'pit/lib/syslib.i' || echo '$(srcdir)/'`pit/lib/syslib.i syslib.i
syslib3.3i : pit/lib/syslib3.3i
	$(LN_S) `test -f 'pit/lib/syslib.3i' || echo '$(srcdir)/'`pit/lib/syslib3.3i syslib3.3i
syslib4.4i : pit/lib/syslib4.4i
	$(LN_S) `test -f 'pit/lib/syslib.4i' || echo '$(srcdir)/'`pit/lib/syslib4.4i syslib4.4i
syslib5.5i : pit/lib/syslib5.5i
	$(LN_S) `test -f 'pit/lib/syslib.5i' || echo '$(srcdir)/'`pit/lib/syslib5.5i syslib5.5i
syslib6.6i : pit/lib/syslib6.6i
	$(LN_S) `test -f 'pit/lib/syslib.6i' || echo '$(srcdir)/'`pit/lib/syslib6.6i syslib6.6i
syslib7.7i : pit/lib/syslib7.7i
	$(LN_S) `test -f 'pit/lib/syslib.7i' || echo '$(srcdir)/'`pit/lib/syslib7.7i syslib7.7i
floatlib.i : pit/lib/floatlib.i
	$(LN_S) `test -f 'pit/lib/floatlib.i' || echo '$(srcdir)/'`pit/lib/floatlib.i floatlib.i
ick_ec.h : src/ick_ec.h
	$(LN_S) `test -f 'src/ick_ec.h' || echo '$(srcdir)/'`src/ick_ec.h ick_ec.h
ick_lose.h : src/ick_lose.h
	$(LN_S) `test -f 'src/ick_lose.h' || echo '$(srcdir)/'`src/ick_lose.h ick_lose.h
fiddle.h : src/fiddle.h
	$(LN_S) `test -f 'src/fiddle.h' || echo '$(srcdir)/'`src/fiddle.h fiddle.h
abcess.h : src/abcess.h
	$(LN_S) `test -f 'src/abcess.h' || echo '$(srcdir)/'`src/abcess.h abcess.h
yuk.h : src/yuk.h
	$(LN_S) `test -f 'src/ick-ec.h' || echo '$(srcdir)/'`src/yuk.h yuk.h
ick_LDADD = libidiot.a
AM_YFLAGS = -d
# BUILT_SOURCES is used both to make sure parser.h is built early, and to
# create symlinks used by ick to find files if it's run without first being
# installed.
BUILT_SOURCES = parser.h ick-wrap.c pickwrap.c syslib.i syslib3.3i syslib4.4i \
                syslib5.5i syslib6.6i syslib7.7i floatlib.i ick_ec.h fiddle.h abcess.h \
                ick_lose.h yuk.h
noinst_LIBRARIES = libidiot.a
libidiot_a_SOURCES = src/idiotism.oil
libidiot.a: src/idiotism.oil oil$(EXEEXT) src/oil.h src/sizes.h src/ick.h \
	    parser.h src/fiddle.h src/ick_lose.h src/feh.h
	./oil$(EXEEXT) < `test -f 'src/idiotism.oil' || echo '$(srcdir)/'`src/idiotism.oil
	for oo in oilout*.c; do echo $$oo; $(COMPILE) -c $$oo; done
	$(AR) $(ARFLAGS) libidiot.a oilout*.o
	$(RANLIB) libidiot.a
convickt_SOURCES = src/convickt.c src/clc-cset.c src/uncommon.c

@INCLUDE_HIDDEN_FROM_AUTOMAKE@ ./host.mak

noinst_PROGRAMS = oil bin2c
oil_SOURCES = src/oil.y
oil_YFLAGS =
bin2c_SOURCES = src/bin2c.c
oil$(EXEEXT_FOR_BUILD): oil-oil.c bconfig.h
	$(CC_FOR_BUILD) $(DEFAULT_INCLUDES) $(INCLUDES) $(CFLAGS_FOR_BUILD) $(CPPFLAGS_FOR_BUILD) $(LDFLAGS_FOR_BUILD) -o oil$(EXEEXT_FOR_BUILD) oil-oil.c $(LIBS_FOR_BUILD)
bin2c$(EXEEXT_FOR_BUILD): src/bin2c.c bconfig.h
	$(CC_FOR_BUILD) $(DEFAULT_INCLUDES) $(INCLUDES) $(CFLAGS_FOR_BUILD) $(CPPFLAGS_FOR_BUILD) $(LDFLAGS_FOR_BUILD) -o bin2c$(EXEEXT_FOR_BUILD) `test -f 'src/bin2c.c' || echo '$(srcdir)/'`src/bin2c.c $(LIBS_FOR_BUILD)
oil:
bin2c:

# Optional libraries are in lib_LIBRARIES, but with a null build rule.
lib_LIBRARIES = libick.a libickmt.a libickec.a libyuk.a libick_ecto_b98.a
libick_ecto_b98_a_SOURCES =
libick_ecto_b98.a:
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
atari.o: bin2c$(EXEEXT_FOR_BUILD) src/atari.bin
	./bin2c$(EXEEXT_FOR_BUILD) ick_clc_cset_atari < `test -f 'src/atari.bin' || echo '$(srcdir)/'`src/atari.bin > atari.c
	$(COMPILE) -c atari.c
baudot.o: bin2c$(EXEEXT_FOR_BUILD) src/baudot.bin
	./bin2c$(EXEEXT_FOR_BUILD) ick_clc_cset_baudot < `test -f 'src/baudot.bin' || echo '$(srcdir)/'`src/baudot.bin > baudot.c
	$(COMPILE) -c baudot.c
ebcdic.o: bin2c$(EXEEXT_FOR_BUILD) src/ebcdic.bin
	./bin2c$(EXEEXT_FOR_BUILD) ick_clc_cset_ebcdic < `test -f 'src/ebcdic.bin' || echo '$(srcdir)/'`src/ebcdic.bin > ebcdic.c
	$(COMPILE) -c ebcdic.c
latin1.o: bin2c$(EXEEXT_FOR_BUILD) src/latin1.bin
	./bin2c$(EXEEXT_FOR_BUILD) ick_clc_cset_latin1 < `test -f 'src/latin1.bin' || echo '$(srcdir)/'`src/latin1.bin > latin1.c
	$(COMPILE) -c latin1.c

ickinclude_HEADERS = src/ick_ec.h src/fiddle.h src/abcess.h src/ick_lose.h \
                     src/yuk.h
nodist_ickinclude_HEADERS = config.h
dist_ickdata_DATA = src/atari.bin src/baudot.bin src/ebcdic.bin src/latin1.bin \
                    src/ick-wrap.c src/pickwrap.c COPYING
# These necessary pit files are in nodist_ here because they're in EXTRA_DIST
# later along with the rest of the pit.
nodist_ickdata_DATA = pit/lib/syslib.i pit/lib/syslib3.3i pit/lib/syslib4.4i \
                      pit/lib/syslib5.5i pit/lib/syslib6.6i pit/lib/syslib7.7i \
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
             lexer.c doc/ick.txi doc/ick.txt NEWS README \
             buildaux/buildconfig buildaux/buildconfig.ac \
             buildaux/depcomp buildaux/hostmak.in buildaux/install-sh \
             buildaux/missing buildaux/ylwrap src/bconfigh.in

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
	touch $(distdir)/MANIFEST
	(cd $(distdir); find . | sort > MANIFEST)

# Update the prebuilt directory in the original source from the built
# versions; this is for the repository version of C-INTERCAL, so it's
# up to date the same way that distributed versions are.
repoupdate: parser.c parser.h lexer.c oil-oil.c $(srcdir)/MANIFEST
	cp parser.c $(srcdir)/prebuilt/parser.c
	cp parser.h $(srcdir)/prebuilt/parser.h
	cp lexer.c $(srcdir)/prebuilt/lexer.c
	cp oil-oil.c $(srcdir)/prebuilt/oil-oil.c
$(srcdir)/MANIFEST: distdir
	cp $(distdir)/MANIFEST $(srcdir)/MANIFEST
	$(am__remove_distdir)

# N.B. the parser.y, lexer.l, syslib, headers and skeletons are the
# links made to simplify the build, not the originals
CLEANFILES = oilout*.c parser.y lexer.l ick-wrap.c pickwrap.c \
             atari.c baudot.c ebcdic.c latin1.c ick.dvi \
             syslib.i syslib3.3i syslib4.4i syslib5.5i \
             syslib6.6i syslib7.7i ick_ec.h fiddle.h abcess.h \
             ick_lose.h yuk.h
# Distclean deletes the Makefile and thus requires reconfiguring,
# and therefore this is a safe moment to delete the build tree
# versions of the prebuilt files. (The source tree versions will
# be safe in the prebuilt dir, which Automake doesn't even know
# about.)
DISTCLEANFILES = parser.c parser.h lexer.c oil-oil.c host.mak bconfig.h config.status.build

# The build system itself is included in the makefile; tell Automake
# about my tweaking.
$(top_srcdir)/configure:  $(am__configure_deps)
	cd $(top_srcdir) && $(AUTOCONF)
$(top_srcdir)/buildaux/buildconfig: $(top_srcdir)/buildaux/buildconfig.ac $(top_srcdir)/src/bconfigh.in
	cd $(top_srcdir) && $(AUTOCONF) buildaux/buildconfig.ac > buildaux/buildconfig
	chmod a+x $(top_srcdir)/buildaux/buildconfig
$(top_srcdir)/src/bconfigh.in: $(top_srcdir)/buildaux/buildconfig.ac
	cd $(top_srcdir)/buildaux && $(AUTOHEADER) buildconfig.ac
	rm -f stamp-h1
	touch $@
CONFIG_STATUS_DEPENDENCIES=$(top_srcdir)/buildaux/buildconfig $(top_srcdir)/src/bconfigh.in

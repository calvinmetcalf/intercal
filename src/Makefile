# Makefile for the INTERCAL compiler

# Set BINDIR, INCDIR, LIBDIR to the locations where you want "make install"
# to install the executable, include files, and library for the compiler.
# The compiler will look there for the stuff it needs when you compile
# INTERCAL programs.  INCDIR and LIBDIR can be this directory if you don't
# want to "make install", e.g., if you don't trust makefiles moving stuff
# all over creation behind your back.
# (/dev/null may be another good choice :-))

DVMT=/home2/esr/WWW/intercal-0.15

BINDIR = $(DVMT)/bin
INCDIR = $(DVMT)/lib
LIBDIR = $(DVMT)/lib
#INCDIR = $(PWD)
#LIBDIR = $(PWD)

YDEBUG = -vlt
DEBUG	= -g -DDEBUG
# DEBUG = -O
CFLAGS = -I. $(DEBUG) -DICKINCLUDEDIR=\"$(INCDIR)\" \
	-DICKLIBDIR=\"$(LIBDIR)\" -DYYDEBUG
LDFLAGS =

CC = gcc -Wall -W

# You can use lex or flex
#LEX = lex
LEX = flex

# You can use either yacc or bison
#YACC = yacc
YACC = bison -y

CSOURCES = ick.y lexer.l feh.c lose.c fiddle.c perpetrate.c
ISOURCES = cesspool.c arrgghh.c ick-wrap.c
HEADERS = ick.h lose.h sizes.h abcess.h fiddle.h feh.h
SOURCES = $(CSOURCES) $(ISOURCES) $(HEADERS)

all: ick libick.a

ick: ick.o lexer.o feh.o lose.o fiddle.o perpetrate.o
	$(CC) perpetrate.o ick.o lexer.o feh.o lose.o fiddle.o $(LDFLAGS) -o ick

y.tab.h ick.c: ick.y ick.h sizes.h feh.h lose.h
	$(YACC) -d $(YDEBUG) ick.y
	mv y.tab.c ick.c

lexer.c: lexer.l ick.h lose.h
	$(LEX) $(LFLAGS) lexer.l
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
	-mkdir $(BINDIR); rm -f $(BINDIR)/*
	cp ick $(BINDIR)
	-mkdir $(INCDIR)
	(cd $(INCDIR); rm -f lose.h abcess.h fiddle.h ick-wrap.c)
	cp lose.h abcess.h fiddle.h ick-wrap.c $(INCDIR)
	-mkdir $(LIBDIR)
	(cd $(LIBDIR); rm -f libick.a syslib.i)
	cp libick.a ../pit/lib/syslib.i $(LIBDIR)
	-ranlib $(LIBDIR)/libick.a

clean:
	rm -f yacc.* y.tab.* y.output *~ *.[oa] lexer.c ick.c lextest ick

TAGS: $(SOURCES)
	etags $(SOURCES)

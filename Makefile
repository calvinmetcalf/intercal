# Makefile for the INTERCAL compiler

YDEBUG = -v -t
SYSTYPE = -DATT		# -DBSD for Berkeley systems
CFLAGS = -I. $(SYSTYPE) -g
LDFLAGS = -lg

DOCS = READ.ME intercal.man intercal.tex intercal.sty BUGS THEORY
SOURCES = cesspool.[ch] feh.c fiddle.c ick.[yh] lose.[ch] lexer.l

all: ick libick.a

ick: y.tab.o lexer.o feh.o lose.o fiddle.o
	$(CC) y.tab.o lexer.o feh.o lose.o fiddle.o $(LDFLAGS) -o ick

y.tab.h y.tab.c: ick.y
	@echo "expect 13 shift/reduce conflicts"
	yacc -d $(YDEBUG) ick.y

lextest: y.tab.h lexer.c ick.h
	$(CC) -DMAIN lexer.c -ll -o lextest

y.tab.o: y.tab.c ick.h lose.h
lexer.o: lexer.c y.tab.h ick.h
feh.o: feh.c y.tab.h ick.h lose.h
cesspool.o: cesspool.c cesspool.h lose.h
lose.o: lose.h

libick.a: cesspool.o lose.o fiddle.o
	ar cr libick.a cesspool.o lose.o fiddle.o

install:
	cp cesspool.h /usr/include
	cp libick.a /usr/lib
	cp ick /usr/bin

clean:
	rm -f yacc.* y.tab.* y.output *~ *.[oa] lexer.c lextest ick
	rm -f sample.c sample ick.shar Part1.sh Part2.sh

shar:
	shar $(DOCS) Makefile $(SOURCES) ick-wrapper.c sample.i >ick.shar

mailable:
	shar $(DOCS) Makefile >Part1.sh
	shar  $(SOURCES) ick-wrapper.c sample.i >Part2.sh



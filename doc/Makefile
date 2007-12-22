#
# Makefile for the Intercal documentation
#

# Making the original manual:
# To see the Revised Manual, generate intercal.txt or intercal.ps
# To see the original INTERCAL-72 manual, generate original.txt or original.ps.

# Making the Revamped Manual:
# generate ick.inf, ickhtml (will be placed into a directory),
# ick.htm (all in one file), ick.pdf, or ick.txt.

# We make two passes in order to resolve SETR/GETNH references.
# The sed hackery strips the trailing dots from level 1 section references.

x.txt: x.mm
	groff -dc=y -Tascii -p -t -mm x.mm >x.txt

all: allorig allnew

allorig: intercal.ps intercal.txt original.ps original.txt

allnew: ick.inf ickhtml ick.htm ick.txt ick.pdf

intercal.ps: intercal.mm
	groff -dc=y -Tps    -p -t -mm intercal.mm >intercal.refs.tmp
	sed <intercal.refs.tmp -e '/hn \([0-9]*\)\.$$/s//hn \1/' >tmp.refs;
	mv tmp.refs intercal.refs.tmp
	groff -dc=y -Tps    -p -t -mm intercal.mm >intercal.ps
	-rm intercal.refs.qrf

original.ps: intercal.mm
	groff -dc=n -Tps    -p -t -mm intercal.mm >intercal.refs.tmp
	sed <intercal.refs.tmp -e '/hn \([0-9]*\)\.$$/s//hn \1/' >tmp.refs;
	mv tmp.refs intercal.refs.tmp
	groff -dc=n -Tps    -p -t -mm intercal.mm >original.ps
	-rm intercal.refs.qrf

intercal.txt: intercal.mm
	groff -dc=y -Tascii -p -t -mm intercal.mm >/dev/null
	sed <intercal.refs.tmp -e '/hn \([0-9]*\)\.$$/s//hn \1/' >tmp.refs;
	mv tmp.refs intercal.refs.tmp
	GROFF_NO_SGR=1 groff -dc=y -Tascii -p -t -mm intercal.mm >intercal.txt
	-rm intercal.refs.qrf

original.txt: intercal.mm
	groff -dc=n -Tascii -p -t -mm intercal.mm >/dev/null
	sed <intercal.refs.tmp -e '/hn \([0-9]*\)\.$$/s//hn \1/' >tmp.refs;
	mv tmp.refs intercal.refs.tmp
	GROFF_NO_SGR=1 groff -dc=n -Tascii -p -t -mm intercal.mm >original.txt
	-rm intercal.refs.qrf

clean:
	rm -f *.ps *.txt *.refs* ick.inf ick.txt ick.htm ick.pdf
	rm -rf html

ick.inf: ick.txi fdl-1-2.txi
	makeinfo -o ick.inf ick.txi

# The --css-include is skipped here because it seems to cause makeinfo
# to leak open file space, and it fails with an EMFILE error. Instead,
# fix83.pl (which changes filenames to 8.3 format, among other things)
# is used to add the styling. Tidy is run to clean up the output if
# available, but if it isn't, that isn't a problem; there will just be
# a harmless warning.
ickhtml:html/index.htm

html/index.htm: ick.txi ickstyle.css fdl-1-2.txi tidy.cfg
	rm -rf html
	makeinfo --html -o html ick.txi
	ls -1 html | perl -w fix83.pl | sh
	perl -i.tmp -w fix83.pl html/*.htm
	chmod u+w html/*.htm
	-tidy -config tidy.cfg -m html/*.htm || :
	rm html/*.tmp

# fixtoc.pl is used here to reformat the TOC to look good under the
# multiple-part system (which makeinfo doesn't really know about).
ick.htm: ick.txi ickstyle.css fdl-1-2.txi tidy.cfg
	makeinfo --html --no-split --no-header --css-include=ickstyle.css -D notsplit -o ick.htm ick.txi
	-perl -i.tmp -w fixtoc.pl ick.htm
	rm ick*.tmp
	chmod u+w ick.htm
	-tidy -config tidy.cfg -m ick.htm || :

# For some reason, ick.htm comes out of this readonly sometimes, which
# is why the chmod seems to be needed. No idea why, though.

ick.txt: ick.txi fdl-1-2.txi
	makeinfo --plaintext -D notsplit -o ick.txt ick.txi
ick.pdf: ick.txi fdl-1-2.txi
	-texi2dvi -b -c -e -p -q ick.txi

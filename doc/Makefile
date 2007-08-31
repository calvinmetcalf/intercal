#
# Makefile for the Intercal documentation
#

# To see the Revised Manual, generate intercal.txt or intercal.ps
# To see the original INTERCAL-72 manual, generate original.txt or original.ps.

# We make two passes in order to resolve SETR/GETNH references.
# The sed hackery strips the trailing dots from level 1 section references.

x.txt: x.mm
	groff -dc=y -Tascii -p -t -mm x.mm >x.txt


all: intercal.ps intercal.txt original.ps original.txt

intercal.ps: intercal.mm
	groff -dc=y -Tps    -p -t -mm intercal.mm >/dev/null
	sed <intercal.refs.tmp -e '/hn \([0-9]*\)\.$$/s//hn \1/' >tmp.refs;
	mv tmp.refs intercal.refs.tmp
	groff -dc=y -Tps    -p -t -mm intercal.mm >intercal.ps
	rm intercal.refs.qrf

original.ps: intercal.mm
	groff -dc=n -Tps    -p -t -mm intercal.mm >/dev/null
	sed <intercal.refs.tmp -e '/hn \([0-9]*\)\.$$/s//hn \1/' >tmp.refs;
	mv tmp.refs intercal.refs.tmp
	groff -dc=n -Tps    -p -t -mm intercal.mm >original.ps
	rm intercal.refs.qrf

intercal.txt: intercal.mm
	groff -dc=y -Tascii -p -t -mm intercal.mm >/dev/null
	sed <intercal.refs.tmp -e '/hn \([0-9]*\)\.$$/s//hn \1/' >tmp.refs;
	mv tmp.refs intercal.refs.tmp
	groff -dc=y -Tascii -p -t -mm intercal.mm >intercal.txt
	rm intercal.refs.qrf

original.txt: intercal.mm
	groff -dc=n -Tascii -p -t -mm intercal.mm >/dev/null
	sed <intercal.refs.tmp -e '/hn \([0-9]*\)\.$$/s//hn \1/' >tmp.refs;
	mv tmp.refs intercal.refs.tmp
	groff -dc=n -Tascii -p -t -mm intercal.mm >original.txt
	rm intercal.refs.qrf

clean:
	rm -f *.ps *.txt *.refs*


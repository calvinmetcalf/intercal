# Make the derived forms of documentation for upload

all: README.html NEWS.html BUGS.html THEORY.html ../doc/ick.htm ../doc/intercal.ps

clean:
	rm -f README.html NEWS.html BUGS.html THEORY.html

README.html: ../README
	asciidoc - <../README >README.html
BUGS.html: ../BUGS
	asciidoc - <../BUGS >BUGS.html
NEWS.html: ../NEWS
	asciidoc - <../NEWS >NEWS.html
THEORY.html: ../doc/THEORY.txt
	asciidoc - <../doc/THEORY.txt >THEORY.html

../doc/ick.htm: ../doc/ick.txi
	cd ../doc; make ick.htm
../doc/intercal.ps: ../doc/intercal.mm
	cd ../doc; make intercal.ps

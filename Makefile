#
# Utility productions for the INTERCAL distribution
#
VERSION = 0.17

tgz: intercal-$(VERSION).tar.gz
tar: intercal-$(VERSION).tar

# This requires my patch to tar-1.12, or a stock tar-1.13 (not yet released);
# the version must implement the --name-prefix option.
intercal-$(VERSION).tar.gz: $(all)
	tar --name-prefix='intercal-$(VERSION)/' -czf intercal-$(VERSION).tar.gz `cat MANIFEST`



SHAROPTS = -l63 -n intercal -o intercal -a -s esr@snark.thyrsus.com 
shar:
	(cd ..; shar $(SHAROPTS) $(MANIFEST) >intercal-$(VERSION).shar)

clean:
	rm -f intercal-$(VERSION).shar intercal-$(VERSION).tar intercal-$(VERSION).tar.gz


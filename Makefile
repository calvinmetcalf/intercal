#
# Utility productions for the INTERCAL distribution
#
VERSION = 0.15
MANIFEST = `sed <intercal-$(VERSION)/MANIFEST -e "s/^/intercal-$(VERSION)\//"`

tgz: intercal-$(VERSION).tar.gz
tar: intercal-$(VERSION).tar

intercal-$(VERSION).tar.gz: intercal-$(VERSION).tar
	gzip -f intercal-$(VERSION).tar
intercal-$(VERSION).tar:
	(cd ..; tar cvf ick.tar $(MANIFEST))
	mv ../ick.tar ./intercal-$(VERSION).tar

SHAROPTS = -l63 -n intercal -o intercal -a -s esr@snark.thyrsus.com 
shar:
	(cd ..; shar $(SHAROPTS) $(MANIFEST) >intercal-$(VERSION).shar)

clean:
	rm -f intercal-$(VERSION).shar intercal-$(VERSION).tar intercal-$(VERSION).tar.gz


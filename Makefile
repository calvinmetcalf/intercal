#
# Utility productions for the INTERCAL distribution
#
VERSION=0.21

dist: intercal-$(VERSION).tar.gz

intercal-$(VERSION).tar.gz:
	sed "s:^:intercal-$(VERSION)/:" <MANIFEST >manifest
	(cd ..; ln -s intercal intercal-$(VERSION))
	(cd ..; tar -czvf intercal/intercal-$(VERSION).tar.gz `cat intercal/manifest`)
	rm ../intercal-$(VERSION) manifest

RPMROOT=/usr/src/redhat
RPM = rpm
RPMFLAGS = -ba
rpm: dist
	cp intercal-$(VERSION).tar.gz $(RPMROOT)/SOURCES;
	cp intercal.spec $(RPMROOT)/SPECS
	cd $(RPMROOT)/SPECS; $(RPM) $(RPMFLAGS) intercal.spec	
	cp $(RPMROOT)/RPMS/`arch|sed 's/i[4-9]86/i386/'`/intercal-$(VERSION)*.rpm .
	cp $(RPMROOT)/SRPMS/intercal-$(VERSION)*.src.rpm .



SHAROPTS = -l63 -n intercal -o intercal -a -s esr@snark.thyrsus.com 
shar:
	(cd ..; shar $(SHAROPTS) $(MANIFEST) >intercal-$(VERSION).shar)

clean:
	rm -f intercal-$(VERSION).shar intercal-$(VERSION).tar intercal-$(VERSION).tar.gz


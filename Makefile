#
# Utility productions for the INTERCAL distribution
#
# Don't forget to fix the spec file and index.html when you bump this.
VERSION=0.22

dist: intercal-$(VERSION).tar.gz

intercal-$(VERSION).tar.gz:
	sed "s:^:intercal-$(VERSION)/:" <MANIFEST >manifest
	(cd ..; ln -s intercal intercal-$(VERSION))
	(cd ..; tar -czvf intercal/intercal-$(VERSION).tar.gz `cat intercal/manifest`)
	rm ../intercal-$(VERSION) manifest

intercal.spec:
	specgen.sh $(VERSION) >intercal.spec

RPMROOT=/usr/src/redhat
RPM = rpmbuild
RPMFLAGS = -ba
rpm: dist
	cp intercal-$(VERSION).tar.gz $(RPMROOT)/SOURCES;
	cp intercal.spec $(RPMROOT)/SPECS
	cd $(RPMROOT)/SPECS; $(RPM) $(RPMFLAGS) intercal.spec	
	cp $(RPMROOT)/RPMS/`arch|sed 's/i[4-9]86/i386/'`/intercal-$(VERSION)*.rpm .
	cp $(RPMROOT)/SRPMS/intercal-$(VERSION)*.src.rpm .

clean:
	rm -f intercal-$(VERSION).shar intercal-$(VERSION).tar intercal-$(VERSION).tar.gz intercal.spec


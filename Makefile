#
# Utility productions for the INTERCAL distribution
#
ick.tar:
	(cd ..; tar cvf ick.tar `cat intercal-0.13/MANIFEST`)
	mv ../ick.tar .

ick.tar.Z: ick.tar
	compress ick.tar

ick.tar.gz: ick.tar
	gzip ick.tar

SHAROPTS = -l63 -n intercal -o intercal -a -s esr@snark.thyrsus.com 
ick.shar:
	(cd ..; shar $(SHAROPTS) `cat intercal-0.13/MANIFEST` >ick.shar)

#
# Torture-tester for the INTERCAL compiler
#

ICK = ../src/ick -b

SOURCES = \
	./sample.i \
	./beer.i \
	./bubble.i \
	./count.3i \
	./hail_mary.3i \
	./icat.i \
	./tests/test1.i \
	./tests/limits.i \
	./tests/limits.3i \
	./tests/test16bit.i \
	./tests/testarray.i \
	./tests/testcf.i \
	./tests/testlib2.i \
	./tests/test2.i \
	./tests/testops.3i \
	./tests/testops.5i \
	./tests/testsplat.i \
	./tests/testi18n.i \
	./pow.i \
	./random.i \
	./random2.i \
	./rot13.i \
	./rot13b.i \
	./iogerunds.i \
	./pi.i \
	./lib/lib2.i \
	./lib/numio.i \
	./lib/syslib.i \
	./lib/syslib3.i \
	./lib/syslib4.i \
	./lib/syslib5.i \
	./lib/syslib6.i \
	./lib/syslib7.i \
	./lib/plus.i \
	./primes.i \
	./life.i \
	./life2.i \
	./sink.i \
	./change.i \
	./iching1.i \
	./iching2.i \
	./hello.i \
	./ins.i \
	./yapp.i

TARGETS = \
	./sample \
	./beer \
	./bubble \
	./count \
	./hail_mary \
	./icat \
	./tests/test1 \
	./tests/limits \
	./tests/limits3 \
	./tests/test16bit \
	./tests/testarray \
	./tests/testcf \
	./tests/testlib2 \
	./tests/test2 \
	./tests/testops \
	./tests/testops \
	./tests/testsplat \
	./tests/testi18n \
	./pow \
	./random \
	./random2 \
	./rot13 \
	./rot13b \
	./iogerunds \
	./pi \
	./lib/lib2 \
	./lib/numio \
	./lib/syslib \
	./lib/syslib3 \
	./lib/syslib4 \
	./lib/syslib5 \
	./lib/syslib6 \
	./lib/syslib7 \
	./lib/plus \
	./primes \
	./life \
	./life2 \
	./sink \
	./change \
	./iching1 \
	./iching2 \
	./hello \
	./ins \
	./yapp

.SUFFIXES:	.i .3i .4i .5i .6i .7i

.i:
	$(ICK) $<

.3i:
	$(ICK) $<

.4i:
	$(ICK) $<

.5i:
	$(ICK) $<

.6i:
	$(ICK) $<

.7i:
	$(ICK) $<

test: $(TARGETS)

./lib/syslib3: ./lib/syslib.3i
	cp ./lib/syslib.3i ./lib/syslib3.3i
	$(ICK) ./lib/syslib3.3i

./lib/syslib4: ./lib/syslib.4i
	cp ./lib/syslib.4i ./lib/syslib4.4i
	$(ICK) ./lib/syslib4.4i

./lib/syslib5: ./lib/syslib.5i
	cp ./lib/syslib.5i ./lib/syslib5.5i
	$(ICK) ./lib/syslib5.5i

./lib/syslib6: ./lib/syslib.6i
	cp ./lib/syslib.6i ./lib/syslib6.6i
	$(ICK) ./lib/syslib6.6i

./lib/syslib7: ./lib/syslib.7i
	cp ./lib/syslib.7i ./lib/syslib7.7i
	$(ICK) ./lib/syslib7.7i

./tests/limits3: ./tests/limits.3i
	cp ./tests/limits.3i ./tests/limits3.3i
	$(ICK) ./tests/limits3.3i

lines:
	wc -l $(SOURCES)

clean:
	rm -f $(TARGETS) *.c *.o lib/syslib?.?i tests/limits3.3i

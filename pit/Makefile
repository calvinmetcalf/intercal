#
# Torture-tester for the INTERCAL compiler
#

SOURCES = \
	./sample.i \
	./beer.i \
	./bubble.i \
	./count.3i \
	./hail_mary.3i \
	./icat.i \
	./tests/limits.i \
	./tests/test1.i \
	./tests/test16bit.i \
	./tests/testarray.i \
	./tests/testcf.i \
	./tests/testlib2.i \
	./tests/test2.i \
	./tests/testlimits.i \
	./tests/testsplat.i \
	./pow.i \
	./random.i \
	./random2.i \
	./rot13.i \
	./iogerunds.i \
	./pi.i \
	./lib/lib2.i \
	./lib/numio.i \
	./lib/syslib.i \
	./lib/plus.i \
	./primes.i \
	./life.i \
	./life2.i \
	./sink.i

TARGETS = \
	./sample \
	./beer \
	./bubble \
	./count \
	./hail_mary \
	./icat \
	./tests/limits \
	./tests/test1 \
	./tests/test16bit \
	./tests/testarray \
	./tests/testcf \
	./tests/testlib2 \
	./tests/test2 \
	./tests/testlimits \
	./tests/testsplat \
	./pow \
	./random \
	./random2 \
	./rot13 \
	./iogerunds \
	./pi \
	./lib/lib2 \
	./lib/numio \
	./lib/syslib \
	./lib/plus \
	./primes \
	./life \
	./life2 \
	./sink

test:
	for x in $(SOURCES); do	echo "Test-compiling $$x"; ../src/ick -b $$x; done

lines:
	wc -l $(SOURCES)

clean:
	rm -f $(TARGETS)


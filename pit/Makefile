#
# Torture-tester for the INTERCAL compiler
#

SOURCES = \
	./sample.i \
	./bubble.i \
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
	./random.i \
	./iogerunds.i \
	./pi.i \
	./lib/lib2.i \
	./lib/syslib.i \
	./lib/plus.i \
	./primes.i \
	./life.i \
	./life2.i \
	./sink.i

TARGETS = \
	./sample \
	./bubble \
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
	./random \
	./iogerunds \
	./pi \
	./lib/lib2 \
	./lib/syslib \
	./lib/plus \
	./primes \
	./life \
	./life2 \
	./sink

test:
	testcompile $(SOURCES)

lines:
	wc -l $(SOURCES)

clean:
	rm -f $(TARGETS)


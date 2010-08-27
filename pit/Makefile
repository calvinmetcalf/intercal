#
# Torture-tester for the INTERCAL compiler
#
# Type 'make' to test-compile the pit programs and run a regression check

ICK = ../ick -b

BASE2SOURCES = \
	./beer.i \
	./bubble.i \
	./change.i \
	./fft.i \
	./guess.i \
	./icat.i \
	./iching1.i \
	./iching2.i \
	./ins.i \
	./lib/lib2.i \
	./lib/numio.i \
	./lib/plus.i \
	./lib/syslib.i \
	./lib/syslib3.i \
	./lib/syslib4.i \
	./lib/syslib5.i \
	./lib/syslib6.i \
	./lib/syslib7.i \
	./life.i \
	./life2.i \
	./pi.i \
	./pow.i \
	./quine \
	./primes.i \
	./random.i \
	./random2.i \
	./rot13.i \
	./rot13b.i \
	./sample.i \
	./sort.i \
	./tpk.i \
	./tests/hello.i \
	./tests/iogerunds.i \
	./tests/limits.i \
	./tests/permute.i \
	./tests/sink.i \
	./tests/test1.i \
	./tests/test16bit.i \
	./tests/test2.i \
	./tests/testarray.i \
	./tests/testcf.i \
	./tests/testlib2.i \
	./tests/testsplat.i \
	./yapp.i

BASENSOURCES = \
	./hail_mary.3i \
	./tests/count.3i \
	./tests/limits3.3i \
	./tests/testops3.3i \
	./tests/testops5.5i

SOURCES = $(BASE2SOURCES) $(BASENSOURCES)

TARGETS = $(BASE2SOURCES:.i=) \
	./hail_mary \
	./tests/count \
	./tests/limits3 \
	./tests/testops3 \
	./tests/testops5

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

# For instructions on test conventions, see the header comment of regresscheck
test: $(TARGETS)
	@./beer | ./regresscheck beer 
	@cat fft-delta.tst | ./fft | ./regresscheck fft-delta
	@cat fft-tophat.tst | ./fft | ./regresscheck fft-tophat
	@echo 'Hello, sailor!' | ./regresscheck icat 
	@echo 'A man, a plan, a canal, Panama.' | ./rot13 | ./regresscheck rot13
	@echo 'A man, a plan, a canal, Panama.' | ./rot13b | ./regresscheck rot13b
	@echo "FIVE" | ./pi | ./regresscheck pi
	@cp quine.i quine.chk; ./quine | ./regresscheck quine; rm quine.chk 
	@cat sort.tst | ./sort | ./regresscheck sort
	@cat tpk.tst | ./tpk | ./regresscheck tpk
	@./tests/count | ./regresscheck tests/count 
	@./tests/hello | ./regresscheck tests/hello 
	@./tests/permute | ./regresscheck tests/permute 
	@./tests/sink 2>&1 | ./regresscheck tests/sink
	@./tests/test1 2>&1 | ./regresscheck tests/test1 
	@cat tests/test2.tst | ./tests/test2 | ./regresscheck tests/test2
	@echo "Tests complete."

# Needs special handling because the -m option is required
tests/permute: tests/permute.i
	$(ICK) -m tests/permute.i
fft: fft.i
	$(ICK) -m fft.i

lines:
	wc -l $(SOURCES)

clean:
	rm -f $(TARGETS) *.c *.o *~

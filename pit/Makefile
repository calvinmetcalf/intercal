#
# Torture-tester for the INTERCAL compiler
#

ICK = ../ick -b

BASE2SOURCES = \
	./beer.i \
	./bubble.i \
	./change.i \
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
	./primes.i \
	./random.i \
	./random2.i \
	./rot13.i \
	./rot13b.i \
	./sample.i \
	./tests/hello.i \
	./tests/iogerunds.i \
	./tests/permute.i \
	./tests/sink.i \
	./tests/test1.i \
	./tests/limits.i \
	./tests/test16bit.i \
	./tests/testarray.i \
	./tests/testcf.i \
	./tests/testlib2.i \
	./tests/test2.i \
	./tests/testsplat.i \
	./yapp.i

BASENSOURCES = \
	./hail_mary.3i \
	./tests/count.3i \
	./tests/limits.3i \
	./tests/testops.3i \
	./tests/testops.5i

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

test: $(TARGETS)
	@beer | regresscheck beer 
	@echo 'Hello, sailor!' | regresscheck icat 
	@echo 'A man, a plan, a canal, Panama.' | rot13 | regresscheck rot13
	@echo 'A man, a plan, a canal, Panama.' | rot13b | regresscheck rot13b
	@echo "FIVE" | pi | regresscheck pi
	@tests/count | regresscheck tests/count 
	@tests/hello | regresscheck tests/hello 
	@tests/permute | regresscheck tests/permute 
	@tests/sink 2>&1 | regresscheck tests/sink
	@echo "Tests complete."

# Needs special handling because the -m option is required
tests/permute: tests/permute.i
	$(ICK) -m tests/permute.i

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

./tests/testops3: ./tests/testops.3i
	cp ./tests/testops.3i ./tests/testops3.3i
	$(ICK) ./tests/testops3.3i

./tests/testops5: ./tests/testops.5i
	cp ./tests/testops.5i ./tests/testops5.5i
	$(ICK) ./tests/testops5.5i

lines:
	wc -l $(SOURCES)

clean:
	rm -f $(TARGETS) *.c *.o *~ lib/syslib?.?i 
	rm tests/limits3.3i tests/testops3.3i tests/testops5.5i

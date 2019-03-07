.PHONY: test libco

test: libco-test-64 libco-test-32
	@echo "==== TEST 64 bit ===="
	@LD_LIBRARY_PATH=.. ./libco-test-64
	@echo "==== TEST 32 bit ===="
	@LD_LIBRARY_PATH=.. ./libco-test-32

libco:
	@cd .. && make -s

libco-test-64: main.c libco
	gcc -I.. -L.. -m64 main.c -o libco-test-64 -lco-64

libco-test-32: main.c libco
	gcc -I.. -L.. -m32 main.c -o libco-test-32 -lco-32

clean:
	rm -f libco-test-32 libco-test-64

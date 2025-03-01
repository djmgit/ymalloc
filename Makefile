build:
	gcc -o ymalloc ymalloc.c

run:
	./ymalloc

build_and_run: build run

test:
	gcc -I./ -o tests tests.c ymalloc.c
	./tests
	rm -rf tests

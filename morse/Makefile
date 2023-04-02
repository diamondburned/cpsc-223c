.PHONY: build test

CC ?= gcc
CFLAGS = $(shell echo $$(cat compile_flags.txt))
CFILES = $(shell find . -name "*.c" -and -not -name "*test.c")
CTESTS = $(shell find . -name "*.c" -and -not -name "main.c")

build: main.out

test: test.out
	./test.out

morse.txt.h:
	xxd -i morse.txt > morse.txt.h

main.out: $(CFILES) morse.txt.h
	$(CC) $(CFLAGS) -o main.out $(CFILES)

test.out: $(CTESTS)
	$(CC) $(CFLAGS) -o test.out $(CTESTS)

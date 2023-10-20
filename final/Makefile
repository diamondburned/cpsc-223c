.PHONY: build test

CC ?= gcc
CFLAGS = $(shell echo $$(cat compile_flags.txt))
CFILES = $(shell find . -name "*.c" -and -not -name "*test.c")
CTESTS = $(shell find . -name "*.c" -and -not -name "main.c")

build: main.out

test: test.out
	./test.out

main.out: $(CFILES)
	$(CC) $(CFLAGS) -o main.out $(CFILES)

test.out: $(CTESTS)
	$(CC) $(CFLAGS) -o test.out -fsanitize=address $(CTESTS)

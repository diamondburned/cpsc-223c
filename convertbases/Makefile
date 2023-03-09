.PHONY: all test

CFILES = $(shell find . -name '*.c')
HFILES = $(shell find . -name '*.h')

all: main.out test

main.out: $(CFILES) $(HFILES)
	gcc -Wall -Wextra -O3 -o main.out $(CFILES)

test: main.out
	./main.out --run-tests

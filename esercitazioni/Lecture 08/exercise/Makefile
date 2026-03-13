CC = gcc
CFLAGS = -O3 -Wall -pedantic -std=c17
HEADERS = $(wildcard *.h)

# Clang (default su macOS) non supporta questi flag GCC.
ifeq ($(shell $(CC) --version 2>/dev/null | head -n1 | grep -ci clang),0)
CFLAGS += -fno-if-conversion -fno-if-conversion2 -fno-tree-loop-if-convert
endif

all: main

main: main.o merge.o
	${CC} ${CFLAGS} -o $@ $^

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $<


.PHONY: clean all

clean:
	rm -f *.o

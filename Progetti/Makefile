CC     = gcc
OBJS   = tensor.o stack.o ops.o io.o lexer.o main.o

# Rileva il sistema operativo
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
    # macOS - M1: OpenMP via Homebrew libomp
    BREW_PREFIX := $(shell brew --prefix libomp)
    CFLAGS  = -O2 -march=native -Wall -Wextra \
              -Xpreprocessor -fopenmp \
              -I$(BREW_PREFIX)/include
    LDFLAGS = -L$(BREW_PREFIX)/lib -lomp
else
    # Linux
    CFLAGS  = -O2 -march=native -fopenmp -Wall -Wextra
    LDFLAGS =
endif

tensorforth: $(OBJS)
	$(CC) $(CFLAGS) -o tensorforth $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

tensor.o: tensor.c tensor.h
stack.o:  stack.c stack.h tensor.h
ops.o:    ops.c ops.h tensor.h stack.h
io.o:     io.c io.h tensor.h stack.h
lexer.o:  lexer.c lexer.h
main.o:   main.c stack.h ops.h io.h lexer.h

clean:
	rm -f $(OBJS) tensorforth


test: tensor.o stack.o test_main.o
	gcc -O2 -Wall -o test_stack tensor.o stack.o test_main.o

test_main.o: test_main.c
	gcc -O2 -Wall -c test_main.c -o test_main.o
CC     = gcc
CFLAGS = -O2 -march=native -fopenmp -Wall -Wextra
OBJS   = tensor.o stack.o ops.o io.o lexer.o main.o

tensorforth: $(OBJS)
	$(CC) $(CFLAGS) -o tensorforth $(OBJS)

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
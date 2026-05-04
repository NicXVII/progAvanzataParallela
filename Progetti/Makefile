CC     = gcc
SRCDIR = src
INCDIR = include
OBJS   = $(SRCDIR)/tensor.o $(SRCDIR)/stack.o $(SRCDIR)/ops.o $(SRCDIR)/lexer.o $(SRCDIR)/main.o

# Rileva il sistema operativo
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
    # macOS - M1: OpenMP via Homebrew libomp
    BREW_PREFIX := $(shell brew --prefix libomp)
    CFLAGS  = -O2 -march=native -Wall -Wextra -I$(INCDIR) \
              -Xpreprocessor -fopenmp \
              -I$(BREW_PREFIX)/include
    LDFLAGS = -L$(BREW_PREFIX)/lib -lomp
else
    # Linux
    CFLAGS  = -O2 -march=native -fopenmp -Wall -Wextra -I$(INCDIR)
    LDFLAGS =
endif

tensorforth: $(OBJS)
	$(CC) $(CFLAGS) -o tensorforth $(OBJS) $(LDFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRCDIR)/tensor.o: $(SRCDIR)/tensor.c $(INCDIR)/tensor.h
$(SRCDIR)/stack.o:  $(SRCDIR)/stack.c $(INCDIR)/stack.h $(INCDIR)/tensor.h
$(SRCDIR)/ops.o:    $(SRCDIR)/ops.c $(INCDIR)/ops.h $(INCDIR)/tensor.h $(INCDIR)/stack.h
$(SRCDIR)/lexer.o:  $(SRCDIR)/lexer.c $(INCDIR)/lexer.h
$(SRCDIR)/main.o:   $(SRCDIR)/main.c $(INCDIR)/stack.h $(INCDIR)/ops.h $(INCDIR)/lexer.h

clean:
	rm -f $(OBJS) tensorforth test_stack test_main.o


test: $(SRCDIR)/tensor.o $(SRCDIR)/stack.o test_main.o
	gcc -O2 -Wall -I$(INCDIR) -o test_stack $(SRCDIR)/tensor.o $(SRCDIR)/stack.o test_main.o

test_main.o: test_main.c
	gcc -O2 -Wall -I$(INCDIR) -c test_main.c -o test_main.o
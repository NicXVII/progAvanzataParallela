CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g
SRC = src/main.c src/bst.c src/t_node.c
OBJ = $(SRC:.c=.o)
TARGET = bst_app
LIBRARY = libbst.a

all: $(TARGET) $(LIBRARY)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

$(LIBRARY): src/bst.o src/t_node.o
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) $(LIBRARY)

.PHONY: all clean
CC = gcc
CFLAGS = -std=c11 -pedantic-errors -Wall -O2
LDFLAGS = -O2
LDLIBS = -lm

BINARY = problem1
OBJECTS = main.o file_utils.o disjoint_set.o

.PHONY: all clean run

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f *.o
	-rm -f $(BINARY)
run: $(BINARY)
	./$<

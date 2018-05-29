CC = gcc
CFLAGS = -std=c11 -pedantic-errors -Wall -O2
SAN_FLAGS = -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak\
 -fsanitize=undefined -fsanitize=float-divide-by-zero -fno-sanitize-recover=all
LDFLAGS = $(CFLAGS)
LDLIBS = -lm

BINARY = problem1
OBJECTS = main.o file_utils.o disjoint_set.o boundary.o

.PHONY: all clean run san

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c defs.h
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(BINARY)
	./$<

san: CFLAGS += $(SAN_FLAGS)
san: clean run
	-rm -f *.o

clean:
	-rm -f *.o
	-rm -f $(BINARY)


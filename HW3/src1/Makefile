CC = gcc
CFLAGS = -std=c89 -pedantic-errors -Wall
LDFLAGS =
LDLIBS = -lm

TARGET = problem1
OBJECTS = $(TARGET).o file_utils.o disjoint_set.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f *.o
	-rm -f $(TARGET)


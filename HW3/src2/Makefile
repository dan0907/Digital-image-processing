CC = gcc
CFLAGS = -std=c89 -pedantic-errors -Wall
LDFLAGS =
LDLIBS = -lm

TARGET = problem2
OBJECTS = $(TARGET).o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f *.o
	-rm -f $(TARGET)


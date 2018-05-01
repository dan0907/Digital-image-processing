CC = gcc
CFLAGS = -std=c89 -pedantic-errors -Wall -O3
LDFLAGS = -O3
LDLIBS = -lm

TARGET = problem2
OBJECTS = $(TARGET).o file_utils.o boundary.o

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f *.o
	-rm -f $(TARGET)

run: $(TARGET)
	./$<


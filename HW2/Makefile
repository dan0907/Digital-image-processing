# DIP Homework Assignment 2
# 4 4, 2018
# Name: 沈育霖
# ID: r06922109
# email: danny321974345@gmail.com
# compiled on Linux (x86_64) with gcc

CC = gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra -Wstrict-prototypes -g
LIBS = -lm

OBJ1 = problem1.o file_utils.o boundary.o histogram.o
OBJ2 = problem2.o file_utils.o boundary.o

.PHONY: all clean

all: problem1.out  problem2.out
	@echo "Usage: ./problem1.out"
	@echo "Usage: ./problem2.out"

problem1.out: $(OBJ1)
	$(CC) $(CFLAGS) -o $@ $(OBJ1) $(LIBS)

problem2.out: $(OBJ2)
	$(CC) $(CFLAGS) -o $@ $(OBJ2) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f *.o
	-rm -f *.out


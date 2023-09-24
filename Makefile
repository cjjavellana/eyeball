CC=gcc
CFLAGS=-g3 -Wall

all: clean main

main: main.o
	gcc -lm main.o -o eyeball


main.o: main.c
	$(CC) -c main.c

clean:
	$(RM) *.o *.out eyeball

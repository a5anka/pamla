CC=gcc
CFLAGS=

example: main.c perfcount.o
	$(CC) main.c perfcount.o -o example
perfcount.o: perfcount.c perfcount.h
	$(CC) -c perfcount.c -o perfcount.o

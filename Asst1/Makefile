CC = gcc
CFLAGS = -Wall -O

memgrind: mymalloc.c memgrind.c
	$(CC) -o memgrind mymalloc.c memgrind.c

clean:
	rm -f *.o memgrind.o
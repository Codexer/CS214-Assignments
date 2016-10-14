CC=gcc
REQUIRED_FLAGS = -Wall -g
CFLAGS=-I.
DEPS = mymalloc.h
OBJ = mymalloc.o memgrind.o 

memgrind: $(OBJ)
	$(CC) $(REQUIRED_FLAGS) -o $@ $^ $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c $(REQUIRED_FLAGS) -o $@ $< $(CFLAGS)

clean:
	rm *.o memgrind
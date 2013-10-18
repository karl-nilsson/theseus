CC=gcc
CFLAGS=-I. -Wall -pedantic -std=c99

DEPS = none

maze_gen: maze_generation.o
	$(CC) $(CFLAGS) -o maze_gen maze_generation.o

maze_generation.o: maze_generation.c
	$(CC) $(CFLAGS) -c maze_generation.c

test: maze_gen
	./maze_gen
	

clean:
	        rm *.o maze_gen




CC=gcc
CFLAGS=-I.

all: sequential_min_max parallel_min_max

sequential_min_max: utils.o find_min_max.o
	$(CC) -o sequential_min_max sequential_min_max.c utils.o find_min_max.o $(CFLAGS)

parallel_min_max: utils.o find_min_max.o
	$(CC) -o parallel_min_max parallel_min_max.c utils.o find_min_max.o $(CFLAGS)

utils.o: utils.c utils.h
	$(CC) -c utils.c -o utils.o $(CFLAGS)

find_min_max.o: find_min_max.c find_min_max.h utils.h
	$(CC) -c find_min_max.c -o find_min_max.o $(CFLAGS)

clean:
	rm -f *.o sequential_min_max parallel_min_max

.PHONY: all clean

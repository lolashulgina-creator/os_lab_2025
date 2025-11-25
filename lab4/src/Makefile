CC=gcc
CFLAGS=-I.

all: parallel_min_max_timeout process_memory parallel_sum

parallel_min_max_timeout: parallel_min_max_timeout.c find_min_max.o utils.o
	$(CC) -o parallel_min_max_timeout parallel_min_max_timeout.c find_min_max.o utils.o $(CFLAGS)

process_memory: process_memory.c
	$(CC) -o process_memory process_memory.c $(CFLAGS)

parallel_sum: parallel_sum.c utils.o
	$(CC) -o parallel_sum parallel_sum.c utils.o $(CFLAGS) -lpthread

find_min_max.o: find_min_max.c find_min_max.h utils.h
	$(CC) -c find_min_max.c -o find_min_max.o $(CFLAGS)

utils.o: utils.c utils.h
	$(CC) -c utils.c -o utils.o $(CFLAGS)

clean:
	rm -f *.o parallel_min_max_timeout process_memory parallel_sum

.PHONY: all clean
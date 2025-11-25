CC=gcc
CFLAGS=-I.

all: parallel_factorial deadlock_demo

parallel_factorial: parallel_factorial.c
	$(CC) -o parallel_factorial parallel_factorial.c $(CFLAGS) -lpthread

deadlock_demo: deadlock_demo.c
	$(CC) -o deadlock_demo deadlock_demo.c $(CFLAGS) -lpthread

clean:
	rm -f parallel_factorial deadlock_demo

.PHONY: all clean
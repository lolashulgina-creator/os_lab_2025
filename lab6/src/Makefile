CC=gcc
CFLAGS=-I.
LDFLAGS=-lpthread

all: client server

client: client.c utils.o
	$(CC) -o client client.c utils.o $(CFLAGS) $(LDFLAGS)

server: server.c utils.o
	$(CC) -o server server.c utils.o $(CFLAGS) $(LDFLAGS)

utils.o: utils.c utils.h
	$(CC) -c utils.c -o utils.o $(CFLAGS)

clean:
	rm -f client server utils.o

.PHONY: all clean
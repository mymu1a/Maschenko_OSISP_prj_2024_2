CC=gcc
CFLAGS=-I.
LIBS=-lssl -lcrypto

all : controller

controller : main.o
	$(CC) -o $@ $^ $(LIBS)

main.o : main.cc main.h
	$(CC) -c $< $(CFLAGS)

.PHONY: clean

clean:
	rm *.o
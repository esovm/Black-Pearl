
CC=gcc

CFLAGS=-O3

all: pearl libpearl.a

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

pearl: pearl.o start.o
	$(CC) -o $@ $^ $(CFLAGS)

libpearl.a: pearl.o
	ar rcs $@ $^

.PHONY: clean

clean:
	rm -f *.o libpearl.a pearl.exe pearl.
    
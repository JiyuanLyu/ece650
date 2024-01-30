CC=gcc
CFLAGS=-O3 -fPIC
DEPS=my_malloc.h

all: lib
lib: libmymalloc.so

libmymalloc.so: my_malloc.o
	$(CC) $(CFLAGS) -shared -o $@ $< 

%.o: %.c my_malloc.h
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	rm -f *~ *.o *.so

clobber:
	rm -f *~ *.o

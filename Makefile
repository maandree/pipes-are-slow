.POSIX:

CC = cc

CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
CFLAGS   = -std=c99 -Wall -O2
LDFLAGS  = -s


all: bench

.c.o:
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

.o:
	$(CC) -o $@ $< $(LDFLAGS)

clean:
	-rm -f -- bench *.o *.su

.SUFFIXES:
.SUFFIXES: .o .c

.PHONY: all clean

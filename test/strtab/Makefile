ROOT=../..
INCP=$(ROOT)/include
LIBP=$(ROOT)/lib

STLIBS=$(LIBP)/libstrtab.a $(LIBP)/liberror.a

SRC=strtabtest.c
OBJ=$(SRC:.c=.o)

INC=-I$(INCP)

include $(ROOT)/src/sp.mak

.PHONY: all clean test

all: strtabtest

.c.o:
	$(CC) $(CFLAGS) -c $<

strtabtest: strtabtest.o $(STLIBS)
	$(LD) $(LDFLAGS) -o strtabtest strtabtest.o $(STLIBS)

clean:
	rm -f $(OBJ) strtabtest


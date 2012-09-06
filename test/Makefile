ROOT=..
INCP=$(ROOT)/include
LIBP=$(ROOT)/lib

LBUFFER=$(LIBP)/libbuffer.a

SRC=bgettest.c bungettest.c
OBJ=$(SRC:.c=.o)

INC=-I$(INCP)

include ../src/sp.mak

.PHONY: all clean test

all: bgettest bungettest

.c.o:
	$(CC) $(CFLAGS) -c $<

bgettest: bgettest.o $(LBUFFER)
	$(LD) $(LDFLAGS) -o bgettest bgettest.o $(LBUFFER)

bungettest: bungettest.o $(LBUFFER)
	$(LD) $(LDFLAGS) -o bungettest bungettest.o $(LBUFFER)

clean:
	rm -f $(OBJ) bgettest bungettest

test: all
	./bungettest data/king_james_bible.ascii.txt


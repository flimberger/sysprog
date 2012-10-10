ROOT=../..
INCP=$(ROOT)/include
LIBP=$(ROOT)/lib

STLIBS=$(LIBP)/libbuffer.a $(LIBP)/liberror.a

SRC=bgettest.c bungettest.c bputtest.c
OBJ=$(SRC:.c=.o)

INC=-I$(INCP)

include $(ROOT)/src/sp.mak

.PHONY: all clean test

all: bgettest bungettest bputtest bprintftest

.c.o:
	$(CC) $(CFLAGS) -c $<

bgettest: bgettest.o $(STLIBS)
	$(LD) $(LDFLAGS) -o bgettest bgettest.o $(STLIBS)

bungettest: bungettest.o $(STLIBS)
	$(LD) $(LDFLAGS) -o bungettest bungettest.o $(STLIBS)

bputtest: bputtest.o $(STLIBS)
	$(LD) $(LDFLAGS) -o bputtest bputtest.o $(STLIBS)

bprintftest: bprintftest.o $(STLIBS)
	$(LD) $(LDFLAGS) -o bprintftest bprintftest.o $(STLIBS)

clean:
	rm -f $(OBJ) bgettest bungettest


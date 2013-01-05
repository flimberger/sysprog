ROOT=../..
INCP=$(ROOT)/include
LIBP=$(ROOT)/lib

STLIBS=$(LIBP)/libsymtab.a $(LIBP)/libstrtab.a $(LIBP)/liberror.a

SRC=symtabtest.c
OBJ=$(SRC:.c=.o)

INC=-I$(INCP)

include $(ROOT)/src/sp.mak

.PHONY: all clean

all: symtabtest

.c.o:
	$(CC) $(CFLAGS) -c $<

symtabtest: symtabtest.o $(STLIBS)
	$(LD) $(LDFLAGS) -o symtabtest symtabtest.o $(STLIBS)

clean:
	rm -f $(OBJ) symtabtest


NAME=symtab

ROOT=../../..
INCP=$(ROOT)/include
LIBP=$(ROOT)/lib

HDR=$(INCP)/$(NAME).h
SRC=symtab.c
OBJ=$(SRC:.c=.o)

AROBJ=lib$(NAME).a

INC=-I$(INCP)

include ../../sp.mak

.PHONY: all install clean nuke

all: $(AROBJ)

.c.o: $(HDR) $(INCP)/defs.h
	$(CC) $(CFLAGS) -c $<

$(AROBJ): $(OBJ)
	ar rs $@ $(OBJ)

install: all
	cp $(AROBJ) $(LIBP)

clean:
	rm -f $(OBJ) $(AROBJ)

nuke: clean
	rm -f $(LIBP)/$(AROBJ)

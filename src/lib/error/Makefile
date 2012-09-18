NAME=error

ROOT=../../..
INCP=$(ROOT)/include
LIBP=$(ROOT)/lib

HDR=$(INCP)/$(NAME).h
SRC=error.c
OBJ=$(SRC:.c=.o)

AROBJ=lib$(NAME).a

INC=-I$(INCP)

include ../../sp.mak

.PHONY: all clean install

all: $(AROBJ)

.c.o: $(HDR)
	$(CC) $(CFLAGS) -c $<

$(AROBJ): $(OBJ)
	ar rs $@ $(OBJ)

clean:
	rm -f $(OBJ) $(AROBJ)

install: all
	cp $(AROBJ) $(LIBP)


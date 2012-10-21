NAME=spc

ROOT=../../..
INCP=$(ROOT)/include
LIBP=$(ROOT)/lib
BINP=$(ROOT)/bin

HDR=$(NAME).h
SRC=spc.c acceptor.c
OBJ=$(SRC:.c=.o)

INC=-I$(INCP)
ARS=$(LIBP)/liberror.a $(LIBP)/libbuffer.a $(LIBP)/libstrtab.a $(LIBP)/libsymtab.a

include ../../sp.mak

.PHONY: all install clean nuke

all: $(NAME)

.c.o: $(HDR) $(INCP)/defs.h
	$(CC) $(CFLAGS) -c $<

$(NAME): $(HDR) $(OBJ) $(ARS)
	$(LD) $(LDFLAGS) $(OBJ) $(ARS) -o $(NAME)

install: all
	cp $(NAME) $(BINP)

clean:
	rm -f $(OBJ) $(NAME)

nuke: clean
	rm -f $(BINP)/spc

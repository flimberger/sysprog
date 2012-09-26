NAME=spc

ROOT=../../..
INCP=$(ROOT)/include
LIBP=$(ROOT)/lib
BINP=$(ROOT)/bin

HDR=$(NAME).h
SRC=lexer.c symtab.c
OBJ=$(SRC:.c=.o)

INC=-I$(INCP)
ARS=$(LIBP)/liberror.a $(LIBP)/libbuffer.a

include ../../sp.mak

.PHONY: all clean install

all: $(NAME)

.c.o: $(HDR)
	$(CC) $(CFLAGS) -c $<

$(NAME): $(HDR) $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) $(ARS) -o $(NAME)

clean:
	rm -f $(OBJ) $(NAME)

install: all
	cp $(NAME) $(BINP)

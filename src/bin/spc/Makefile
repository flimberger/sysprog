NAME=spc

ROOT=../../..
INCP=$(ROOT)/include
LIBP=$(ROOT)/lib
BINP=$(ROOT)/bin

HDR=$(NAME).h
SRC=spc.c scanner.c parser.c btree.c printtoken.c typecheck.c spvm_opcodes.c codegen.c
OBJ=$(SRC:.c=.o)

INC=-I$(INCP)
LIB=-static -L$(LIBP) -lerror -lbuffer -lsymtab -lstrtab

include ../../sp.mak

.PHONY: all install clean nuke

all: $(NAME)

.c.o: $(HDR) $(INCP)/defs.h
	$(CC) $(CFLAGS) -c $<

spvm_opcodes.c: spvm_opcodes.h

codegen.c: spvm_opcodes.h

$(NAME): $(HDR) $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(NAME) $(LIB)

install: all
	cp $(NAME) $(BINP)

clean:
	rm -f $(OBJ) $(NAME)

nuke: clean
	rm -f $(BINP)/spc

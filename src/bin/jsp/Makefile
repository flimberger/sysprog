NAME=jsp
PLATFORM=linux-x64

ROOT=../../..
INCP=$(ROOT)/include
LIBP=$(ROOT)/lib
BINP=$(ROOT)/bin
SPCDIR=../spc

HDR=jvm.h $(PLATFORM)/platform.h $(SPCDIR)/spc.h
SRC=generator.c
OBJ=$(SPCDIR)/spc.o $(SPCDIR)/btree.o $(SPCDIR)/scanner.o $(SPCDIR)/parser.o $(SRC:.c=.o)

INC=-I$(INCP) -I$(PLATFORM) -I$(SPCDIR)
LIB=-static -L$(LIBP) -lerror -lbuffer -lsymtab -lstrtab

include ../../sp.mak

.PHONY: all install clean nuke spc

all: $(NAME)

.c.o: $(HDR) $(INCP)/defs.h
	$(CC) $(CFLAGS) -c $<

spc:
	$(MAKE) -C $(SPCDIR) .c.o

$(NAME): $(HDR) $(OBJ) spc
	$(LD) $(LDFLAGS) $(OBJ) -o $(NAME) $(LIB)

install: all
	cp $(NAME) $(BINP)

clean:
	rm -f $(OBJ) $(NAME)

nuke: clean
	rm -f $(BINP)/spc

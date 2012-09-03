NAME=libbuffer

ROOT=../../..
INCP=$(ROOT)/include
LIBP=$(ROOT)/lib

HDR=$(INCP)/buffer.h
SRC=makebuf.c flushbuf.c
OBJ=$(SRC:.c=.o)

INC=-I$(INCP)

include ../../sp.mak

.PHONY: all clean install

all: libbuffer

.c.o: $(HDR)
	$(CC) $(CFLAGS) -c $<

$(NAME): $(HDR) $(OBJ)
	ar rs $(NAME).a $(OBJ)

clean:
	rm -f $(OBJ) $(NAME).a

install: all
	cp $(NAME) $(LIBP)


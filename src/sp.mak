# This file is included after setting the local build variables like
# $(INC) and $(LIB)

# directories
LIBS=-lc $(LIB)

# compiler and linker options
CDBG=-g -O0
CPPFLAGS=-D_GNU_SOURCE -D_POSIX_C_SOURCE=200112L
CFLAGS=-Wall -Wextra -Werror -std=c99 -pedantic $(CDBG) $(INC) $(CPPFLAGS) -fstrict-aliasing

LDBG=-g
LDFLAGS=$(LIBS) $(LDBG)

# compiler and linker
CC=gcc
LD=gcc

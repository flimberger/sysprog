BIN=bin
INC=include
LIB=lib
SRC=src

BINARIES=$(LIB)/libbuffer.a $(LIB)/liberror.a $(LIB)/libstrtab.a $(LIB)/libsymtab.a $(BIN)/spc
SOURCES=$(SRC)/lib/buffer $(SRC)/lib/error $(SRC)/lib/strtab $(SRC)/lib/symtab $(SRC)/bin/spc

.PHONY: all install doc clean nuke $(SOURCES)

all:
	@echo To build and install all binaries, use the install target.

install: $(BINARIES)
	@echo Add $$PWD/bin to your \$$PATH to use the spc executable.

$(LIB)/libbuffer.a:
	$(MAKE) -C $(SRC)/lib/buffer install

$(LIB)/liberror.a:
	$(MAKE) -C $(SRC)/lib/error install

$(LIB)/libstrtab.a:
	$(MAKE) -C $(SRC)/lib/strtab install

$(LIB)/libsymtab.a:
	$(MAKE) -C $(SRC)/lib/symtab install

$(BIN)/spc: $(LIB)/libbuffer.a $(LIB)/liberror.a $(LIB)/libstrtab.a $(LIB)/libsymtab.a
	$(MAKE) -C $(SRC)/bin/spc install

$(SOURCES):
	$(MAKE) -C $@ nuke

doc:
	$(MAKE) -C $@

clean:
	@echo No files to remove. To remove installed files, use the nuke target.

nuke: $(SOURCES)

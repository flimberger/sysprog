TEX=xelatex
DOC=Sysprog-Dokumentation
DOCII=Sysprog-Dokumentation-II
SRC=sysprog.tex intro.tex buffer.tex error.tex strtab.tex symtab.tex scanner.tex manpages.tex defs.tex
ROOT=..
MAN=spc.txt buffer.txt error.txt strtab.txt symtab.txt


.PHONY: all once clean man

all: $(DOC).pdf $(DOCII).pdf

$(DOC).pdf: $(SRC) man
	xelatex sysprog && xelatex sysprog && xelatex sysprog
	mv sysprog.pdf $(DOC).pdf

$(DOCII).pdf:
	xelatex sysprogII && xelatex sysprogII && xelatex sysprogII
	mv sysprogII.pdf $(DOCII).pdf
	
man: $(MAN)

spc.txt: $(ROOT)/man/man1/spc.1
	man $< | col -b > $@

buffer.txt: $(ROOT)/man/man3/buffer.3
	man $< | col -b > $@

error.txt: $(ROOT)/man/man3/error.3
	man $< | col -b > $@

strtab.txt: $(ROOT)/man/man3/strtab.3
	man $< | col -b > $@

symtab.txt: $(ROOT)/man/man3/symtab.3
	man $< | col -b > $@

.gv.png:
	$(GV) $(GVFLAGS) $<

once:
	xelatex sysprogII

clean:
	rm -f *.aux *.bbl *.bcf *.blg *.lof *.log *.out *.pdf *.toc $(MAN) 


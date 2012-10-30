TEX=xelatex
DOC=Sysprog-Dokumentation
SRC=sysprog.tex intro.tex buffer.tex error.tex strtab.tex symtab.tex scanner.tex
ROOT=..
MAN=man-spc.tex man-buffer.tex man-error.tex man-strtab.tex man-symtab.tex


.PHONY: all once clean man

all: $(DOC).pdf

$(DOC).pdf: $(SRC) man
	xelatex sysprog && xelatex sysprog && xelatex sysprog
	mv sysprog.pdf $(DOC).pdf
	
man: $(MAN)

man-spc.tex:	$(ROOT)/man/man1/spc.1
	man $< | col -b > $@.txt
	pandoc -o $@ $@.txt

man-buffer.tex: 	$(ROOT)/man/man3/buffer.3
	man $< | col -b > $@.txt
	pandoc -o $@ $@.txt

man-error.tex:	$(ROOT)/man/man3/error.3
	man $< | col -b > $@.txt
	pandoc -o $@ $@.txt

man-strtab.tex:	$(ROOT)/man/man3/strtab.3
	man $< | col -b > $@.txt
	pandoc -o $@ $@.txt

man-symtab.tex:	$(ROOT)/man/man3/symtab.3
	man $< | col -b > $@.txt
	pandoc -o $@ $@.txt


once:
	xelatex sysprog

clean:
	rm -f *.aux *.bbl *.bcf *.blg *.lof *.log *.out *.pdf *.toc $(MAN) #*.txt 

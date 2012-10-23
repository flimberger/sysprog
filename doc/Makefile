TEX=xelatex
DOC=Sysprog-Dokumentation
SRC=sysprog.tex intro.tex buffer.tex error.tex strtab.tex symtab.tex scanner.tex

.PHONY: all once clean

all: $(DOC).pdf

$(DOC).pdf: $(SRC)
	xelatex sysprog && xelatex sysprog && xelatex sysprog
	mv sysprog.pdf $(DOC).pdf

once:
	xelatex sysprog

clean:
	rm -f *.aux *.bbl *.bcf *.blg *.lof *.log *.out *.pdf *.toc

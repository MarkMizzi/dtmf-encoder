# Copyright 2019: Johann A. Briffa <johann.briffa@um.edu.mt>

SOURCES := $(wildcard minutes*.tex)
TARGETS := $(patsubst %.tex,%.pdf,$(SOURCES))

# Master targets

all: $(TARGETS)

## Setting targets

FORCE:

.PHONY:	FORCE all clean

.SUFFIXES: # Delete the default suffixes

.DELETE_ON_ERROR:

# Dependencies

$(TARGETS): $(SOURCES)

# Rules for main compilation

%.aux: %.tex
	pdflatex --shell-escape $<

%.pdf: %.tex %.aux
	pdflatex --shell-escape $<
	pdflatex --shell-escape $<

clean:
	-/bin/rm -f *.aux *.log *.fls *.synctex.gz *.fdb_latexmk *.pdf

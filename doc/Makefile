PAPER = kite
TEX = $(wildcard *.tex)
FIGS = $(wildcard figures/*.pdf)
SRC = $(wildcard *.cls *.sty)

.PHONY: all clean

$(PAPER).pdf: $(TEX) $(BIB) $(FIGS) $(SRC)
	rm -rf *.glsdefs
	pdflatex $(PAPER)
	pdflatex $(PAPER)

tar:
	make; make clean
	mkdir $(PAPER)
	cp -rf $(TEX) $(SRC) *.pdf figures Makefile $(PAPER)/
	zip -r $(PAPER).zip $(PAPER)
	rm -rf $(PAPER)

zip:
	make tar

dist:
	rm -rf *.glsdefs *.aux *.bbl *.blg *.log *.out $(PAPER) $(PAPER).zip

clean:
	make dist; rm -f $(PAPER).pdf

distclean:
	make clean


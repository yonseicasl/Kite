PAPER = kite
TEX = $(wildcard *.tex)
FIG = $(wildcard *.png)
PDF = $(wildcard *.pdf)

.PHONY: all clean

$(PAPER).pdf: $(TEX) $(FIG)
	rm -rf *.glsdefs
	pdflatex $(PAPER)
	pdflatex $(PAPER)

tar:
	make; make clean
	mkdir $(PAPER)
	cp -rf $(TEX) $(FIG) $(PDF) Makefile $(PAPER)/
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


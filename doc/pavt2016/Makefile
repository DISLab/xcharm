all: ucharelib_paper_pavt2016.pdf

ucharelib_paper_pavt2016.pdf: ucharelib_paper_pavt2016.tex
	pdflatex $<
	bibtex ucharelib_paper_pavt2016
	pdflatex $<
	pdflatex $<

clean: 
	rm -f ucharelib_paper_pavt2016.pdf
	rm -f *.aux *.log *.nav *.out *.snm *.toc
	rm -f *.bbl *.blg *.vrb

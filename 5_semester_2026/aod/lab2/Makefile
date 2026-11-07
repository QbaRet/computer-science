# Lista 2 AOD: rozwiązuje wszystkie egzemplarze i buduje sprawozdanie.
# Autor: Jakub Retmańczyk
GLPSOL = glpsol
RUN = $(GLPSOL) --math

all: wyniki sprawozdanie.pdf

wyniki:
	mkdir -p wyniki
	$(RUN) zadanie1/zad1.mod -d zadanie1/zad1.dat > wyniki/zad1.txt
	$(RUN) zadanie2/zad2.mod -d zadanie2/zad2.dat > wyniki/zad2.txt
	$(RUN) zadanie3/zad3.mod -d zadanie3/zad3.dat > wyniki/zad3.txt
	$(RUN) zadanie4/zad4.mod -d zadanie4/a.dat > wyniki/zad4_a.txt
	$(RUN) zadanie4/zad4.mod -d zadanie4/a.dat --nomip > wyniki/zad4_a_lp.txt
	$(RUN) zadanie4/zad4.mod -d zadanie4/a_bez_czasu.dat > wyniki/zad4_a_bez_czasu.txt
	$(RUN) zadanie4/zad4.mod -d zadanie4/a_bez_czasu.dat --nomip > wyniki/zad4_a_bez_czasu_lp.txt
	$(RUN) zadanie4/zad4.mod -d zadanie4/b.dat > wyniki/zad4_b.txt
	$(RUN) zadanie4/zad4.mod -d zadanie4/b_bez_czasu.dat > wyniki/zad4_b_bez_czasu.txt
	$(RUN) zadanie4/zad4.mod -d zadanie4/c_kontrprzyklad.dat > wyniki/zad4_c.txt
	$(RUN) zadanie4/zad4.mod -d zadanie4/c_kontrprzyklad.dat --nomip > wyniki/zad4_c_lp.txt
	$(RUN) zadanie4/zad4.mod -d zadanie4/d_remis.dat --nomip > wyniki/zad4_d_remis_lp.txt
	$(RUN) zadanie4/zad4.mod -d zadanie4/d_remis.dat --nomip --interior > wyniki/zad4_d_remis_ipm.txt
	$(RUN) zadanie5/zad5.mod -d zadanie5/zad5.dat > wyniki/zad5.txt
	for k in 1 2 3; do \
	  $(RUN) zadanie6/zad6.mod -d zadanie6/teren.dat -d zadanie6/k$$k.dat > wyniki/zad6_k$$k.txt; \
	done

sprawozdanie.pdf: sprawozdanie.tex
	latexmk -pdf -interaction=nonstopmode -halt-on-error sprawozdanie.tex

clean:
	rm -rf wyniki
	latexmk -C sprawozdanie.tex

.PHONY: all wyniki clean


:: Kör latexgenomgången med bibtex etc.
:: dirigera bort output genom "lat > NUL" om inga fel påträffas


@echo off

set doc=rapport

:rensa skiten
::del %doc%.glo %doc%.ist
::del %doc%.glg 
::del %doc%.gls
::del %doc%.aux

pdflatex %doc%
::makeglossaries %doc%
::makeindex %doc%.glo -s %doc%.ist -t %doc%.glg -o %doc%.gls
::makeindex -s %doc%.ist -t %doc%.alg -o %doc%.acr %doc%.acn


::biber %doc%
::biblatex %doc% 
bibtex %doc%
pdflatex %doc%
pdflatex %doc%

texlua checkcites.lua --unused %doc%.aux
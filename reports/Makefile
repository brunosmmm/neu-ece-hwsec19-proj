
all: media_files paper.pdf

media_files:
	$(MAKE) -C media

paper.pdf: paper.tex
	latexmk -pdf paper

clean:
	$(MAKE) -C media clean
	rm -rf paper.pdf

.PHONY: clean

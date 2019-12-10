SUBMISSION_FILES=paper.pdf project.README
ZIP_NAME=soutomaiormunizmo-FinalProject.zip

all: media_files paper.pdf

media_files:
	$(MAKE) -C media

paper.pdf: paper.tex
	latexmk -pdf paper

submission: $(SUBMISSION_FILES)
	zip $(ZIP_NAME) $(SUBMISSION_FILES)

clean:
	$(MAKE) -C media clean
	rm -rf paper.pdf

.PHONY: clean

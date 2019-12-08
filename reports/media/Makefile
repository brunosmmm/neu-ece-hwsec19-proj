
PLOTS=ecb_speedup.pdf cbc_speedup.pdf

all: $(PLOTS)

$(PLOTS): plots.py ../../evaluation/rocket.results ../../evaluation/boom.results
	python3 plots.py

clean:
	rm -rf $(PLOTS)

.PHONY: clean


PLOTS=ecb_speedup.pdf cbc_speedup.pdf

all: $(PLOTS)

$(PLOTS): plots.py
	python3 plots.py

clean:
	rm -rf $(PLOTS)

.PHONY: clean


all: riscvfiles lightweightsw iss_extensions evaluate

riscvfiles: riscv
	$(MAKE) -C $^

lightweightsw: lightweight-sw
	$(MAKE) -C $^

iss_extensions: iss-rocc-extension
	$(MAKE) -C $^

evaluate: evaluation
	$(MAKE) -C $^

clean:
	$(MAKE) -C lightweight-sw clean && $(MAKE) -C iss-rocc-extension clean && $(MAKE) -C riscv clean && $(MAKE) -C evaluate clean

.PHONY: clean

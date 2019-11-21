
all: riscvfiles lightweightsw iss_extensions

riscvfiles: riscv
	$(MAKE) -C $^

lightweightsw: lightweight-sw
	$(MAKE) -C $^

iss_extensions: iss-rocc-extension
	$(MAKE) -C $^

clean:
	$(MAKE) -C lightweight-sw clean && $(MAKE) -C iss-rocc-extension clean && $(MAKE) -C riscv clean

.PHONY: clean


all: lightweightsw iss_extensions

lightweightsw: lightweight-sw
	$(MAKE) -C $^

iss_extensions: iss-rocc-extension
	$(MAKE) -C $^

clean:
	$(MAKE) -C lightweight-sw clean && $(MAKE) -C iss-rocc-extension clean

.PHONY: clean

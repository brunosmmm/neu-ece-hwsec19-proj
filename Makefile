
all: lightweightsw

lightweightsw: lightweight-sw
	$(MAKE) -C $^

clean:
	$(MAKE) -C lightweight-sw clean

.PHONY: clean

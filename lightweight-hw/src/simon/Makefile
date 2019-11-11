
SIMULATION_FILES=rotate_unit.v simon_kexp.v simon_round.v

coretb: coretb.v $(SIMULATION_FILES)
	iverilog -g2012 -o $@ -y. $<

simon_kexp.vcd: coretb
	./coretb

simulate: simon_kexp.vcd

clean:
	rm -rf coretb

.PHONY: clean simon_kexp.vcd

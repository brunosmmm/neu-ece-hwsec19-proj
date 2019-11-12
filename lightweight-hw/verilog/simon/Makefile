
SIMULATION_FILES=rotate_unit.v simon_kexp.v simon_round.v

coretb: coretb.v $(SIMULATION_FILES)
	iverilog -g2012 -o $@ -y. $<

coretb_chisel: coretb_chisel.v ../../chisel/simon/SimonKeyExpander.v ../../chisel/simon/SimonRound.v
	iverilog -g2012 -o $@ -y. -y../../chisel/simon $<

../../chisel/simon/%.v:
	$(MAKE) -C ../../chisel/simon $@

simon_kexp.vcd: coretb
	./coretb

simon_kexp_chisel.vcd: coretb_chisel
	./coretb_chisel

simulate: simon_kexp.vcd
simulate_chisel: simon_kexp_chisel.vcd

clean:
	rm -rf coretb coretb_chisel

.PHONY: clean simon_kexp.vcd

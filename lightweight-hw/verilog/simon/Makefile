
SIMULATION_FILES=rotate_unit.v simon_kexp.v simon_round.v
GENERATED_PATH=external

coretb: coretb.v $(SIMULATION_FILES)
	iverilog -g2012 -o $@ -y. $<

coretb_chisel: coretb_chisel.v external/SimonKeyExpander/SimonKeyExpander.v external/SimonRound/SimonRound.v
	iverilog -g2012 -o $@ -y. -yexternal/SimonKeyExpander -yexternal/SimonRound $<

tb_SimonCore: tb_SimonCore.v external/SimonCore/SimonCore.v
	iverilog -g2012 -o $@ -y. -yexternal/SimonCore $<

external/%.v:
	TARGET=$(shell pwd)/$(GENERATED_PATH) $(MAKE) -C ../../chisel/simon $@

simon_kexp.vcd: coretb
	./coretb

simon_kexp_chisel.vcd: coretb_chisel
	./coretb_chisel

SimonCore.vcd: tb_SimonCore
	./tb_SimonCore

simulate: simon_kexp.vcd
simulate_chisel: simon_kexp_chisel.vcd

clean:
	rm -rf coretb coretb_chisel *.vcd

chisel_clean:
	TARGET=$(shell pwd)/$(GENERATED_PATH) $(MAKE) -C ../../chisel/simon clean

.PHONY: clean simon_kexp.vcd simon_kexp_chisel.vcd chisel_clean SimonCore.vcd

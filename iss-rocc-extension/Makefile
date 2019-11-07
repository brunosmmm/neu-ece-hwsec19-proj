CXX ?=g++
ISA_SIM_INCLUDE =../riscv-isa-sim/riscv ../riscv-isa-sim/softfloat ../riscv-isa-sim/build
INCLUDE_FLAGS=$(ISA_SIM_INCLUDE:%=-I%)
CFLAGS ?=-Wall -g

all: libsimon_rocc.so

%.o: %.cc
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -fPIC -c $< -o $@

libsimon_rocc.so: simon_rocc.o
	$(CXX) -fPIC -shared  -o $@ $^  -Wl,--no-whole-archive "../riscv-isa-sim/build/libriscv.a"

run_test: libsimon_rocc.so
	LD_LIBRARY_PATH=. ../riscv-isa-sim/build/spike --extension=simon_rocc ../riscv-pk/build/pk ../lightweight-sw/simon_test_riscv_basic

clean:
	rm -rf *.o *.so

.PHONY: clean

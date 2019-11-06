CXX ?=g++
ISA_SIM_INCLUDE =../riscv-isa-sim/riscv ../riscv-isa-sim/softfloat ../riscv-isa-sim/build
INCLUDE_FLAGS=$(ISA_SIM_INCLUDE:%=-I%)
CFLAGS ?=-Wall

all: libsimon_rocc.so

%.o: %.cc
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -fPIC -c $< -o $@

libsimon_rocc.so: simon_rocc.o
	$(CXX) -fPIC -shared  -o $@ $^  -Wl,--no-whole-archive "../riscv-isa-sim/build/libriscv.a"

clean:
	rm -rf *.o *.so

.PHONY: clean

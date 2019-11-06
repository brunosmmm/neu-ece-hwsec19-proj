CXX ?=g++
ISA_SIM_INCLUDE =../riscv-isa-sim/riscv ../riscv-isa-sim/softfloat ../riscv-isa-sim
INCLUDE_FLAGS=$(ISA_SIM_INCLUDE:%=-I%)
CFLAGS ?=-Wall

all: libsimon_rocc.so

%.o: %.cc
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -fPIC -c $< -o $@

libsimon_rocc.so: simon_rocc.o
	$(CXX) -shared -o $@ $^

clean:
	rm -rf *.o *.so

.PHONY: clean

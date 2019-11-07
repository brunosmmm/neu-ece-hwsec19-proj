CXX ?=g++
ISA_SIM_INCLUDE =../riscv-isa-sim/riscv ../riscv-isa-sim/softfloat ../riscv-isa-sim/build ../lightweight-sw/include
INCLUDE_FLAGS=$(ISA_SIM_INCLUDE:%=-I%)
CFLAGS ?=-Wall -g

all: libsimon_rocc.so simon_mmio.so

%.o: %.cc
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -fPIC -c $< -o $@

libsimon_rocc.so: simon_rocc.o simon_base.o ../lightweight-sw/src/simon.o
	$(CXX) -fPIC -shared  -o $@ $^  -Wl,--no-whole-archive "../riscv-isa-sim/build/libriscv.a"

simon_mmio.so: ../lightweight-sw/src/simon.o simon_mmio.o simon_base.o
	$(CXX) -fPIC -shared -o $@ $^

rocc_test: libsimon_rocc.so
	LD_LIBRARY_PATH=. ../riscv-isa-sim/build/spike --extension=simon_rocc ../riscv-pk/build/pk ../lightweight-sw/simon_test_rocc

mmio_test:
	../riscv-isa-sim/build/spike --extlib=./simon_mmio.so ../riscv-pk/build/pk ../lightweight-sw/simon_test_mmio

clean:
	rm -rf *.o *.so

.PHONY: clean

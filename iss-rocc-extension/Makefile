CXX ?=g++

REF_IMPL_PATH ?= ../lightweight-sw
SPIKE_PATH ?= ../riscv-isa-sim/build
SPIKE_ARGS ?=
PK_PATH ?= ../riscv-pk/build
ISA_SIM_INCLUDE =../riscv-isa-sim/riscv ../riscv-isa-sim/softfloat ../riscv-isa-sim/build $(REF_IMPL_PATH)/include
INCLUDE_FLAGS=$(ISA_SIM_INCLUDE:%=-I%)
CFLAGS ?=-Wall -g

all: libsimon_rocc.so simon_mmio.so libsalsa20_rocc.so

%.o: %.cc
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -fPIC -c $< -o $@

# rocc accelerator targets
lib%_rocc.so: %_rocc.o %_base.o $(REF_IMPL_PATH)/src/%.o
	$(CXX) -shared  -o $@ $^ -Wl,--no-whole-archive "$(SPIKE_PATH)/libriscv.a"

# mmio accelerator targets
%_mmio.so: $(REF_IMPL_PATH)/src/simon.o %_mmio.o %_base.o
	$(CXX) -shared -o $@ $^

%_rocc_test: lib%_rocc.so
	LD_LIBRARY_PATH=. $(SPIKE_PATH)/spike $(SPIKE_ARGS) --extension=$(*F)_rocc $(PK_PATH)/pk $(REF_IMPL_PATH)/$(*F)_test_rocc

%_mmio_test: %_mmio.so
	$(SPIKE_PATH)/spike $(SPIKE_ARGS) --extlib=./$(*F)_mmio.so --device=$(*F)_mmio,0x10000000 $(REF_IMPL_PATH)/$(*F)_test_mmio

clean:
	rm -rf *.o *.so

.PHONY: clean

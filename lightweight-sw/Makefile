
PREFIX ?=
CC ?= $(PREFIX)gcc
RVCC ?= riscv64-unknown-elf-gcc
SRCPATH = src
TESTPATH = tests
INCLUDES = include
INCLUDEFLAGS = -I$(INCLUDES)
RVINCLUDEFLAGS = -I../rocc-software/src -Iinclude
CFLAGS ?= -Wall -g $(INCLUDEFLAGS)
RVCFLAGS ?= -Wall -g $(RVINCLUDEFLAGS) -DRISCV64
RVDEBUGPATH = ../riscv-tests/debug/

CIPHERS ?= simon salsa20 speck
CIPHER_TESTS := $(foreach cipher,$(CIPHERS),$(cipher)_test_*)

SPIKE_PATH ?= ../riscv-isa-sim/build
PK_PATH ?= ../riscv-pk/build

all: native_tests riscv_tests liblightc.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.rv.o: %.c
	$(RVCC) $(RVCFLAGS) -c $< -o $@

liblightc.a : $(SRCPATH)/simon.o $(SRCPATH)/speck.o $(SRCPATH)/salsa20.o
	$(PREFIX)ar rcs $@ $^

%_test_basic: $(TESTPATH)/test_%.o $(SRCPATH)/%.o $(TESTPATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^

%_test_rocc: $(TESTPATH)/test_%_rocc.rv.o $(SRCPATH)/riscv/rvutil.rv.o
	$(RVCC) $(RVCFLAGS) -o $@ $^

%_test_mmio: $(TESTPATH)/test_%_mmio.c $(SRCPATH)/riscv/rvutil.c $(RVDEBUGPATH)/programs/entry.S $(RVDEBUGPATH)/programs/init.c
	$(RVCC) $(RVCFLAGS) -march=rv64imac -mabi=lp64 $^ -DNHARTS=1 \
	-T $(RVDEBUGPATH)/targets/RISC-V/spike64.lds \
	-nostartfiles -mcmodel=medany -DXLEN=64 -o $@

%_test_basic_riscv: $(TESTPATH)/test_%.rv.o $(SRCPATH)/riscv/rvutil.rv.o $(TESTPATH)/util.rv.o $(SRCPATH)/%.rv.o
	$(RVCC) $(RVCFLAGS) -o $@ $^

tests_native_simon: simon_test_basic
tests_native_salsa20: salsa20_test_basic
tests_native_speck: speck_test_basic

tests_riscv_simon: simon_test_rocc simon_test_mmio simon_test_basic_riscv
tests_riscv_salsa20: salsa20_test_basic_riscv

native_tests: tests_native_simon tests_native_speck tests_native_salsa20
riscv_tests: tests_riscv_simon tests_riscv_salsa20

# run tests in spike riscv isa simulator
spike_run_%: %_test_basic_riscv
	$(SPIKE_PATH)/spike $(PK_PATH)/pk $<

clean:
	rm -rf $(TESTPATH)/*.o $(SRCPATH)/*.o $(CIPHER_NATIVE_TESTS) liblightc.a

.PHONY: clean

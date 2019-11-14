
PREFIX ?=
CC ?= $(PREFIX)gcc
RVCC ?= riscv64-unknown-elf-gcc
SRCPATH = src
TESTPATH = tests
INCLUDES = include
INCLUDEFLAGS = -I$(INCLUDES)
RVINCLUDEFLAGS = -I../rocc-software/src -Iinclude
CFLAGS ?= -Wall -g $(INCLUDEFLAGS)
RVCFLAGS ?= -Wall -g $(RVINCLUDEFLAGS) -DRISCV64 -O2
RVDEBUGPATH = misc
RVMCFLAGS ?= -I$(RVDEBUGPATH) $(RVCFLAGS) -mcmodel=medany -std=gnu99 -fno-common -fno-builtin-printf
RVMLDFLAGS=-static -nostdlib -nostartfiles -lgcc

CIPHERS ?= simon salsa20 speck
CIPHER_TESTS := $(foreach cipher,$(CIPHERS),$(cipher)_test_*)

SPIKE_PATH ?= ../riscv-isa-sim/build
PK_PATH ?= ../riscv-pk/build
SPIKE_ARGS ?=

HWACC_MMIO_BASE ?= 0x2000

all: native_tests riscv_tests liblightc.a

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

%.rv.o: %.c
	$(RVCC) $(RVCFLAGS) -c $< -o $@

%.rvm.o: %.S
	$(RVCC) $(RVMCFLAGS) -D__ASSEMBLY__=1 -c $< -o $@

%.rvm.o: %.c
	$(RVCC) -DHWACC_MMIO_BASE=$(HWACC_MMIO_BASE) $(RVMCFLAGS) -c $< -o $@

liblightc.a : $(SRCPATH)/simon.o $(SRCPATH)/speck.o $(SRCPATH)/salsa20.o
	$(PREFIX)ar rcs $@ $^

%_test_basic: $(TESTPATH)/test_%.o $(SRCPATH)/%.o $(TESTPATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^

%_test_state: $(TESTPATH)/test_%_state.o $(SRCPATH)/%.o $(TESTPATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^

%_test_rocc: $(TESTPATH)/test_%_rocc.rv.o $(SRCPATH)/riscv/rvutil.rv.o $(TESTPATH)/util.rv.o
	$(RVCC) $(RVCFLAGS) -o $@ $^

%_test_mmio: $(TESTPATH)/test_%_mmio.rvm.o $(SRCPATH)/riscv/rvutil.rvm.o $(RVDEBUGPATH)/crt.rvm.o $(RVDEBUGPATH)/syscalls.rvm.o
	$(RVCC) -T $(RVDEBUGPATH)/link.ld $(RVMLDFLAGS) $^ -o $@

%_test_basic_riscv: $(TESTPATH)/test_%.rv.o $(SRCPATH)/riscv/rvutil.rv.o $(TESTPATH)/util.rv.o $(SRCPATH)/%.rv.o
	$(RVCC) $(RVCFLAGS) -o $@ $^

tests_native_simon: simon_test_basic
tests_native_salsa20: salsa20_test_basic
tests_native_speck: speck_test_basic

tests_riscv_simon: simon_test_rocc simon_test_mmio simon_test_basic_riscv
tests_riscv_salsa20: salsa20_test_basic_riscv salsa20_test_rocc

native_tests: tests_native_simon tests_native_speck tests_native_salsa20
riscv_tests: tests_riscv_simon tests_riscv_salsa20

# run tests in spike riscv isa simulator
spike_run_%: %_test_basic_riscv
	$(SPIKE_PATH)/spike $(SPIKE_ARGS) $(PK_PATH)/pk $<

clean:
	rm -rf $(TESTPATH)/*.o $(SRCPATH)/*.o $(CIPHER_TESTS) liblightc.a

.PHONY: clean

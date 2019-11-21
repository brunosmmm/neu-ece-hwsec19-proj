PREFIX ?=
CC ?= $(PREFIX)gcc
SRCPATH = src
TESTPATH = tests
INCLUDES = include
INCLUDEFLAGS = -I$(INCLUDES)
CFLAGS ?= -Wall -g $(INCLUDEFLAGS)
RVEXTRACFLAGS=$(INCLUDEFLAGS)
RISCVFILES = ../riscv
include $(RISCVFILES)/riscv.mk

CIPHERS ?= simon salsa20 speck
CIPHER_TESTS := $(foreach cipher,$(CIPHERS),$(cipher)_test_*)

SPIKE_PATH ?= ../riscv-isa-sim/build
PK_PATH ?= ../riscv-pk/build
SPIKE_ARGS ?=

HWACC_MMIO_BASE ?= 0x1000000
BAREMETAL_OBJS = $(RVDEBUGPATH)/crt.rvm.o $(RVDEBUGPATH)/syscalls.rvm.o

all: native_tests riscv_tests liblightc.a

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

liblightc.a : $(SRCPATH)/simon.o $(SRCPATH)/speck.o $(SRCPATH)/salsa20.o
	$(PREFIX)ar rcs $@ $^

%_test_basic: $(TESTPATH)/test_%.o $(SRCPATH)/%.o $(TESTPATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^

%_test_state: $(TESTPATH)/test_%_state.o $(SRCPATH)/%.o $(TESTPATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^

%_test_rocc.riscv: $(TESTPATH)/test_%_rocc.rv.o $(TESTPATH)/util.rv.o $(RVOBJECTS)
	$(RVCC) $(RVCFLAGS) -o $@ $^

%_test_rocc.riscvbm: $(TESTPATH)/test_%_rocc.rvm.o $(RVMOBJECTS)
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

%_test_toosly.riscv: $(TESTPATH)/test_%_toosly.rv.o $(TESTPATH)/util.rv.o $(RVOBJECTS)
	$(RVCC) $(RVCFLAGS) -o $@ $^

%_test_toosly.riscvbm: $(TESTPATH)/test_%_toosly.rvm.o $(RVMOBJECTS)
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

%_test_mmio.riscvbm: $(TESTPATH)/test_%_mmio.rvm.o $(RVMOBJECTS)
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

%_test_basic.riscv: $(TESTPATH)/test_%.rv.o $(TESTPATH)/util.rv.o $(SRCPATH)/%.rv.o $(RVOBJECTS)
	$(RVCC) $(RVCFLAGS) -o $@ $^

%_test_basic.riscvbm: $(TESTPATH)/test_%.rvm.o $(SRCPATH)/%.rvm.o $(RVMOBJECTS)
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

tests_native_simon: simon_test_basic
tests_native_salsa20: salsa20_test_basic
tests_native_speck: speck_test_basic

tests_riscv_simon: simon_test_rocc.riscv simon_test_rocc.riscvbm simon_test_mmio.riscvbm simon_test_basic.riscv simon_test_basic.riscvbm simon_test_toosly.riscv simon_test_toosly.riscvbm
tests_riscv_salsa20: salsa20_test_basic.riscv salsa20_test_rocc.riscv

native_tests: tests_native_simon tests_native_speck tests_native_salsa20
riscv_tests: tests_riscv_simon tests_riscv_salsa20

# run tests in spike riscv isa simulator
spike_run_%: %_test_basic.riscv
	$(SPIKE_PATH)/spike $(SPIKE_ARGS) $(PK_PATH)/pk $<

clean:
	rm -rf $(TESTPATH)/*.o $(SRCPATH)/*.o $(CIPHER_TESTS) liblightc.a

.PHONY: clean

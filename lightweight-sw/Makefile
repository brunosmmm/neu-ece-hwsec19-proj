
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

all: tests liblightc.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.rv.o: %.c
	$(RVCC) $(RVCFLAGS) -c $< -o $@

simon_test_basic: $(TESTPATH)/test_simon.o $(SRCPATH)/simon.o $(TESTPATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^

simon_test_rocc: $(TESTPATH)/test_simon_rocc.rv.o $(SRCPATH)/riscv/rvutil.rv.o
	$(RVCC) $(RVCFLAGS) -o $@ $^

simon_test_mmio: $(TESTPATH)/test_simon_mmio.c $(SRCPATH)/riscv/rvutil.c $(RVDEBUGPATH)/programs/entry.S $(RVDEBUGPATH)/programs/init.c
	$(RVCC) $(RVCFLAGS) -march=rv64imac -mabi=lp64 $^ -DNHARTS=1 \
	-T $(RVDEBUGPATH)/targets/RISC-V/spike64.lds \
	-nostartfiles -mcmodel=medany -DXLEN=64 -o $@

simon_test_basic_riscv: $(TESTPATH)/test_simon.rv.o $(SRCPATH)/riscv/rvutil.rv.o $(TESTPATH)/util.rv.o $(SRCPATH)/simon.rv.o
	$(RVCC) $(RVCFLAGS) -o $@ $^

speck_test_basic: $(TESTPATH)/test_speck.o $(SRCPATH)/speck.o $(TESTPATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^

tests_simon: simon_test_basic
tests_speck: speck_test_basic
tests: tests_simon tests_speck

tests_simon_riscv: simon_test_rocc simon_test_mmio simon_test_basic_riscv
tests_riscv: tests_simon_riscv

liblightc.a : $(SRCPATH)/simon.o $(SRCPATH)/speck.o
	$(PREFIX)ar rcs $@ $^

clean:
	rm -rf $(TESTPATH)/*.o $(SRCPATH)/*.o speck_test_* simon_test_* liblightc.a

.PHONY: clean

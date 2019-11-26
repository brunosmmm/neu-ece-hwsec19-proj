RISCVFILES=../riscv
LIGHTSW=../lightweight-sw
RVEXTRACFLAGS=-I$(LIGHTSW)/include -Iinclude -Wno-implicit-function-declaration
include $(RISCVFILES)/simon.mk
SRCPATH=src
TARGETS=evaluate1.riscv evaluate2.riscv evaluate3.riscv evaluate4.riscv evaluate5.riscv evaluate6.riscv
COMMONOBJS=$(SRCPATH)/common.rvm.o $(RVMOBJECTS) $(SIMONOBJECTS)

all: $(TARGETS)

evaluate1.riscv: $(SRCPATH)/evaluate1.rvm.o $(COMMONOBJS) $(LIGHTSW)/src/simon.rvm.o $(SRCPATH)/sw_ecb.rvm.o
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

evaluate2.riscv: $(SRCPATH)/evaluate2.rvm.o $(COMMONOBJS) $(LIGHTSW)/src/simon.rvm.o $(SRCPATH)/sw_cbc.rvm.o
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

evaluate3.riscv: $(SRCPATH)/evaluate3.rvm.o $(COMMONOBJS) $(SRCPATH)/mmio_ecb.rvm.o
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

evaluate4.riscv: $(SRCPATH)/evaluate4.rvm.o $(COMMONOBJS) $(SRCPATH)/mmio_ecb.rvm.o
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

evaluate5.riscv: $(SRCPATH)/evaluate5.rvm.o $(COMMONOBJS) $(SRCPATH)/mmio_cbc.rvm.o
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

evaluate6.riscv: $(SRCPATH)/evaluate6.rvm.o $(COMMONOBJS) $(SRCPATH)/rocc_ecb.rvm.o
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

test: $(TARGETS)
	./do_tests.sh $^

clean:
	rm -rf $(SRCPATH)/*.o $(TARGETS)

.PHONY: clean

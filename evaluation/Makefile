RISCVFILES=../riscv
LIGHTSW=../lightweight-sw
RVEXTRACFLAGS=-I$(LIGHTSW)/include -Iinclude
include $(RISCVFILES)/simon.mk
SRCPATH=src
TARGETS=evaluate1.riscv
OBJS=$(SRCPATH)/mmio_ecb.rvm.o $(SRCPATH)/sw_ecb.rvm.o $(SRCPATH)/sw_cbc.rvm.o $(SRCPATH)/rocc_ecb.rvm.o

all: $(TARGETS)

evaluate%.riscv: $(SRCPATH)/evaluate%.rvm.o $(RVMOBJECTS) $(SIMONOBJECTS) $(LIGHTSW)/src/simon.rvm.o $(OBJS)
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

clean:
	rm -rf $(SRCPATH)/*.o $(TARGETS)

.PHONY: clean

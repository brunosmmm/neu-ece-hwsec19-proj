RISCVFILES=../riscv
LIGHTSW=../lightweight-sw
RVEXTRACFLAGS=-I$(LIGHTSW)/include
include $(RISCVFILES)/simon.mk
SRCPATH=src
TARGETS=evaluate1.riscv

all: $(TARGETS)

evaluate%.riscv: $(SRCPATH)/evaluate%.rvm.o $(RVMOBJECTS) $(SIMONOBJECTS) $(LIGHTSW)/src/simon.rvm.o
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

clean:
	rm -rf $(SRCPATH)/*.o $(TARGETS)

.PHONY: clean

RISCVFILES=../riscv
LIGHTSW=../lightweight-sw
RVEXTRACFLAGS=-I$(LIGHTSW)/include
include $(RISCVFILES)/riscv.mk
SRCPATH=src
TARGETS=evaluate1.riscv

all: $(TARGETS)

evaluate%.riscv: $(SRCPATH)/evaluate%.rvm.o $(RVMOBJECTS) $(LIGHTSW)/src/simon.rvm.o
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

clean:
	rm -rf $(SRCPATH)/*.o $(TARGETS)

.PHONY: clean

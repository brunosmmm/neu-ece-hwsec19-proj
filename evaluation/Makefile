RISCVFILES=../riscv
include $(RISCVFILES)/riscv.mk
SRCPATH=src
TARGETS=evaluate1.riscv

all: $(TARGETS)

evaluate%.riscv: $(SRCPATH)/evaluate%.rvm.o $(RVMOBJECTS)
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

clean:
	rm -rf $(SRCPATH)/*.o $(TARGETS)

.PHONY: clean

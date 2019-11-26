RISCVFILES ?= .
include $(RISCVFILES)/riscv.mk

SIMONOBJECTS=$(RISCVFILES)/src/simon/mmio.rvm.o $(RISCVFILES)/src/simon/rocc.rvm.o $(RISCVFILES)/src/simon/toosly.rvm.o

all: $(SIMONOBJECTS)

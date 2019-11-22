RISCVFILES ?= .
include $(RISCVFILES)/riscv.mk

SIMONOBJECTS=$(RISCVFILES)/src/simon/mmio.rvm.o

all: $(SIMONOBJECTS)

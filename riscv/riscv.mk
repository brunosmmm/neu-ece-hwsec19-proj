RVEXTRACFLAGS ?=
RISCVFILES ?= .
RVCC = riscv64-unknown-elf-gcc
RVINCLUDE = -I$(RISCVFILES)/include
RVCFLAGS ?= $(RVEXTRACFLAGS) $(RVINCLUDE) -Wall -g -DRISCV64 -O2
RVMCFLAGS ?= $(RVEXTRACFLAGS) $(RVINCLUDE) -Wall -g -DRISCV64 -DBAREMETAL -O2 -mcmodel=medany -fno-common -fno-builtin-printf
RVMLDFLAGS= -T $(RISCVFILES)/misc/link.ld -static -nostdlib -nostartfiles -lgcc
RVMOBJECTS=$(RISCVFILES)/src/crt.rvm.o $(RISCVFILES)/src/syscalls.rvm.o $(RISCVFILES)/src/rvutil.rvm.o
RVOBJECTS=$(RISCVFILES)/src/rvutil.rv.o

%.rvm.o: %.c
	$(RVCC) $(RVMCFLAGS) -c $< -o $@

%.rv.o: %.c
	$(RVCC) $(RVCFLAGS) -c $< -o $@

%.rvm.o: %.S
	$(RVCC) $(RVMCFLAGS) -D__ASSEMBLY__=1 -c $< -o $@

%.riscv: %.rvm.o $(BMOBJECTS)
	$(RVCC) $(RVMLDFLAGS) $^ -o $@

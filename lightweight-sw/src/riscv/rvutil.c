#include "riscv/rvutil.h"

uint64_t rv64_get_cycles(void) {
  uint64_t cycles = 0;
  asm volatile("rdcycle %0" : "=r"(cycles));
  return cycles;
}

uint64_t rv64_get_time(void) {
  uint64_t time = 0;
  asm volatile("rdtime %0" : "=r"(time));
  return time;
}

uint64_t rv64_get_instret(void) {
  uint64_t instret = 0;
  asm volatile("rdinstret %0" : "=r"(instret));
  return instret;
}

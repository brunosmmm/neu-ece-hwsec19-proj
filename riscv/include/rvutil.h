#ifndef _RV_UTIL_H_INCLUDED_
#define _RV_UTIL_H_INCLUDED_

#ifndef RISCV64
#error "include only for cross-compilation for rv64 arch"
#endif

#include <stdint.h>

uint64_t rv64_get_cycles(void);
uint64_t rv64_get_time(void);
uint64_t rv64_get_instret(void);

#endif

#ifndef _SIMON_ROCC_H_INCLUDED_
#define _SIMON_ROCC_H_INCLUDED_

#ifndef RISCV64
#error "use with RISCV arch only"
#endif

#include "rocc.h"

#define ROCC_FUNC_OP_OFFSET 2
#define ROCC_FUNC_MODE_OFFSET 0

// toosly accelerator only supports 64/128 mode
#define SIMON_TOOSLY_ID 1
#define FUNC_INIT (0<<ROCC_FUNC_OP_OFFSET)
#define FUNC_ENC (1<<ROCC_FUNC_OP_OFFSET)
#define FUNC_DEC (2<<ROCC_FUNC_OP_OFFSET)
#define FUNC_CLOAD (3<<ROCC_FUNC_OP_OFFSET)
#define FUNC_CSTOR (4<<ROCC_FUNC_OP_OFFSET)

// define custom simon rocc instructions
#define TOOSLY_CIPHER_LOAD(addr, dest)                              \
  ROCC_INSTRUCTION_DSS(SIMON_TOOSLY_ID, dest, addr, 0, FUNC_CLOAD)

#define TOOSLY_CIPHER_STORE(addr, plain)                        \
  ROCC_INSTRUCTION_SS(SIMON_TOOSLY_ID, addr, plain, FUNC_CSTOR)

#define TOOSLY_ENC(addr, length)                                \
  ROCC_INSTRUCTION_SS(SIMON_TOOSLY_ID, addr, length, FUNC_ENC)

#define TOOSLY_DEC(addr, length)                                \
  ROCC_INSTRUCTION_SS(SIMON_TOOSLY_ID, addr, length, FUNC_DEC)

#define TOOSLY_INIT(kw1, kw2)                               \
  ROCC_INSTRUCTION_SS(SIMON_TOOSLY_ID, kw1, kw2, FUNC_INIT)

#endif

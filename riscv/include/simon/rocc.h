#ifndef _SIMON_ROCC_H_INCLUDED_
#define _SIMON_ROCC_H_INCLUDED_

#include <stdint.h>

#define ROCC_FUNC_OP_OFFSET 2
#define ROCC_FUNC_MODE_OFFSET 0

#define ROCC_FUNC_INIT (0 << ROCC_FUNC_OP_OFFSET)
#define ROCC_FUNC_ENC (1 << ROCC_FUNC_OP_OFFSET)
#define ROCC_FUNC_DEC (2 << ROCC_FUNC_OP_OFFSET)
#define ROCC_FUNC_FLAGS (3 << ROCC_FUNC_OP_OFFSET)
#define ROCC_FUNC_HWORD (4 << ROCC_FUNC_OP_OFFSET)

#define ROCC_MODE_64_128 0
#define ROCC_MODE_128_128 1

void simon_rocc_initialize(uint8_t *key);
uint64_t simon_rocc_64_128_encrypt(uint64_t * block);

#endif

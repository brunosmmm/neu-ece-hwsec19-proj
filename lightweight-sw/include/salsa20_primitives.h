#ifndef _SALSA20_PRIMITIVES_H_INCLUDED_
#define _SALSA20_PRIMITIVES_H_INCLUDED_

#include <stdint.h>

#define SALSA20_WORD_DTYPE uint32_t
#define SALSA20_WORD_DSIZE sizeof(SALSA20_WORD_DTYPE)

#define SALSA20_STATE_SIZE 16

extern const uint32_t* _salsa20_cons;

void _salsa20_initialize_state(uint32_t* key, uint32_t* nonce, uint32_t* state);
void _salsa20_qr(uint32_t* a, uint32_t* b, uint32_t*c, uint32_t* d);

#endif

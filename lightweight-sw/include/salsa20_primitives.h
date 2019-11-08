#ifndef _SALSA20_PRIMITIVES_H_INCLUDED_
#define _SALSA20_PRIMITIVES_H_INCLUDED_

#include <stdint.h>

#define SALSA20_WORD_DTYPE uint32_t
#define SALSA20_WORD_DSIZE sizeof(SALSA20_WORD_DTYPE)

#define SALSA20_STATE_SIZE 16
#define SALSA20_ROUNDS 20

extern const uint32_t* _salsa20_cons;

void _salsa20_initialize_state(uint32_t* key, uint32_t* nonce, uint32_t* state);
void _salsa20_qr(uint32_t* a, uint32_t* b, uint32_t*c, uint32_t* d);
void _salsa20_core(uint8_t *key_stream, uint32_t *state);
uint8_t _salsa20_encrypt_byte(uint8_t input, uint8_t * key_stream,
                              uint32_t * state);
uint64_t _salsa20_position(uint32_t *state);

#endif

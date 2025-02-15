#ifndef _SIMON_PRIMITIVES_H_INCLUDED_
#define _SIMON_PRIMITIVES_H_INCLUDED_

#include <stdint.h>

#define SIMON_64_128_ROUNDS 44 // number of rounds
#define SIMON_128_128_ROUNDS 68

// native data types for pointer arithmetic
#define SIMON_64_128_WORD_DTYPE uint32_t
#define SIMON_64_128_WORD_DSIZE sizeof(SIMON_64_128_WORD_DTYPE)
#define SIMON_64_128_KEY_WORDS (16 / SIMON_64_128_WORD_DSIZE)

#define SIMON_128_128_WORD_DTYPE uint64_t
#define SIMON_128_128_WORD_DSIZE sizeof(SIMON_128_128_WORD_DTYPE)
#define SIMON_128_128_KEY_WORDS (16 / SIMON_128_128_WORD_DSIZE)

// primitive simon definitions for direct usage
void _simon_64_128_enc_round(uint32_t round_key, uint32_t *x, uint32_t *y);
void _simon_64_128_dec_round(uint32_t round_key, uint32_t *x, uint32_t *y);

void _simon_128_128_enc_round(uint64_t round_key, uint64_t *x, uint64_t *y);
void _simon_128_128_dec_round(uint64_t round_key, uint64_t *x, uint64_t *y);

void _simon_64_128_key_expansion(uint8_t *key, uint32_t* round_keys);
void _simon_128_128_key_expansion(uint8_t *key, uint64_t* round_keys);

#endif

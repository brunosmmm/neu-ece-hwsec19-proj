#ifndef _SIMON_PRIMITIVES_H_INCLUDED_
#define _SIMON_PRIMITIVES_H_INCLUDED_

#include <stdint.h>

// primitive simon definitions for direct usage
void _simon_64_128_enc_round(uint32_t round_key, uint32_t *x, uint32_t *y);
void _simon_64_128_dec_round(uint32_t round_key, uint32_t *x, uint32_t *y);

void _simon_128_128_enc_round(uint64_t round_key, uint64_t *x, uint64_t *y);
void _simon_128_128_dec_round(uint64_t round_key, uint64_t *x, uint64_t *y);

void _simon_64_128_key_expansion(uint64_t *key, uint32_t* round_keys);
void _simon_128_128_key_expansion(uint64_t *key, uint64_t* round_keys);

#endif

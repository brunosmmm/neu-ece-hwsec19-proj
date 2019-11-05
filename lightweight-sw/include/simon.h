#ifndef _SIMON_H_INCLUDED_
#define _SIMON_H_INCLUDED_

// Simon 128 implementation
#include "lightweight.h"

void simon128_encrypt(key128_t key, block128_t *input, block128_t *output,
                      uint32_t block_count);

void simon128_decrypt(key128_t key, block128_t *input, block128_t *output,
                      uint32_t block_count);

#endif

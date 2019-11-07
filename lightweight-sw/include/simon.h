#ifndef _SIMON_H_INCLUDED_
#define _SIMON_H_INCLUDED_

// Simon 64/128 implementation
#include "lightweight.h"

void simon_64_128_initialize(key128_t key);
void simon_128_128_initialize(key128_t key);
int simon_64_128_encrypt(block64_t *input, block64_t *output);
int simon_64_128_decrypt(block64_t *input, block64_t *output);
int simon_128_128_encrypt(block128_t *input, block128_t *output);
int simon_128_128_decrypt(block128_t *input, block128_t *output);

#endif

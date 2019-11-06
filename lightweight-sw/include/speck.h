#ifndef _SPECK_H_INCLUDED_
#define _SPECK_H_INCLUDED_

#include "lightweight.h"

void speck_64_128_initialize(key128_t key);
int speck_64_128_encrypt(block64_t *input, block64_t *output);
int speck_64_128_decrypt(block64_t *input, block64_t *output);

#endif

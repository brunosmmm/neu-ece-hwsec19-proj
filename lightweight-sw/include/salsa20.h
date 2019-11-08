#ifndef _SALSA20_H_INCLUDED_
#define _SALSA20_H_INCLUDED_

#include "lightweight.h"

void salsa20_init(key256_t key, key64_t nonce);
void salsa20_encrypt_decrypt(uint8_t *in, uint8_t *out, uint32_t count);

#endif

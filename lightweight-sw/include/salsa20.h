#ifndef _SALSA20_H_INCLUDED_
#define _SALSA20_H_INCLUDED_

#include "lightweight.h"

void salsa20_init(key256_t key, key64_t nonce);

#endif

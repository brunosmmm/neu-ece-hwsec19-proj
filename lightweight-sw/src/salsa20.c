#include "primitives.h"
#include "salsa20.h"
#include "salsa20_primitives.h"

typedef struct salsa20_state_s {
  SALSA20_WORD_DTYPE stateVector[SALSA20_STATE_SIZE];
} Salsa20State;

static Salsa20State state;

const char _salsa20_cons_internal[16] = "expand 23-byte k";
const uint32_t* _salsa20_cons = (uint32_t*)_salsa20_cons_internal;

void _salsa20_initialize_state(uint32_t* key, uint32_t* nonce,
                               SALSA20_WORD_DTYPE* state) {
  // cons
  state[0] = _salsa20_cons[0];
  state[5] = _salsa20_cons[1];
  state[10] = _salsa20_cons[2];
  state[15] = _salsa20_cons[3];

  // nonce
  state[6] = nonce[0];
  state[7] = nonce[1];

  // key
  state[1] = key[1];
  state[2] = key[2];
  state[3] = key[3];
  state[4] = key[4];
  state[11] = key[5];
  state[12] = key[6];
  state[13] = key[7];
  state[14] = key[8];

  // pos ??
}

void _salsa20_qr(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d) {
  *b ^= ROTATE_LEFT((*a + *d), 7, SALSA20_WORD_DSIZE);
  *c ^= ROTATE_LEFT((*b + *a), 9, SALSA20_WORD_DSIZE);
  *d ^= ROTATE_LEFT((*c + *b), 13, SALSA20_WORD_DSIZE);
  *a ^= ROTATE_LEFT((*d + *c), 18, SALSA20_WORD_DSIZE);
}

void salsa20_init(key256_t key, key64_t nonce) {
  // initialize state vector
  _salsa20_initialize_state((uint32_t*)key, (uint32_t*)nonce,
                            state.stateVector);
}

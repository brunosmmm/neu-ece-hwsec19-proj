#include "primitives.h"
#include "salsa20.h"
#include "salsa20_primitives.h"
#include <string.h>

typedef struct salsa20_state_s {
  SALSA20_WORD_DTYPE stateVector[SALSA20_STATE_SIZE];
  uint8_t keyStream[SALSA20_STATE_SIZE*SALSA20_WORD_DSIZE];
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
  state[8] = 0;
  state[9] = 0;
}

void _salsa20_qr(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d) {
  *b ^= ROTATE_LEFT((*a + *d), 7, SALSA20_WORD_DSIZE);
  *c ^= ROTATE_LEFT((*b + *a), 9, SALSA20_WORD_DSIZE);
  *d ^= ROTATE_LEFT((*c + *b), 13, SALSA20_WORD_DSIZE);
  *a ^= ROTATE_LEFT((*d + *c), 18, SALSA20_WORD_DSIZE);
}

void _salsa20_core(uint8_t* key_stream, uint32_t* state) {
  unsigned int i = 0;
  uint32_t tmp[SALSA20_STATE_SIZE];

  memcpy(tmp, state, SALSA20_STATE_SIZE*sizeof(SALSA20_WORD_DTYPE));
  for (i = 0; i < SALSA20_ROUNDS; i+=2) {
    _salsa20_qr(&tmp[0], &tmp[4], &tmp[8], &tmp[12]);
    _salsa20_qr(&tmp[5], &tmp[9], &tmp[13], &tmp[1]);
    _salsa20_qr(&tmp[15], &tmp[3], &tmp[7], &tmp[11]);
    _salsa20_qr(&tmp[0], &tmp[1], &tmp[2], &tmp[3]);
    _salsa20_qr(&tmp[5], &tmp[6], &tmp[7], &tmp[4]);
    _salsa20_qr(&tmp[10], &tmp[11], &tmp[8], &tmp[9]);
    _salsa20_qr(&tmp[15], &tmp[12], &tmp[13], &tmp[14]);
  }
  for (i=0; i<16; i++) {
    ((uint32_t*)key_stream)[i] = tmp[i] + state[i];
  }
}

uint8_t _salsa20_encrypt_byte(uint8_t input, uint8_t* key_stream,
                              uint32_t* state) {
  uint8_t k_stream_byte = key_stream[*((uint64_t*)(&state[8])) % 64];
  // increment position counter
  *((uint64_t*)(&state[8])) += 1;
  return input ^ k_stream_byte;
}

uint64_t _salsa20_position(uint32_t* state) {
  return *((uint64_t*)&state[8]);
}

void salsa20_init(key256_t key, key64_t nonce) {
  // initialize state vector
  _salsa20_initialize_state((uint32_t*)key, (uint32_t*)nonce,
                            state.stateVector);
  // run first iteration of core function
  _salsa20_core(state.keyStream, state.stateVector);
}

void salsa20_encrypt_decrypt(uint8_t* in, uint8_t* out, uint32_t count) {
  unsigned int i = 0;
  uint64_t cur_pos = 0;
  for (i = 0; i < count; i++) {
    out[i] = _salsa20_encrypt_byte(in[i], state.keyStream, state.stateVector);
    cur_pos = _salsa20_position(state.stateVector);
    if (!(cur_pos%64) && (cur_pos > 0)) {
      _salsa20_core(state.keyStream, state.stateVector);
    }
  }
}

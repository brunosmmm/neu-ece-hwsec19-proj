#include "speck.h"
#include "primitives.h"
#include <string.h>

// cipher topology
#define SPECK_64_128_ROUNDS 27
#define SPECK_64_128_DTYPE uint32_t
#define SPECK_64_128_DSIZE sizeof(SPECK_64_128_DTYPE)
#define SPECK_64_128_ALPHA 8
#define SPECK_64_128_BETA 3

// internal state
#define SPECK_FLAG_INIT 0x1

typedef struct speck_state_s {
  SPECK_64_128_DTYPE round_keys[SPECK_64_128_ROUNDS];
  uint8_t flags;
} SpeckState;

// internal state
static SpeckState state;


static void _speck_64_128_enc_round(SPECK_64_128_DTYPE round_key,
                                    SPECK_64_128_DTYPE* x,
                                    SPECK_64_128_DTYPE* y) {
  *x = (ROTATE_RIGHT(*x, SPECK_64_128_ALPHA, SPECK_64_128_DSIZE) + *y)
    ^ round_key;
  *y = ROTATE_LEFT(*y, SPECK_64_128_BETA, SPECK_64_128_DSIZE) ^ *x;
}

static void _speck_64_128_dec_round(SPECK_64_128_DTYPE round_key,
                                    SPECK_64_128_DTYPE *x,
                                    SPECK_64_128_DTYPE *y) {

  *y = ROTATE_RIGHT((*x^*y), SPECK_64_128_BETA, SPECK_64_128_DSIZE);
  *x = ROTATE_LEFT(((*x ^ round_key) - *y), SPECK_64_128_ALPHA,
                   SPECK_64_128_DSIZE);
}

void speck_64_128_initialize(key128_t key) {
  unsigned int i = 0;
  SPECK_64_128_DTYPE l[SPECK_64_128_ROUNDS+SPECK_64_128_DSIZE-2];

  // initialize
  memset(&state, 0, sizeof(SpeckState));
  // set initial key words
  memset(l, 0, SPECK_64_128_DSIZE*(SPECK_64_128_ROUNDS+SPECK_64_128_DSIZE-2));
  memcpy(l, ((SPECK_64_128_DTYPE*)key)+1,
         sizeof(key128_t)-sizeof(SPECK_64_128_DTYPE));
  state.round_keys[0] = ((SPECK_64_128_DTYPE*)key)[0];

  // key expansion
  for (i=0; i<SPECK_64_128_ROUNDS-1; i++) {
    l[i+SPECK_64_128_DSIZE-1] = (state.round_keys[i] +
                                 ROTATE_RIGHT(l[i],
                                              SPECK_64_128_ALPHA,
                                              SPECK_64_128_DSIZE)) ^ i;
    state.round_keys[i+1] = ROTATE_LEFT(state.round_keys[i],
                                        SPECK_64_128_BETA,
                                        SPECK_64_128_DSIZE) ^
      l[i+SPECK_64_128_DSIZE-1];
  }
  state.flags = SPECK_FLAG_INIT;
}

int speck_64_128_encrypt(block64_t* input, block64_t* output) {
  unsigned int i = 0;
  SPECK_64_128_DTYPE x, y;
  if (!(state.flags & SPECK_FLAG_INIT)) {
    return CIPHER_ERR_ENC;
  }

  x = ((SPECK_64_128_DTYPE *)input)[0];
  y = ((SPECK_64_128_DTYPE *)input)[1];

  for (i = 0; i < SPECK_64_128_ROUNDS; i++) {
    _speck_64_128_enc_round(state.round_keys[i], &x, &y);
  }

  ((SPECK_64_128_DTYPE *)output)[0] = x;
  ((SPECK_64_128_DTYPE *)output)[1] = y;

  return CIPHER_ERR_OK;
}

int speck_64_128_decrypt(block64_t *input, block64_t *output) {
  unsigned int i = 0;
  SPECK_64_128_DTYPE x, y;
  if (!(state.flags & SPECK_FLAG_INIT)) {
    return CIPHER_ERR_DEC;
  }

  x = ((SPECK_64_128_DTYPE *)input)[0];
  y = ((SPECK_64_128_DTYPE *)input)[1];

  for (i = SPECK_64_128_ROUNDS; i > 0; i--) {
    _speck_64_128_dec_round(state.round_keys[i-1], &x, &y);
  }

  ((SPECK_64_128_DTYPE *)output)[0] = x;
  ((SPECK_64_128_DTYPE *)output)[1] = y;

  return CIPHER_ERR_OK;
}

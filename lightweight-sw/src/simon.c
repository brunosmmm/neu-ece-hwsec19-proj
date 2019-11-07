#include "simon.h"
#include "primitives.h"
#include <string.h>

// cipher topology
#define SIMON_64_128_ROUNDS 44 // number of rounds
#define SIMON_64_128_DTYPE uint32_t // use C types for easy handling
#define SIMON_64_128_DSIZE sizeof(SIMON_64_128_DTYPE) // word size in bytes
#define SIMON_64_128_KWORDS (16/SIMON_64_128_DSIZE)

#define SIMON_128_128_ROUNDS 68 // number of rounds
#define SIMON_128_128_DTYPE uint64_t // use C types for easy handling
#define SIMON_128_128_DSIZE sizeof(SIMON_128_128_DTYPE) // word size in bytes
#define SIMON_128_128_KWORDS (16/SIMON_128_128_DSIZE)

#define SIMON_MAX_ROUNDS SIMON_128_128_ROUNDS
#define SIMON_ROUNDK_DTYPE SIMON_128_128_DTYPE

// internal state flags
#define SIMON_FLAG_INIT 0x1

// z values
static const uint64_t Z[5] =
  {
   0b0001100111000011010100100010111110110011100001101010010001011111,
   0b0001011010000110010011111011100010101101000011001001111101110001,
   0b0011001101101001111110001000010100011001001011000000111011110101,
   0b0011110000101100111001010001001000000111101001100011010111011011,
   0b0011110111001001010011000011101000000100011011010110011110001011
  };

// internal state definition
typedef struct simon_state_s {
  SIMON_ROUNDK_DTYPE round_keys[SIMON_MAX_ROUNDS];
  uint8_t flags;
} SimonState;

// internal state
static SimonState state;

void _simon_64_128_enc_round(SIMON_64_128_DTYPE round_key,
                             SIMON_64_128_DTYPE* x,
                             SIMON_64_128_DTYPE* y) {
  SIMON_64_128_DTYPE tmp = 0;
  tmp = *x;
  *x = *y ^ (ROTATE_LEFT(*x, 1, SIMON_64_128_DSIZE) &
             ROTATE_LEFT(*x, 8, SIMON_64_128_DSIZE)) ^
    ROTATE_LEFT(*x, 2, SIMON_64_128_DSIZE) ^ round_key;
  *y = tmp;

}

void _simon_64_128_dec_round(SIMON_64_128_DTYPE round_key,
                             SIMON_64_128_DTYPE* x,
                             SIMON_64_128_DTYPE* y) {
  SIMON_64_128_DTYPE tmp = 0;
  tmp = *y;
  *y = *x ^ (ROTATE_LEFT(*y, 1, SIMON_64_128_DSIZE) &
             ROTATE_LEFT(*y, 8, SIMON_64_128_DSIZE)) ^
    ROTATE_LEFT(*y, 2, SIMON_64_128_DSIZE) ^ round_key;
  *x = tmp;
}

void _simon_128_128_enc_round(SIMON_128_128_DTYPE round_key,
                              SIMON_128_128_DTYPE *x,
                              SIMON_128_128_DTYPE *y) {
  SIMON_128_128_DTYPE tmp = 0;
  tmp = *x;
  *x = *y ^
       (ROTATE_LEFT(*x, 1, SIMON_128_128_DSIZE) &
        ROTATE_LEFT(*x, 8, SIMON_128_128_DSIZE)) ^
       ROTATE_LEFT(*x, 2, SIMON_128_128_DSIZE) ^ round_key;
  *y = tmp;
}

void _simon_128_128_dec_round(SIMON_128_128_DTYPE round_key,
                              SIMON_128_128_DTYPE *x,
                              SIMON_128_128_DTYPE *y) {
  SIMON_128_128_DTYPE tmp = 0;
  tmp = *y;
  *y = *x ^
       (ROTATE_LEFT(*y, 1, SIMON_128_128_DSIZE) &
        ROTATE_LEFT(*y, 8, SIMON_128_128_DSIZE)) ^
       ROTATE_LEFT(*y, 2, SIMON_128_128_DSIZE) ^ round_key;
  *x = tmp;
}

void _simon_64_128_key_expansion(key128_t key,
                                 SIMON_ROUNDK_DTYPE* round_keys) {
  unsigned int i = 0;
  SIMON_64_128_DTYPE kWordTmp = 0;
  memcpy(round_keys, (SIMON_64_128_DTYPE*)key,
         SIMON_64_128_KWORDS*SIMON_64_128_DSIZE);

  for (i = SIMON_64_128_KWORDS; i < SIMON_64_128_ROUNDS; i++) {
    kWordTmp = ROTATE_RIGHT(round_keys[i - 1], 3, SIMON_64_128_DSIZE);
    // key length is 128 bits, so m = 4
    kWordTmp ^= round_keys[i - 3];
    kWordTmp ^= ROTATE_RIGHT(kWordTmp, 1, SIMON_64_128_DSIZE);
    round_keys[i] = ~round_keys[i - SIMON_64_128_KWORDS] ^ kWordTmp ^
                     ((Z[3] >> ((i - SIMON_64_128_KWORDS) % 62)) & 1) ^ 3;
  }
}

void _simon_128_128_key_expansion(key128_t key,
                                         SIMON_128_128_DTYPE *round_keys) {
  unsigned int i = 0;
  SIMON_128_128_DTYPE kWordTmp = 0;
  memcpy(round_keys, (SIMON_128_128_DTYPE *)key,
         SIMON_128_128_KWORDS * SIMON_128_128_DSIZE);

  for (i = SIMON_128_128_KWORDS; i < SIMON_128_128_ROUNDS; i++) {
    kWordTmp = ROTATE_RIGHT(round_keys[i - 1], 3, SIMON_128_128_DSIZE);
    // key length is 128 bits, so m = 4
    kWordTmp ^= round_keys[i - 3];
    kWordTmp ^= ROTATE_RIGHT(kWordTmp, 1, SIMON_128_128_DSIZE);
    round_keys[i] = ~round_keys[i - SIMON_128_128_KWORDS] ^ kWordTmp ^
                    ((Z[4] >> ((i - SIMON_128_128_KWORDS) % 62)) & 1) ^ 3;
  }
}

static void _initialize_flags(void) {
  state.flags = SIMON_FLAG_INIT;
}

void simon_64_128_initialize(key128_t key) {
  // initialize
  memset(&state, 0, sizeof(SimonState));
  memcpy(state.round_keys,
         (SIMON_64_128_DTYPE*)key, sizeof(key128_t));
  // perform key expansion
  _simon_64_128_key_expansion(key, state.round_keys);
  _initialize_flags();
}

void simon_128_128_initialize(key128_t key) {
  // initialize
  memset(&state, 0, sizeof(SimonState));
  memcpy(state.round_keys, (SIMON_128_128_DTYPE *)key, sizeof(key128_t));
  // perform key expansion
  _simon_128_128_key_expansion(key, state.round_keys);
  _initialize_flags();
}

int simon_64_128_encrypt(block64_t* input, block64_t* output) {
  unsigned int i = 0;
  SIMON_64_128_DTYPE x, y;
  if (!(state.flags & SIMON_FLAG_INIT)) {
    return CIPHER_ERR_ENC;
  }

  x = ((SIMON_64_128_DTYPE*)input)[0];
  y = ((SIMON_64_128_DTYPE*)input)[1];

  for (i=0;i<SIMON_64_128_ROUNDS;i++) {
    _simon_64_128_enc_round(state.round_keys[i], &x, &y);
  }

  ((SIMON_64_128_DTYPE*)output)[0] = x;
  ((SIMON_64_128_DTYPE*)output)[1] = y;
  return CIPHER_ERR_OK;
}

int simon_64_128_decrypt(block64_t* input, block64_t* output) {
  unsigned int i = 0;
  SIMON_64_128_DTYPE x, y;
  if (!(state.flags & SIMON_FLAG_INIT)) {
    return CIPHER_ERR_DEC;
  }

  x = ((SIMON_64_128_DTYPE *)input)[0];
  y = ((SIMON_64_128_DTYPE *)input)[1];

  for (i = SIMON_64_128_ROUNDS; i > 0; i--) {
    _simon_64_128_dec_round(state.round_keys[i-1], &x, &y);
  }

  ((SIMON_64_128_DTYPE *)output)[0] = x;
  ((SIMON_64_128_DTYPE *)output)[1] = y;
  return CIPHER_ERR_OK;
}

int simon_128_128_encrypt(block128_t *input, block128_t *output) {
  unsigned int i = 0;
  SIMON_128_128_DTYPE x, y;
  if (!(state.flags & SIMON_FLAG_INIT)) {
    return CIPHER_ERR_ENC;
  }

  x = ((SIMON_128_128_DTYPE *)input)[0];
  y = ((SIMON_128_128_DTYPE *)input)[1];

  for (i = 0; i < SIMON_128_128_ROUNDS; i++) {
    _simon_128_128_enc_round(state.round_keys[i], &x, &y);
  }

  ((SIMON_128_128_DTYPE *)output)[0] = x;
  ((SIMON_128_128_DTYPE *)output)[1] = y;
  return CIPHER_ERR_OK;
}

int simon_128_128_decrypt(block128_t *input, block128_t *output) {
  unsigned int i = 0;
  SIMON_128_128_DTYPE x, y;
  if (!(state.flags & SIMON_FLAG_INIT)) {
    return CIPHER_ERR_DEC;
  }

  x = ((SIMON_128_128_DTYPE *)input)[0];
  y = ((SIMON_128_128_DTYPE *)input)[1];

  for (i = SIMON_128_128_ROUNDS; i > 0; i--) {
    _simon_128_128_dec_round(state.round_keys[i - 1], &x, &y);
  }

  ((SIMON_128_128_DTYPE *)output)[0] = x;
  ((SIMON_128_128_DTYPE *)output)[1] = y;
  return CIPHER_ERR_OK;
}

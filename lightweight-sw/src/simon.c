#include "simon.h"
#include <string.h>

// Simon primitives
#define ROTATE_LEFT(x, amount, dsize)\
  ((x<<amount)|(x>>((dsize*8)-amount)))
#define ROTATE_RIGHT(x, amount, dsize)\
  ((x>>amount)|((x&((1<<amount)-1)<<((dsize*8)-amount))))

// cipher topology
#define SIMON_64_128_ROUNDS 44 // number of rounds
#define SIMON_64_128_DTYPE uint32_t // use C types for easy handling
#define SIMON_64_128_DSIZE sizeof(uint32_t) // word size in bytes

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
  SIMON_64_128_DTYPE round_keys[SIMON_64_128_ROUNDS];
  uint8_t flags;
} SimonState;

// internal state
static SimonState state;

static void _simon_64_128_enc_round(SIMON_64_128_DTYPE round_key,
                                    SIMON_64_128_DTYPE* x,
                                    SIMON_64_128_DTYPE* y) {
  SIMON_64_128_DTYPE tmp = 0;
  tmp = *x;
  *x = *y ^ (ROTATE_LEFT(*x, 1, SIMON_64_128_DSIZE) &
             ROTATE_LEFT(*x, 8, SIMON_64_128_DSIZE)) ^
    ROTATE_LEFT(*x, 2, SIMON_64_128_DSIZE) ^ round_key;
  *y = tmp;

}

static void _simon_64_128_dec_round(SIMON_64_128_DTYPE round_key,
                                    SIMON_64_128_DTYPE* x,
                                    SIMON_64_128_DTYPE* y) {
  SIMON_64_128_DTYPE tmp = 0;
  tmp = *y;
  *y = *x ^ (ROTATE_LEFT(*y, 1, SIMON_64_128_DSIZE) &
             ROTATE_LEFT(*y, 8, SIMON_64_128_DSIZE)) ^
    ROTATE_LEFT(*y, 2, SIMON_64_128_DSIZE) ^ round_key;
  *x = tmp;
}

void simon_64_128_initialize(key128_t key) {
  unsigned int i = 0;
  SIMON_64_128_DTYPE kWordTmp = 0;
  SIMON_64_128_DTYPE *kWordNative = state.round_keys;
  // initialize
  memset(&state, 0, sizeof(SimonState));
  memcpy(state.round_keys,
         (SIMON_64_128_DTYPE*)key, sizeof(key128_t));

  // perform key expansion
  for (i=SIMON_64_128_DSIZE;i<SIMON_64_128_ROUNDS;i++) {
    kWordTmp = ROTATE_RIGHT(kWordNative[i-1], 3, SIMON_64_128_DSIZE);
    // key length is 128 bits, so m = 4
    kWordTmp ^= kWordNative[i-3];
    kWordTmp ^= ROTATE_RIGHT(kWordTmp, 1, SIMON_64_128_DSIZE);
    kWordNative[i] = ~kWordNative[i-SIMON_64_128_DSIZE] ^ kWordTmp
       ^ ((Z[3] >> ((i-SIMON_64_128_DSIZE)%62)) & 1) ^ 3;
  }

  state.flags = SIMON_FLAG_INIT;
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
    return CIPHER_ERR_ENC;
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

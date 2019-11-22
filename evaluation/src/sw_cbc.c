#include "common.h"
#include "simon.h"
#include "simon_primitives.h"

extern const uint8_t key[];
extern const uint8_t iv[];

uint64_t test_sw_cbc(unsigned int testSize, uint8_t *testData) {
  unsigned int i = 0;
  uint64_t cycles = 0;
  uint64_t tmp = 0;
  // perform initialization
  simon_64_128_initialize((uint8_t *)key);

  // encrypt
  cycles = rv64_get_cycles();

  for (i = 0; i < testSize; i += SIMON_64_128_BLOCK_SIZE) {
    if (i == 0) {
      tmp = *((uint64_t *)testData) ^ *((uint64_t *)iv);
    } else {
      tmp ^= *((uint64_t*)(testData+i));
    }
    simon_64_128_encrypt((block64_t*)&tmp,
                         (block64_t *)&tmp);
  }
  return rv64_get_cycles() - cycles;
}

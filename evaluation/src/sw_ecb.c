#include "simon_primitives.h"
#include "simon.h"
#include "common.h"

uint64_t test_sw_ecb(unsigned int testSize, uint8_t* testData) {
  unsigned int i = 0;
  uint64_t cycles = 0;
  // perform initialization
  simon_64_128_initialize((uint8_t *)key);

  // encrypt
  cycles = rv64_get_cycles();
  for (i = 0; i < testSize; i += SIMON_64_128_BLOCK_SIZE) {
    simon_64_128_encrypt(((block64_t *)testData + i),
                         (block64_t *)(testData + i));
  }
  return rv64_get_cycles() - cycles;
}

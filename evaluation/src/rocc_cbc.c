#include "common.h"
#include "rocc.h"
#include "rvutil.h"
#include "simon/rocc.h"

uint64_t test_rocc_cbc(unsigned int testSize, uint8_t *testData) {
  uint64_t tmp = 0, cycles = 0;
  uint64_t kw1, kw2;
  unsigned int i = 0;

  simon_rocc_initialize(key);

  cycles = rv64_get_cycles();
  for (i = 0; i < testSize; i += SIMON_64_128_BLOCK_SIZE) {
    if (i == 0) {
      tmp = *((uint64_t *)testData) ^ *((uint64_t *)iv);
    } else {
      tmp ^= simon_rocc_64_128_encrypt((uint64_t *)(testData + i));
      *((uint64_t *)(testData + i)) = tmp;
    }
  }

  return rv64_get_cycles() - cycles;
}

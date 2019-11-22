#include "common.h"
#include "rvutil.h"
#include "rocc.h"
#include "simon/rocc.h"

extern const uint8_t key[];
extern int err;

uint64_t test_rocc_ecb(unsigned int testSize, uint8_t *testData) {
  uint64_t tmp = 0, cycles = 0;
  uint64_t kw1, kw2;
  unsigned int i = 0;

  simon_rocc_initialize(key);

  cycles = rv64_get_cycles();
  for (i = 0; i < testSize; i += SIMON_64_128_BLOCK_SIZE) {
    *((uint64_t *)(testData + i)) =
        simon_rocc_64_128_encrypt((uint64_t *)(testData + i));
  }

  return rv64_get_cycles() - cycles;
}

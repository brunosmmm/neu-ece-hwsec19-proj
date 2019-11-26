#include "common.h"
#include "rocc.h"
#include "simon/rocc.h"

TestResult test_rocc_ecb(unsigned int testSize, uint8_t *testData) {
  unsigned int i = 0;
  TestResult result;

  simon_rocc_initialize((uint8_t*)key);

  test_start(&result);
  for (i = 0; i < testSize; i += SIMON_64_128_BLOCK_SIZE) {
    *((uint64_t *)(testData + i)) =
        simon_rocc_64_128_encrypt((uint64_t *)(testData + i));
  }

  test_end(&result);
  return result;
}

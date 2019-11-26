#include "common.h"
#include "rocc.h"
#include "simon/rocc.h"

TestResult test_rocc_cbc(unsigned int testSize, uint8_t *testData) {
  uint64_t tmp = 0;
  unsigned int i = 0;
  TestResult result;

  simon_rocc_initialize(key);

  test_start(&result);
  for (i = 0; i < testSize; i += SIMON_64_128_BLOCK_SIZE) {
    if (i == 0) {
      tmp = *((uint64_t *)testData) ^ *((uint64_t *)iv);
    } else {
      tmp ^= simon_rocc_64_128_encrypt((uint64_t *)(testData + i));
      *((uint64_t *)(testData + i)) = tmp;
    }
  }

  test_end(&result);
  return result;
}

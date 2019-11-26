#include "common.h"
#include "simon.h"
#include "simon_primitives.h"

TestResult test_sw_cbc(unsigned int testSize, uint8_t *testData) {
  unsigned int i = 0;
  uint64_t tmp = 0;
  TestResult result;
  // perform initialization
  simon_64_128_initialize((uint8_t *)key);

  // encrypt

  test_start(&result);
  for (i = 0; i < testSize; i += SIMON_64_128_BLOCK_SIZE) {
    if (i == 0) {
      tmp = *((uint64_t *)testData) ^ *((uint64_t *)iv);
    } else {
      tmp ^= *((uint64_t*)(testData+i));
    }
    simon_64_128_encrypt((block64_t*)&tmp,
                         (block64_t *)&tmp);
  }
  test_end(&result);
  return result;
}

#include "simon_primitives.h"
#include "simon.h"
#include "common.h"

TestResult test_sw_ecb(unsigned int testSize, uint8_t* testData) {
  unsigned int i = 0;
  TestResult result;
  // perform initialization
  simon_64_128_initialize((uint8_t *)key);

  // encrypt
  test_start(&result);
  for (i = 0; i < testSize; i += SIMON_64_128_BLOCK_SIZE) {
    simon_64_128_encrypt(((block64_t *)testData + i),
                         (block64_t *)(testData + i));
  }
  test_end(&result);
  return result;
}

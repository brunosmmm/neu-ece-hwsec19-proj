#include "common.h"
#include "simon/toosly.h"

TestResult test_toosly_ecb(unsigned int testSize, uint8_t *testData) {
  TestResult result;

  simon_toosly_initialize((uint8_t*)key);

  test_start(&result);
  // encrypt
  TOOSLY_ENC(testData, testSize);

  // serialize
  asm volatile("fence");
  test_end(&result);
  return result;
}

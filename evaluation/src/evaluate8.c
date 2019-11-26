#include "common.h"
#include "toosly_tests.h"

void do_test(void) {
  uint64_t kw1, kw2;
  TestResult result;
  kw1 = ((uint64_t *)key)[0];
  kw2 = ((uint64_t *)key)[1];

  printf("Testing ECB mode with toosly acc...");
  // initialize
  TOOSLY_INIT(kw1, kw2);

  // load test block
  // block = *((uint64_t *)test_block);
  test_start(&result);
  // perform encryption rounds
  TOOSLY_ENC(&testData, TEST_DATA_SIZE);

  asm volatile("fence");
  test_end(&result);
  printf(" done in %lu cycles, %lu instructions\n", result.cycles,
         result.instret);
}

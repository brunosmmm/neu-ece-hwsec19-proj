#include "common.h"
#include "toosly_tests.h"
#include "simon/toosly.h"

static uint8_t localData[10000];

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
  TOOSLY_ENC(localData, TEST_DATA_SIZE/8);

  asm volatile("fence");
  test_end(&result);
  printf(" done in %lu cycles, %lu instructions\n", result.cycles,
         result.instret);
}

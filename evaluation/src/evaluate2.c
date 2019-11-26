#include "common.h"
#include "sw_tests.h"

void do_test(void) {
  uint64_t cycles = 0;
  printf("Testing CBC mode in software...");
  cycles = test_sw_cbc(TEST_DATA_SIZE, testData);
  printf(" done in %lu cycles\n", cycles);
}

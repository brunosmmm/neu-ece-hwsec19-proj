#include "common.h"
#include "rocc_tests.h"

void do_test(void) {
  uint64_t cycles = 0;
  printf("Testing ECB mode using RoCC acc...");
  cycles = test_rocc_ecb(TEST_DATA_SIZE, testData);
  printf(" done in %lu cycles\n", cycles);
}

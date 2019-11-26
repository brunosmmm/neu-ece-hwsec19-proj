#include "common.h"
#include "rocc_tests.h"

void do_test(void) {
  TestResult result;
  printf("Testing ECB mode using RoCC acc...");
  result = test_rocc_ecb(TEST_DATA_SIZE, testData);
  printf(" done in %lu cycles, %lu instructions\n", result.cycles,
         result.instret);
}

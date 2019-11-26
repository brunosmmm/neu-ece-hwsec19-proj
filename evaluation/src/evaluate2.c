#include "common.h"
#include "sw_tests.h"

void do_test(void) {
  TestResult result;
  printf("Testing CBC mode in software...");
  result = test_sw_cbc(TEST_DATA_SIZE, testData);
  printf(" done in %lu cycles, %lu instructions\n", result.cycles,
         result.instret);
}

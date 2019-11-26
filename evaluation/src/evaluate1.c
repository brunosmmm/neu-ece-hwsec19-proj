#include "common.h"
#include "sw_tests.h"

void do_test(void) {
  TestResult result;
  printf("Testing ECB mode in software...");
  result = test_sw_ecb(TEST_DATA_SIZE, testData);
  printf(" done in %lu cycles, %lu instructions\n",
         result.cycles, result.instret);
}

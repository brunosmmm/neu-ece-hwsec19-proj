#include "common.h"
#include "toosly_tests.h"

void do_test(void) {
  TestResult result;
  printf("Testing ECB mode with toosly acc...");
  result = test_toosly_ecb();
  printf(" done in %lu cycles, %lu instructions\n", result.cycles,
         result.instret);
}

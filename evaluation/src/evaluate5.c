#include "common.h"
#include "mmio_tests.h"

void do_test(void) {
  TestResult result;
  printf("Testing ECB mode using MMIO Acc / single...");
  result = test_mmio_cbc_single(TEST_DATA_SIZE, testData);
  if (err) {
    printf(" MMIO accelerator not present, aborted\n");
  } else {
    printf(" done in %lu cycles, %lu instructions\n", result.cycles,
           result.instret);
  }
}

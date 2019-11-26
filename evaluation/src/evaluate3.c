#include "common.h"
#include "mmio_tests.h"

void do_test(void) {
  TestResult result;
  printf("Testing ECB mode using MMIO Acc / auto reload...");
  result = test_mmio_ecb_auto(TEST_DATA_SIZE, testData);
  if (err) {
    printf(" MMIO accelerator not present, aborted\n");
  } else {
    printf(" done in %lu cycles, %lu instructions\n", result.cycles,
           result.instret);
  }
}

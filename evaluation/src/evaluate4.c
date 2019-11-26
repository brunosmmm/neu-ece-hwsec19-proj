#include "common.h"
#include "mmio_tests.h"

void do_test(void) {
  uint64_t cycles = 0;
  printf("Testing ECB mode using MMIO Acc / single...");
  cycles = test_mmio_ecb_single(TEST_DATA_SIZE, testData);
  if (err) {
    printf(" MMIO accelerator not present, aborted\n");
  } else {
    printf(" done in %lu cycles\n", cycles);
  }
}

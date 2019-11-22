#include "common.h"
#include "mmio_tests.h"
#include "sw_tests.h"
#include "simon.h"

#define TEST_DATA_SIZE 1024

// Evaluation test #!
// Encrypt 1 kB of data

const uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                         0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                         0x0c, 0x0d, 0x0e, 0x0f};

const uint8_t iv[8] = {0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7};

// bogus data array
static uint8_t testData[TEST_DATA_SIZE];
int err = 0;

int main(void) {
  uint64_t cycles = 0;

  printf("Testing ECB mode in software...");
  cycles = test_sw_ecb(TEST_DATA_SIZE, testData);
  printf(" done in %lu cycles\n", cycles);

  printf("Testing CBC mode in software...");
  cycles = test_sw_cbc(TEST_DATA_SIZE, testData);
  printf(" done in %lu cycles\n", cycles);

  printf("Testing ECB mode using MMIO Acc / single...");
  cycles = test_mmio_ecb_single(TEST_DATA_SIZE, testData);
  if (err) {
    printf(" MMIO accelerator not present, aborted\n");
  } else {
    printf(" done in %lu cycles\n", cycles);
  }

  printf("Testing ECB mode using MMIO Acc / auto reload...");
  cycles = test_mmio_ecb_auto(TEST_DATA_SIZE, testData);
  if (err) {
    printf(" MMIO accelerator not present, aborted\n");
  } else {
    printf(" done in %lu cycles\n", cycles);
  }

  return 0;
}

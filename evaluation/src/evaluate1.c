#include <stdint.h>
#include "simon.h"
#include "rvutil.h"
#include "simon/mmio.h"
#include "common.h"
#include "mmio_tests.h"

#define TEST_DATA_SIZE 1024

// Evaluation test #!
// Encrypt 1 kB of data

const uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                         0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                         0x0c, 0x0d, 0x0e, 0x0f};

// bogus data array
static uint8_t testData[TEST_DATA_SIZE];
int err = 0;

static uint64_t test_sw_ecb(void) {
  unsigned int i = 0;
  uint64_t cycles = 0;
  // perform initialization
  simon_64_128_initialize((uint8_t *)key);

  // encrypt
  cycles = rv64_get_cycles();
  for (i=0;i<TEST_DATA_SIZE;i+=SIMON_64_128_BLOCK_SIZE) {
    simon_64_128_encrypt(((block64_t*)testData+i), (block64_t*)(testData+i));
  }
  return rv64_get_cycles() - cycles;
}

//#define DEBUG

int main(void) {
  uint64_t cycles = 0;

  #ifdef DEBUG
  volatile int wait = 1;
  while(wait);
  #endif

  printf("Testing ECB mode in software...");
  cycles = test_sw_ecb();
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

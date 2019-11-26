#include "common.h"
#include "rvutil.h"

// Evaluation test #!
// Encrypt 1 kB of data

const uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                         0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

const uint8_t iv[8] = {0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7};

// bogus data array
uint8_t testData[TEST_DATA_SIZE];
int err = 0;

inline void test_start(TestResult *data) {
  if (!data) return;
  data->cycles = rv64_get_cycles();
  data->instret = rv64_get_instret();
}

inline void test_end(TestResult *data) {
  uint64_t cycles, instret;
  if (!data) return;
  cycles = rv64_get_cycles();
  instret = rv64_get_instret();
  data->cycles = cycles - data->cycles;
  data->instret = instret - data->instret;
}

int main(void) {
  do_test();

  return 0;
}

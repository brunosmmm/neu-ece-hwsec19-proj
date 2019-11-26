#include "common.h"
#include "rvutil.h"
#include "simon/mmio.h"

uint64_t test_mmio_cbc_single(unsigned int testSize, uint8_t *testData) {
  uint64_t tmp = 0, cycles = 0;
  unsigned int i = 0;
#ifdef MMIO_CHECK
  tmp = simon_mmio_get_id();
  if (tmp != SIMON_ID) {
    err = -1;
    return 0;
  }
#endif

  // initialize acc
  simon_mmio_initialize((uint8_t *)key,
                        SIMON_MMIO_SCONF_SINGLE | SIMON_MMIO_SCONF_ENCDEC, 1);

  cycles = rv64_get_cycles();
  for (i = 0; i < testSize; i += SIMON_64_128_BLOCK_SIZE) {
    if (i == 0) {
      tmp = *((uint64_t *)testData) ^ *((uint64_t *)iv);
    } else {
      tmp ^= simon_mmio_64_128_encrypt_single((uint64_t *)(testData + i));
      *((uint64_t *)(testData + i)) = tmp;
    }
  }

  return rv64_get_cycles() - cycles;
}

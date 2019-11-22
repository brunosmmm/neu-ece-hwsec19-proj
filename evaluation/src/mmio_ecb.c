#include "simon/mmio.h"
#include "rvutil.h"
#include "common.h"

extern const uint8_t key[];
extern int err;

uint64_t test_mmio_ecb_single(unsigned int testSize, uint8_t* testData) {
  uint64_t tmp = 0, cycles = 0;
  unsigned int i = 0;
  tmp = simon_mmio_get_id();
  if (tmp!=SIMON_ID) {
    err = -1;
    return 0;
  }

  // initialize acc
  simon_mmio_initialize((uint8_t *)key,
                        SIMON_MMIO_SCONF_SINGLE | SIMON_MMIO_SCONF_ENCDEC,
                        1);

  cycles = rv64_get_cycles();
  for (i=0;i<testSize;i+=SIMON_64_128_BLOCK_SIZE) {
    *((uint64_t*)(testData+i)) =
      simon_mmio_64_128_encrypt_single((uint64_t*)(testData+i));
  }

  return rv64_get_cycles() - cycles;
}

uint64_t test_mmio_ecb_auto(unsigned int testSize, uint8_t* testData) {
  uint64_t tmp = 0, cycles = 0;
  unsigned int i = 0;
  tmp = simon_mmio_get_id();
  if (tmp != SIMON_ID) {
    err = -1;
    return 0;
  }

  // initialize acc
  simon_mmio_initialize((uint8_t *)key,
                        SIMON_MMIO_SCONF_SINGLE | SIMON_MMIO_SCONF_ENCDEC, 1);

  cycles = rv64_get_cycles();
  for (i = 0; i <testSize; i += SIMON_64_128_BLOCK_SIZE) {
    *((uint64_t *)(testData + i)) =
        simon_mmio_64_128_encrypt_auto((uint64_t *)(testData + i));
  }

  return rv64_get_cycles() - cycles;
}

#include "simon/mmio.h"
#include "rvutil.h"
#include "common.h"

TestResult test_mmio_ecb_single(unsigned int testSize, uint8_t* testData) {
  unsigned int i = 0;
  TestResult result;
#ifdef MMIO_CHECK
  tmp = simon_mmio_get_id();
  if (tmp!=SIMON_ID) {
    err = -1;
    return 0;
  }
#endif

  // initialize acc
  simon_mmio_initialize((uint8_t *)key,
                        SIMON_MMIO_SCONF_SINGLE | SIMON_MMIO_SCONF_ENCDEC,
                        1);

  test_start(&result);
  for (i=0;i<testSize;i+=SIMON_64_128_BLOCK_SIZE) {
    *((uint64_t*)(testData+i)) =
      simon_mmio_64_128_encrypt_single((uint64_t*)(testData+i));
  }
  test_end(&result);
  return result;
}

TestResult test_mmio_ecb_auto(unsigned int testSize, uint8_t* testData) {
  unsigned int i = 0;
  TestResult result;
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

  test_start(&result);
  for (i = 0; i <testSize; i += SIMON_64_128_BLOCK_SIZE) {
    *((uint64_t *)(testData + i)) =
        simon_mmio_64_128_encrypt_auto((uint64_t *)(testData + i));
  }
  test_end(&result);
  return result;
}

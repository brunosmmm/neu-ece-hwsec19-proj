#include "simon/rocc.h"
#include "rocc.h"

void simon_rocc_initialize(uint8_t* key) {
  uint64_t kw1, kw2;
  kw1 = ((uint64_t *)key)[0];
  kw2 = ((uint64_t *)key)[1];

  // initialize
  ROCC_INSTRUCTION_SS(0, kw1, kw2, ROCC_FUNC_INIT);
}

uint64_t simon_rocc_64_128_encrypt(uint64_t* block) {
  uint64_t tmp = 0;
  unsigned int i = 0;

  tmp = *block;

  // perform encryption rounds
  for (i = 0; i < 44; i++) {
    ROCC_INSTRUCTION_DSS(0, tmp, tmp, 0, ROCC_FUNC_ENC);
  }
  return tmp;
}

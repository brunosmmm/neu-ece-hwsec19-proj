#include "simon_primitives.h"
#include "simon.h"
#include "util.h"
#include <stdint.h>

const static key128_t test_key = {0x80, 0x01, 0x02, 0xFF, 0x2A, 0xAA,
                                  0x42, 0x00, 0x11, 0x30, 0xF9, 0xA4,
                                  0xBB, 0x09, 0xAB, 0x56};

static SIMON_64_128_WORD_DTYPE expandedKey[SIMON_64_128_ROUNDS];

int main(void) {
  unsigned int i = 0;

  // expand key
  _simon_64_128_key_expansion((uint8_t*)test_key, expandedKey);

  printf("INFO: dumping expanded key\n");
  for (i = 0; i < SIMON_64_128_ROUNDS; i++) {
    printf("%02d: 0x%x\n", i, expandedKey[i]);
  }

  return 0;
}

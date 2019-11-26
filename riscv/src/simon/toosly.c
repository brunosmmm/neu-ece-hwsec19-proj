#include "simon/toosly.h"

void simon_toosly_initialize(uint8_t *key) {
  uint64_t kw1, kw2;
  kw1 = ((uint64_t *)key)[0];
  kw2 = ((uint64_t *)key)[1];

  // initialize
  TOOSLY_INIT(kw1, kw2);
}

// this is ECB only
void simon_toosly_64_128_encrypt(uint64_t *start, uint64_t len) {
  // encrypt
  TOOSLY_ENC(start, len);

  //serialize
  asm volatile("fence");
}

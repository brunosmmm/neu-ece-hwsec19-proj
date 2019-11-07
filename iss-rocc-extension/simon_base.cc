#include "simon_base.hh"
#include <cstring>

simon_base_hwacc::simon_base_hwacc() {
  this->flags = 0;
  this->round_counter = 0;
  memset(&this->round_keys, 0, sizeof(uint64_t) * SIMON_MAX_ROUNDS);
}

void simon_base_hwacc::initialize(uint64_t keyWord1, uint64_t keyWord2) {
  uint8_t mode = this->get_mode();
  uint8_t key[16];
  // assemble key
  memcpy(key, &keyWord1, 8);
  memcpy(key + 8, &keyWord2, 8);
  // set appropriate round count
  switch (mode) {
  case SIMON_64_128_MODE:
    _simon_64_128_key_expansion(key, this->round_keys.k64_128);
    break;
  case SIMON_128_128_MODE:
    _simon_128_128_key_expansion(key, this->round_keys.k128_128);
    break;
  default:
    return;
  }
  this->round_counter = 0;
  this->flags |= SIMON_FLAG_INITIALIZED;
}

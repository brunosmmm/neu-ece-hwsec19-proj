#include "salsa20_base.hh"
#include <cstring>

salsa20_base_hwacc::salsa20_base_hwacc() {
  this->flags = 0;
  memset(&this->key_stream, 0, SALSA20_ROCC_KSTREAM_SIZE*sizeof(uint64_t));

  // initialize state without key, nonce
  _salsa20_initialize_state(0, 0, this->state);
}

void salsa20_base_hwacc::setkey_high(uint64_t kw4, uint64_t kw3) {
  *((uint64_t*)&this->state[13]) = kw4;
  *((uint64_t *)&this->state[11]) = kw3;
}

void salsa20_base_hwacc::setkey_low(uint64_t kw2, uint64_t kw1) {
  *((uint64_t *)&this->state[3]) = kw2;
  *((uint64_t *)&this->state[1]) = kw1;
}

void salsa20_base_hwacc::setnonce(uint64_t nonce) {
  *((uint64_t *)&this->state[6]) = nonce;
}

void salsa20_base_hwacc::incr_pos(void) {
  *((uint64_t*)&this->state[8]) += 1;
}

void salsa20_base_hwacc::reset_pos(void) {
  *((uint64_t *)&this->state[8]) = 0;
}

void salsa20_base_hwacc::refresh_stream(void) {
  _salsa20_core((uint8_t*)this->key_stream, this->state);
}

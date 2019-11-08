#include <stdint.h>
#include <string>

extern "C" {
#include "salsa20_primitives.h"
}

#define SALSA20_ROCC_KSTREAM_SIZE                                              \
  ((SALSA20_STATE_SIZE * SALSA20_WORD_DSIZE) / sizeof(uint64_t))

class salsa20_base_hwacc {
public:
  salsa20_base_hwacc();
private:
  uint64_t flags;
  SALSA20_WORD_DTYPE state[SALSA20_STATE_SIZE];
  uint64_t key_stream[SALSA20_ROCC_KSTREAM_SIZE];

  // salsa20 key is 256-bit. we need two operations to set it
  void setkey_high(uint64_t kw4, uint64_t kw3);
  void setkey_low(uint64_t kw1, uint64_t kw2);

  // salsa20 nonce is 64-bit. user must change it every 2^64 bytes?
  void setnonce(uint64_t nonce);

  // increment position
  void incr_pos(void);

  // reset position
  void reset_pos(void);

  // calculate new keystream
  void refresh_stream(void);
};

#include <stdint.h>
#include <string>

extern "C" {
#include "simon_primitives.h"
}

// two modes of operation will be allowed: Simon64/128 and Simon128/128
#define SIMON_64_128_MODE 0x0
#define SIMON_128_128_MODE 0x1

// internal flags
#define SIMON_INTERNAL_FLAGS_MASK 0xFFFF
#define SIMON_INTERNAL_MODE_MASK 0xFFFF0000
#define SIMON_INTERNAL_FLAGS_OFFSET 0
#define SIMON_INTERNAL_MODE_OFFSET 32

// internal state flags - half of flags register
#define SIMON_FLAG_INITIALIZED 0x1
#define SIMON_FLAG_DONE 0x2

#define SIMON_MAX_ROUNDS SIMON_128_128_ROUNDS

class simon_base_hwacc {
public:
  simon_base_hwacc();
protected:
  union {
    uint32_t k64_128[SIMON_64_128_ROUNDS];
    uint64_t k128_128[SIMON_128_128_ROUNDS];
  } round_keys;
  uint64_t flags;
  uint64_t round_counter;
  uint64_t tmp_buffer;

  void initialize(uint64_t keyWord1, uint64_t keyWord2);

  uint8_t get_mode(void) {
    return (this->flags & SIMON_INTERNAL_MODE_MASK) >>
           SIMON_INTERNAL_MODE_OFFSET;
  }

  void debug_print(std::string str) {
    fprintf(stderr, "SIMON_ROCC: %s\n", str.c_str());
  }
};

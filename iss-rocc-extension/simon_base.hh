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
#define SIMON_FLAG_READY 0x2
#define SIMON_FLAG_ERR 0x4

#define SIMON_MAX_ROUNDS SIMON_128_128_ROUNDS

// funct field subfields
#define SIMON_FUNCT_MODE_OFFSET 0
#define SIMON_FUNCT_OP_OFFSET 2
#define SIMON_FUNCT_MODE_MASK                                                  \
  ((1 << SIMON_FUNCT_MODE_OFFSET) | (1 << (SIMON_FUNCT_MODE_OFFSET + 1)))
#define SIMON_FUNCT_OP_MASK                                                    \
  ((1 << SIMON_FUNCT_OP_OFFSET) | (1 << (SIMON_FUNCT_OP_OFFSET + 1)))

// supported functions
#define FUNC_INIT 0x0      // initialize with key
#define FUNC_ENC_ROUND 0x1 // perform encryption round
#define FUNC_DEC_ROUND 0x2 // perform decryption round
#define FUNC_GET_FLAGS 0x3 // retrieve status
#define FUNC_GET_HWORD 0x4 // get stored half-word (for 128/128 mode)

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

#include "rocc.h"
#include <string>
#include <cstring>
#include <cstdlib>

extern "C" {
#include "simon_primitives.h"
}

// funct field subfields
#define SIMON_FUNCT_MODE_OFFSET 0
#define SIMON_FUNCT_OP_OFFSET 2
#define SIMON_FUNCT_MODE_MASK ((1<<SIMON_FUNCT_MODE_OFFSET) | (1<<(SIMON_FUNCT_MODE_OFFSET+1)))
#define SIMON_FUNCT_OP_MASK ((1<<SIMON_FUNCT_OP_OFFSET) | (1<<(SIMON_FUNCT_OP_OFFSET+1)))

// two modes of operation will be allowed: Simon64/128 and Simon128/128
#define SIMON_64_128_MODE 0x0
#define SIMON_128_128_MODE 0x1

// supported functions
#define FUNC_INIT 0x0       // initialize with key
#define FUNC_ENC_ROUND 0x1  // perform encryption round
#define FUNC_DEC_ROUND 0x2  // perform decryption round
#define FUNC_GET_FLAGS 0x3  // retrieve status
#define FUNC_GET_HWORD 0x4  // get stored half-word (for 128/128 mode)

// Simon 64/128 definitions
#define SIMON_MAX_ROUNDS SIMON_128_128_ROUNDS

// internal flags
#define SIMON_INTERNAL_FLAGS_MASK 0xFFFF
#define SIMON_INTERNAL_MODE_MASK 0xFFFF0000
#define SIMON_INTERNAL_FLAGS_OFFSET 0
#define SIMON_INTERNAL_MODE_OFFSET 32

// internal state flags - half of flags register
#define SIMON_FLAG_INITIALIZED 0x1
#define SIMON_FLAG_DONE 0x2


class crypto_rocc_t : public rocc_t
{
 public:
  const char* name() { return "crypto_rocc"; }

  reg_t custom0(rocc_insn_t insn, reg_t xs1, reg_t xs2)
  {
    uint8_t operation = (insn.funct & SIMON_FUNCT_OP_MASK) >> SIMON_FUNCT_OP_OFFSET;
    reg_t ret_val = 0;
    switch (operation)
    {
    case FUNC_INIT:
      {
        uint8_t mode = (insn.funct & SIMON_FUNCT_MODE_MASK);
        // initialize accelerator, internally performs key expansion
        if ((mode == SIMON_64_128_MODE) || (mode == SIMON_128_128_MODE)) {
          // store mode
          this->flags = ((uint64_t)mode << SIMON_INTERNAL_MODE_OFFSET);
          // key words (64 bits) from registers passed in
          this->initialize(xs1, xs2);
          // return number of rounds for convenience
          if (mode == SIMON_64_128_MODE) {
            ret_val = SIMON_64_128_ROUNDS;
          } else {
            ret_val = SIMON_128_128_ROUNDS;
          }
        } else {
          // illegal mode
          std::string msg = std::string("FUNC_INIT: illegal mode ") + std::to_string(mode);
          debug_print(msg);
          illegal_instruction();
        }
        break;
      }
    case FUNC_ENC_ROUND:
      if (!(this->flags & SIMON_FLAG_INITIALIZED)) {
        std::string msg =
            std::string("FUNC_ENC_ROUND: acc not initialized");
        debug_print(msg);
        illegal_instruction();
      } else {
        if (this->get_mode() == SIMON_64_128_MODE) {
          // only one register needed
          SIMON_64_128_WORD_DTYPE x, y;
          x = ((SIMON_64_128_WORD_DTYPE*)xs1)[0];
          y = ((SIMON_64_128_WORD_DTYPE*)xs1)[1];
          _simon_64_128_enc_round(this->round_keys.k64_128[this->round_counter++],
                                  &x,
                                  &y);
          ((SIMON_64_128_WORD_DTYPE*)ret_val)[0] = x;
          ((SIMON_64_128_WORD_DTYPE *)ret_val)[0] = y;
        } else {
          SIMON_128_128_WORD_DTYPE x, y;
          // two registers needed
          x = xs1;
          y = xs2;
          _simon_128_128_enc_round(this->round_keys.k128_128[this->round_counter++],
                                   &x,
                                   &y);
          // in this case we can only return half of the block, so we store it internally
          ret_val = x;
          this->tmp_buffer = y;
        }
      }
      break;
    case FUNC_DEC_ROUND:
      if (!(this->flags & SIMON_FLAG_INITIALIZED)) {
        std::string msg = std::string("FUNC_DEC_ROUND: acc not initialized");
        debug_print(msg);
        illegal_instruction();
      } else {
        if (this->get_mode() == SIMON_64_128_MODE) {
          SIMON_64_128_WORD_DTYPE x, y;
          x = ((SIMON_64_128_WORD_DTYPE *)xs1)[0];
          y = ((SIMON_64_128_WORD_DTYPE *)xs1)[1];
          _simon_64_128_dec_round(this->round_keys.k64_128[(SIMON_64_128_ROUNDS-1)-this->round_counter++],
                                  &x,
                                  &y);
          ((SIMON_64_128_WORD_DTYPE *)ret_val)[0] = x;
          ((SIMON_64_128_WORD_DTYPE *)ret_val)[0] = y;
        } else {
          SIMON_128_128_WORD_DTYPE x, y;
          // two registers needed
          x = xs1;
          y = xs2;
          _simon_128_128_dec_round(this->round_keys.k128_128[(SIMON_128_128_ROUNDS-1)-this->round_counter++],
                                   &x,
                                   &y);
          // in this case we can only return half of the block, so we store it
          // internally
          ret_val = x;
          this->tmp_buffer = y;
        }
      }
      break;
    case FUNC_GET_FLAGS:
      ret_val = this->flags;
      break;
    case FUNC_GET_HWORD:
      if (!(this->flags & SIMON_FLAG_INITIALIZED) || (this->get_mode() != SIMON_128_128_MODE)) {
        std::string msg = std::string("FUNC_GET_HWORD: acc not initialized or not in 128/128 mode");
        debug_print(msg);
        illegal_instruction();
      } else {
        ret_val = this->tmp_buffer;
      }
      break;
    default:
      std::string msg = std::string("illegal function: ") + std::to_string(operation);
      debug_print(msg);
      illegal_instruction();
      break;
    }

    if (this->get_mode() == SIMON_64_128_MODE) {
      if (this->round_counter == SIMON_64_128_ROUNDS) {
        this->flags |= SIMON_FLAG_DONE;
        this->round_counter = 0;
      }
    } else if (this->get_mode() == SIMON_128_128_MODE) {
      if (this->round_counter == SIMON_64_128_ROUNDS) {
        this->flags |= SIMON_FLAG_DONE;
        this-> round_counter = 0;
      }
    }

    return ret_val;
  }

  crypto_rocc_t()
  {
    this->flags = 0;
    this->round_counter = 0;
    this->tmp_buffer = 0;
    memset(&this->round_keys, 0, sizeof(uint64_t)*SIMON_MAX_ROUNDS);
  }

private:
  // 64 bit registers match architecture and are maximum word size in any case
  // register count is maximal for mode that requires more rounds
  union {
    uint32_t k64_128[SIMON_64_128_ROUNDS];
    uint64_t k128_128[SIMON_128_128_ROUNDS];
  } round_keys;
  uint64_t flags;
  uint64_t round_counter;
  uint64_t tmp_buffer;

  void initialize(uint64_t keyWord1, uint64_t keyWord2) {
    uint8_t mode = this->get_mode();
    uint8_t key[16];
    // assemble key
    memcpy(key, &keyWord1, 8);
    memcpy(key+8, &keyWord2, 8);
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

  uint8_t get_mode(void) {
    return (this->flags & SIMON_INTERNAL_MODE_MASK) >> SIMON_INTERNAL_MODE_OFFSET;
  }

  void debug_print(std::string str) {
    fprintf(stderr, "SIMON_ROCC: %s\n", str.c_str());
  }

};


REGISTER_EXTENSION(simon_rocc, []() { return new crypto_rocc_t; })

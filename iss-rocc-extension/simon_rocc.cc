#include "rocc.h"
#include <string>
#include <cstring>
#include <cstdlib>

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
#define SIMON_64_128_ROUNDS 44
#define SIMON_128_128_ROUNDS 68
#define SIMON_MAX_ROUNDS SIMON_128_128_ROUNDS

// internal flags
#define SIMON_INTERNAL_FLAGS_MASK 0xFFFF
#define SIMON_INTERNAL_MODE_MASK 0xFFFF0000
#define SIMON_INTERNAL_FLAGS_OFFSET 0
#define SIMON_INTERNAL_MODE_OFFSET 32

// internal state flags - half of flags register
#define SIMON_FLAG_INITIALIZED 0x1
#define SIMON_FLAG_DONE 0x2

// native data types for pointer arithmetic
#define SIMON_64_128_WORD_DTYPE uint32_t
#define SIMON_64_128_WORD_DSIZE sizeof(SIMON_64_128_WORD_DTYPE)
#define SIMON_64_128_KEY_WORDS (16/SIMON_64_128_WORD_DSIZE)

#define SIMON_128_128_WORD_DTYPE uint64_t
#define SIMON_128_128_WORD_DSIZE sizeof(SIMON_128_128_WORD_DTYPE)
#define SIMON_128_128_KEY_WORDS (16/SIMON_128_128_WORD_DSIZE)

// primitives
#define ROTATE_LEFT(x, amount, dsize)                                          \
  ((x << amount) | (x >> ((dsize * 8) - amount)))
#define ROTATE_RIGHT(x, amount, dsize)                                         \
  ((x >> amount) | ((x & ((1 << amount) - 1) << ((dsize * 8) - amount))))

static const uint64_t Z[5] = {
    0b0001100111000011010100100010111110110011100001101010010001011111,
    0b0001011010000110010011111011100010101101000011001001111101110001,
    0b0011001101101001111110001000010100011001001011000000111011110101,
    0b0011110000101100111001010001001000000111101001100011010111011011,
    0b0011110111001001010011000011101000000100011011010110011110001011};

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
          this->expand_key(xs1, xs2);
          // return number of rounds for convenience
          if (mode == SIMON_64_128_MODE) {
            ret_val = SIMON_64_128_ROUNDS;
          } else {
            ret_val = SIMON_128_128_ROUNDS;
          }
        } else {
          // illegal mode
          std::string msg = std::string("illegal mode ") + std::to_string(mode);
          debug_print(msg);
          illegal_instruction();
        }
        break;
      }
    case FUNC_ENC_ROUND:
      if (!(this->flags & SIMON_FLAG_INITIALIZED)) {
        illegal_instruction();
      } else {
        if (this->get_mode() == SIMON_64_128_MODE) {
          // only one register needed
          SIMON_64_128_WORD_DTYPE x, y;
          x = ((SIMON_64_128_WORD_DTYPE*)xs1)[0];
          y = ((SIMON_64_128_WORD_DTYPE*)xs1)[1];
          this->enc_round_64_128(this->round_keys[this->round_counter++], &x,
                                 &y);
          ((SIMON_64_128_WORD_DTYPE*)ret_val)[0] = x;
          ((SIMON_64_128_WORD_DTYPE *)ret_val)[0] = y;
        } else {
          SIMON_128_128_WORD_DTYPE x, y;
          // two registers needed
          x = xs1;
          y = xs2;
          this->enc_round_128_128(this->round_keys[this->round_counter++], &x,
                                 &y);
          // in this case we can only return half of the block, so we store it internally
          ret_val = x;
          this->tmp_buffer = y;
        }
      }
      break;
    case FUNC_DEC_ROUND:
      if (!(this->flags & SIMON_FLAG_INITIALIZED)) {
        illegal_instruction();
      } else {
        if (this->get_mode() == SIMON_64_128_MODE) {
          SIMON_64_128_WORD_DTYPE x, y;
          x = ((SIMON_64_128_WORD_DTYPE *)xs1)[0];
          y = ((SIMON_64_128_WORD_DTYPE *)xs1)[1];
          this->dec_round_64_128(this->round_keys[(SIMON_64_128_ROUNDS-1)-this->round_counter++], &x,
                                 &y);
          ((SIMON_64_128_WORD_DTYPE *)ret_val)[0] = x;
          ((SIMON_64_128_WORD_DTYPE *)ret_val)[0] = y;
        } else {
          SIMON_128_128_WORD_DTYPE x, y;
          // two registers needed
          x = xs1;
          y = xs2;
          this->dec_round_128_128(this->round_keys[(SIMON_128_128_ROUNDS-1)-this->round_counter++], &x,
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
        illegal_instruction();
      } else {
        ret_val = this->tmp_buffer;
      }
      break;
    default:
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
  uint64_t round_keys[SIMON_MAX_ROUNDS];
  uint64_t flags;
  uint64_t round_counter;
  uint64_t tmp_buffer;

  void expand_key(uint64_t keyWord1, uint64_t keyWord2) {
    uint8_t mode = this->get_mode();
    uint8_t rounds = 0;
    uint8_t key_words = 0;
    unsigned int i = 0;
    // set appropriate round count
    switch (mode) {
    case SIMON_64_128_MODE:
      rounds = SIMON_64_128_ROUNDS;
      key_words = SIMON_64_128_KEY_WORDS;
      round_keys[0] = ((SIMON_64_128_WORD_DTYPE*)(&keyWord1))[0];
      round_keys[1] = ((SIMON_64_128_WORD_DTYPE *)(&keyWord1))[1];
      round_keys[2] = ((SIMON_64_128_WORD_DTYPE *)(&keyWord2))[0];
      round_keys[3] = ((SIMON_64_128_WORD_DTYPE *)(&keyWord2))[1];
      break;
    case SIMON_128_128_MODE:
      rounds = SIMON_128_128_ROUNDS;
      key_words = SIMON_128_128_KEY_WORDS;
      round_keys[0] = keyWord1;
      round_keys[1] = keyWord2;
      break;
    default:
      return;
    }

    // perform key expansion
    for (i = key_words; i < rounds; i++) {
      if (mode == SIMON_64_128_MODE) {
        SIMON_64_128_WORD_DTYPE kTmp = 0;
        kTmp = ROTATE_RIGHT(((SIMON_64_128_WORD_DTYPE*)this->round_keys)[i - 1],
                            3, SIMON_64_128_WORD_DSIZE);
        kTmp ^= ((SIMON_64_128_WORD_DTYPE*)this->round_keys)[i-3];
        kTmp ^= ROTATE_RIGHT(kTmp, 1, SIMON_64_128_WORD_DSIZE);
        ((SIMON_64_128_WORD_DTYPE *)this->round_keys)[i] =
            ~((SIMON_64_128_WORD_DTYPE *)this->round_keys)[i-SIMON_64_128_KEY_WORDS] ^ kTmp ^
          ((Z[3] >> ((i-SIMON_64_128_KEY_WORDS)%62)) & 1) ^ 3;
            continue;
      }

      if (mode == SIMON_128_128_MODE) {
        SIMON_128_128_WORD_DTYPE kTmp = 0;
        kTmp = ROTATE_RIGHT(((SIMON_128_128_WORD_DTYPE *)this->round_keys)[i - 1],
                            3, SIMON_128_128_WORD_DSIZE);
        kTmp ^= ((SIMON_128_128_WORD_DTYPE *)this->round_keys)[i - 3];
        kTmp ^= ROTATE_RIGHT(kTmp, 1, SIMON_128_128_WORD_DSIZE);
        ((SIMON_64_128_WORD_DTYPE *)this->round_keys)[i] =
            ~((SIMON_128_128_WORD_DTYPE *)this
                  ->round_keys)[i - SIMON_128_128_KEY_WORDS] ^
            kTmp ^ ((Z[2] >> ((i - SIMON_128_128_KEY_WORDS) % 62)) & 1) ^ 3;

        continue;
      }
    }
    this->round_counter = 0;
    this->flags |= SIMON_FLAG_INITIALIZED;
  }

  void enc_round_64_128(SIMON_64_128_WORD_DTYPE roundKey,
                        SIMON_64_128_WORD_DTYPE* x,
                        SIMON_64_128_WORD_DTYPE* y) {
    SIMON_64_128_WORD_DTYPE tmp = *x;
    *x = *y ^
      (ROTATE_LEFT(*x, 1, SIMON_64_128_WORD_DSIZE) &
       ROTATE_LEFT(*x, 8, SIMON_64_128_WORD_DSIZE)) ^
      ROTATE_LEFT(*x, 2, SIMON_64_128_WORD_DSIZE) ^ roundKey;
    *y = tmp;
  }

  void dec_round_64_128(SIMON_64_128_WORD_DTYPE round_key,
                        SIMON_64_128_WORD_DTYPE *x,
                        SIMON_64_128_WORD_DTYPE *y) {
    SIMON_64_128_WORD_DTYPE tmp = *y;
    *y = *x ^
      (ROTATE_LEFT(*y, 1, SIMON_64_128_WORD_DSIZE) &
       ROTATE_LEFT(*y, 8, SIMON_64_128_WORD_DSIZE)) ^
      ROTATE_LEFT(*y, 2, SIMON_64_128_WORD_DSIZE) ^ round_key;
    *x = tmp;
  }

  void enc_round_128_128(SIMON_128_128_WORD_DTYPE roundKey,
                         SIMON_128_128_WORD_DTYPE *x,
                         SIMON_128_128_WORD_DTYPE *y) {
    SIMON_128_128_WORD_DTYPE tmp = *x;
    *x = *y ^
      (ROTATE_LEFT(*x, 1, SIMON_128_128_WORD_DSIZE) &
       ROTATE_LEFT(*x, 8, SIMON_128_128_WORD_DSIZE)) ^
      ROTATE_LEFT(*x, 2, SIMON_128_128_WORD_DSIZE) ^ roundKey;
    *y = tmp;
  }

  void dec_round_128_128(SIMON_128_128_WORD_DTYPE round_key,
                        SIMON_128_128_WORD_DTYPE *x,
                        SIMON_128_128_WORD_DTYPE *y) {
    SIMON_128_128_WORD_DTYPE tmp = *y;
    *y = *x ^
      (ROTATE_LEFT(*y, 1, SIMON_128_128_WORD_DSIZE) &
       ROTATE_LEFT(*y, 8, SIMON_128_128_WORD_DSIZE)) ^
      ROTATE_LEFT(*y, 2, SIMON_128_128_WORD_DSIZE) ^ round_key;
    *x = tmp;
  }

  uint8_t get_mode(void) {
    return (this->flags & SIMON_INTERNAL_MODE_MASK) >> SIMON_INTERNAL_MODE_OFFSET;
  }

  void debug_print(std::string str) {
    fprintf(stderr, "SIMON_ROCC: %s\n", str.c_str());
  }

};


REGISTER_EXTENSION(simon_rocc, []() { return new crypto_rocc_t; })

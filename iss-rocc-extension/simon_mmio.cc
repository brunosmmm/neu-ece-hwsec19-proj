#include <cstdio>
#include <cstring>
#include "mmio_plugin.h"
#include "simon_base.hh"
#include "processor.h"

extern "C" {
#include "simon_primitives.h"
}

#define SIMON_MMIO_REG_SCONF 0x0
#define SIMON_MMIO_REG_KEY1  0x8
#define SIMON_MMIO_REG_KEY2  0x10
#define SIMON_MMIO_REG_DATA1 0x18
#define SIMON_MMIO_REG_DATA2 0x20

#define SIMON_MMIO_CONF_WR_MASK 0xFF

// not very useful because the simulator has no timing?
// usage as follows
// 1. write key words to KEY1/KEY2
// 2. write MODE and OP bits to SCONF
// 3. write data to DATA1/DATA2
// 4. poll for completion
// 5. read data from DATA1/DATA2
// 6. rinse, repeat

class simon_mmio: public simon_base_hwacc {
public:
  simon_mmio(const std::string &args) {}

  ~simon_mmio() {}

  bool load(reg_t addr, size_t len, uint8_t *bytes) {
    switch(addr) {
    case SIMON_MMIO_REG_SCONF:
      memcpy(bytes, &this->flags, len);
      break;
    case SIMON_MMIO_REG_DATA1:
      memcpy(bytes, &(this->data_regs[0]), len);
      break;
    case SIMON_MMIO_REG_DATA2:
      memcpy(bytes, &(this->data_regs[1]), len);
      break;
    case SIMON_MMIO_REG_KEY1:
    case SIMON_MMIO_REG_KEY2:
    default:
      return false;
    }
    return true;
  }

  bool store(reg_t addr, size_t len, const uint8_t *bytes) {
    switch(addr) {
    case SIMON_MMIO_REG_KEY1:
      memcpy(&(this->key_words[0]), bytes, len);
      break;
    case SIMON_MMIO_REG_KEY2:
      memcpy(&(this->key_words[1]), bytes, len);
      break;
    case SIMON_MMIO_REG_DATA1:
      memcpy(&(this->data_regs[0]), bytes, len);
      break;
    case SIMON_MMIO_REG_DATA2:
      memcpy(&(this->data_regs[1]), bytes, len);
      if (this->flags & SIMON_FLAG_INITIALIZED) {
        // trigger new enc/dec automatically
        if (this->last_op == FUNC_ENC_ROUND) {
          do_enc_round();
        } else {
          do_dec_round();
        }
      }
      break;
    case SIMON_MMIO_REG_SCONF:
      {
        uint64_t masked = 0;
        uint8_t op = 0;
        memcpy(&masked, bytes, len);
        masked &= SIMON_MMIO_CONF_WR_MASK;
        op = (masked & SIMON_FUNCT_OP_MASK) >> SIMON_FUNCT_OP_OFFSET;
        // trigger execution here
        switch(op) {
        case FUNC_INIT:
          {
            uint8_t mode = (masked & SIMON_FUNCT_MODE_MASK);
            if ((mode == SIMON_64_128_MODE) || (mode == SIMON_128_128_MODE)) {
              // store mode
              this->flags = ((uint64_t)mode << SIMON_INTERNAL_MODE_OFFSET);
              // key words (64 bits) from registers passed in
              this->initialize(this->key_words[0], this->key_words[1]);
            }
          }
          break;
        case FUNC_ENC_ROUND:
          if (!(this->flags & SIMON_FLAG_INITIALIZED)) {
            break;
          }
          do_enc_round();
          this->last_op = FUNC_ENC_ROUND;
          break;
        case FUNC_DEC_ROUND:
          if (!(this->flags & SIMON_FLAG_INITIALIZED)) {
            break;
          }
          do_dec_round();
          this->last_op = FUNC_DEC_ROUND;
        default:
          break;
        }
        break;
      }
    default:
      return false;
    }

    if (this->get_mode() == SIMON_64_128_MODE) {
      if (this->round_counter == SIMON_64_128_ROUNDS) {
        this->flags |= SIMON_FLAG_READY;
        this->round_counter = 0;
      }
    } else if (this->get_mode() == SIMON_128_128_MODE) {
      if (this->round_counter == SIMON_64_128_ROUNDS) {
        this->flags |= SIMON_FLAG_READY;
        this->round_counter = 0;
      }
    }
    return true;
  }
private:
  uint64_t data_regs[2];
  uint64_t key_words[2];
  uint8_t last_op;

  void do_enc_round(void) {
    if (this->get_mode() == SIMON_64_128_MODE) {
      // only one register needed
      SIMON_64_128_WORD_DTYPE x, y;
      x = ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[0])[0];
      y = ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[0])[1];
      _simon_64_128_enc_round(this->round_keys.k64_128[this->round_counter++],
                              &x, &y);
      ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[0])[0] = x;
      ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[0])[1] = y;
    } else {
      // two registers needed
      _simon_128_128_enc_round(this->round_keys.k128_128[this->round_counter++],
                               &this->data_regs[0], &this->data_regs[1]);
    }
  }

  void do_dec_round(void) {
    if (this->get_mode() == SIMON_64_128_MODE) {
      SIMON_64_128_WORD_DTYPE x, y;
      x = ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[0])[0];
      y = ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[0])[1];
      _simon_64_128_dec_round(
          this->round_keys
              .k64_128[(SIMON_64_128_ROUNDS - 1) - this->round_counter++],
          &x, &y);
      ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[0])[0] = x;
      ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[0])[1] = y;
    } else {
      // two registers needed
      _simon_128_128_dec_round(
          this->round_keys
              .k128_128[(SIMON_128_128_ROUNDS - 1) - this->round_counter++],
          &this->data_regs[0], &this->data_regs[1]);
    }
  }
};

static mmio_plugin_registration_t<simon_mmio> simon_mmio_plugin_registration("simon_mmio");

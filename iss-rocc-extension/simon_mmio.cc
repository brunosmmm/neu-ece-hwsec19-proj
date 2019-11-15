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

#define SIMON_MMIO_SCONF_MODE (1<<0)
#define SIMON_MMIO_SCONF_ENCDEC (1<<1)
#define SIMON_MMIO_SCONF_SINGLE (1<<2)
#define SIMON_MMIO_SCONF_INIT (1<<3)
#define SIMON_MMIO_SCONF_READY (1<<4)

#define SIMON_OP_ENC 1
#define SIMON_OP_DEC 0

#define SIMON_MMIO_CONF_WR_MASK (SIMON_MMIO_SCONF_MODE|SIMON_MMIO_SCONF_ENCDEC|SIMON_MMIO_SCONF_SINGLE)

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

  ~simon_mmio() { this->enc_dec = SIMON_OP_DEC; this->single = false; }

  bool load(reg_t addr, size_t len, uint8_t *bytes) {
    reg_t reg_val = this->get_sconf_reg();
    switch(addr) {
    case SIMON_MMIO_REG_SCONF:
      memcpy(bytes, &reg_val, len);
      break;
    case SIMON_MMIO_REG_DATA1:
      memcpy(bytes, &(this->data_regs[0]), len);
      break;
    case SIMON_MMIO_REG_DATA2:
      memcpy(bytes, &(this->data_regs[1]), len);
      break;
    case SIMON_MMIO_REG_KEY1:
    case SIMON_MMIO_REG_KEY2:
      memset(bytes, 0, len);
      break;
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
      // auto initialize
      this->initialize(this->key_words[0], this->key_words[1]);
      break;
    case SIMON_MMIO_REG_DATA1:
      memcpy(&(this->data_regs[0]), bytes, len);
      break;
    case SIMON_MMIO_REG_DATA2:
      memcpy(&(this->data_regs[1]), bytes, len);
      if (this->flags & SIMON_FLAG_INITIALIZED) {
        // trigger new enc/dec automatically
        if (this->enc_dec == SIMON_OP_ENC) {
          if (this->single) {
            do_enc_round();
          } else {
            do_full_enc();
          }
        } else {
          if (this->single) {
            do_dec_round();
          }
          else {
            do_full_dec();
          }
        }
      }
      break;
    case SIMON_MMIO_REG_SCONF:
      {
        uint64_t masked = 0;
        memcpy(&masked, bytes, len);
        masked &= SIMON_MMIO_CONF_WR_MASK;
        if (masked & SIMON_MMIO_SCONF_MODE) {
          this->set_mode(SIMON_128_128_MODE);
        } else {
          this->set_mode(SIMON_64_128_MODE);
        }
        if (masked & SIMON_MMIO_SCONF_ENCDEC) {
          this->enc_dec = SIMON_OP_ENC;
        } else {
          this->enc_dec = SIMON_OP_DEC;
        }
        this->single = (masked & SIMON_MMIO_SCONF_SINGLE) ? true: false;
        break;
      }
    default:
      return false;
    }

    return true;
  }
private:
  uint64_t data_regs[2];
  uint64_t key_words[2];
  uint8_t enc_dec;
  bool single;

  void do_full_enc(void) {
    unsigned int i = 0;
    uint32_t rounds = 0;
    if (this->get_mode() == SIMON_64_128_MODE) {
      rounds = SIMON_64_128_ROUNDS;
    }
    else {
      rounds = SIMON_128_128_ROUNDS;
    }

    for(i=0;i<rounds;i++) {
      do_enc_round();
    }
  }

  void do_full_dec(void) {
    unsigned int i = 0;
    uint32_t rounds = 0;
    if (this->get_mode() == SIMON_64_128_MODE) {
      rounds = SIMON_64_128_ROUNDS;
    } else {
      rounds = SIMON_128_128_ROUNDS;
    }

    for (i = 0; i < rounds; i++) {
      do_dec_round();
    }
  }

  void do_enc_round(void) {
    if (this->get_mode() == SIMON_64_128_MODE) {
      // only one register needed
      SIMON_64_128_WORD_DTYPE x, y;
      x = ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[0])[0];
      y = ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[1])[0];
      _simon_64_128_enc_round(this->round_keys.k64_128[this->round_counter++],
                              &x, &y);
      ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[0])[0] = x;
      ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[1])[0] = y;
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
      y = ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[1])[0];
      _simon_64_128_dec_round(
          this->round_keys
              .k64_128[(SIMON_64_128_ROUNDS - 1) - this->round_counter++],
          &x, &y);
      ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[0])[0] = x;
      ((SIMON_64_128_WORD_DTYPE *)&this->data_regs[1])[0] = y;
    } else {
      // two registers needed
      _simon_128_128_dec_round(
          this->round_keys
              .k128_128[(SIMON_128_128_ROUNDS - 1) - this->round_counter++],
          &this->data_regs[0], &this->data_regs[1]);
    }
  }

  reg_t get_sconf_reg(void) {
    reg_t reg_val = 0;
    if (this->flags & SIMON_FLAG_INITIALIZED) {
      reg_val |= SIMON_MMIO_SCONF_INIT;
      // no timing, so always ready if initialized
      reg_val |= SIMON_MMIO_SCONF_READY;
    }
    if (this->flags & SIMON_FLAG_READY) {
      reg_val |= SIMON_MMIO_SCONF_READY;
    }
    if (this->get_mode() == SIMON_128_128_MODE) {
      reg_val |= SIMON_MMIO_SCONF_MODE;
    }
    if (this->single) {
      reg_val |= SIMON_MMIO_SCONF_SINGLE;
    }
    return reg_val;
  }
};

static mmio_plugin_registration_t<simon_mmio> simon_mmio_plugin_registration("simon_mmio");

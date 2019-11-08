#include "rocc.h"
#include "salsa20_base.hh"
#include <cstdlib>
#include <cstring>
#include <string>

extern "C" {
#include "salsa20_primitives.h"
}

class salsa20_rocc_t: public rocc_t, public salsa20_base_hwacc {
public:
  const char* name() { return "salsa20_rocc"; }

  reg_t custom0(rocc_insn_t insn, reg_t xs1, reg_t xs2) {
    reg_t ret_val = 0;
    switch(insn.funct) {
    case SALSA20_FUNC_SET_KEY1:
      // set two of the key words
      this->setkey_low(xs1, xs2);
      this->reset_pos();
      break;
    case SALSA20_FUNC_SET_KEY2:
      this->setkey_high(xs1, xs2);
      this->reset_pos();
      break;
    case SALSA20_FUNC_SET_NONCE:
      this->set_nonce(xs1);
      this->reset_pos();
      break;
    case SALSA20_FUNC_ENC_DEC:
      this->encrypt_decrypt((uint8_t*)&xs1, (uint8_t*)&ret_val, sizeof(reg_t));
      break;
    default:
      illegal_instruction();
      break;
    }
    return ret_val;
  }

  salsa20_rocc_t() { }
};

REGISTER_EXTENSION(salsa20_rocc, []() { return new salsa20_rocc_t; })

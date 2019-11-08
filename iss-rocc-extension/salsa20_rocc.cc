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

    return 0;
  }

  salsa20_rocc_t() { }
};

REGISTER_EXTENSION(salsa20_rocc, []() { return new salsa20_rocc_t; })

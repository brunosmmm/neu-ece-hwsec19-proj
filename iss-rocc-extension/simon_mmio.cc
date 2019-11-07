#include <cstdio>
#include <cstring>
#include "mmio_plugin.h"

extern "C" {
#include "simon_primitives.h"
}

#define SIMON_MMIO_REG_SCONF 0x0
#define SIMON_MMIO_REG_KEY1  0x4
#define SIMON_MMIO_REG_KEY2  0x8
#define SIMON_MMIO_REG_DATA1 0xC
#define SIMON_MMIO_REG_DATA2 0x10

#define SIMON_MMIO_CONF_WR_MASK 0xFF

class simon_mmio {
public:
  simon_mmio(const std::string &args) {}

  ~simon_mmio() {}

  bool load(reg_t addr, size_t len, uint8_t *bytes) {
    switch(addr) {
    default:
      memset(bytes, 0, len);
      break;
    }
    return true;
  }

  bool store(reg_t addr, size_t len, const uint8_t *bytes) {
    switch(addr) {
    default:
      break;
    }
    return true;
  }
};

static mmio_plugin_registration_t<simon_mmio> simon_mmio_plugin_registration("simon_mmio");

#include "simon/mmio.h"
#include "mmio.h"

void simon_mmio_initialize(uint8_t* key, uint8_t config, uint8_t wait) {
  uint64_t kw1, kw2;
  kw1 = ((uint64_t *)key)[0];
  kw2 = ((uint64_t *)key)[1];

  // initialize
  reg_write64(SIMON_MMIO_BASE + SIMON_MMIO_REG_SCONF,
              SIMON_MMIO_SCONF_SINGLE | SIMON_MMIO_SCONF_ENCDEC);
  reg_write64(SIMON_MMIO_BASE + SIMON_MMIO_REG_KEY1, kw1);
  reg_write64(SIMON_MMIO_BASE + SIMON_MMIO_REG_KEY2, kw2);

  // wait for completion of key expansion
  if (wait) {
    SIMON_MMIO_WAIT_READY;
  }

}

uint64_t simon_mmio_get_id(void) {
  return reg_read64(SIMON_MMIO_BASE+SIMON_MMIO_REG_ID);
}

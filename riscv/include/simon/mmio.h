#ifndef _SIMON_MMIO_H_INCLUDED_
#define _SIMON_MMIO_H_INCLUDED_

#include <stdint.h>
#include "mmio.h"

#define SIMON_MODE_64_128 0
#define SIMON_MODE_128_128 1

#define SIMON_MMIO_REG_SCONF 0x0
#define SIMON_MMIO_REG_KEY1 0x8
#define SIMON_MMIO_REG_KEY2 0x10
#define SIMON_MMIO_REG_DATA1 0x18
#define SIMON_MMIO_REG_DATA2 0x20
#define SIMON_MMIO_REG_ID 0x28

#define SIMON_MMIO_SCONF_MODE (1 << 0)
#define SIMON_MMIO_SCONF_ENCDEC (1 << 1)
#define SIMON_MMIO_SCONF_SINGLE (1 << 2)
#define SIMON_MMIO_SCONF_INIT (1 << 3)
#define SIMON_MMIO_SCONF_READY (1 << 4)

#define SIMON_ID 0x53494d4f4e313238
#define SIMON_MMIO_BASE 0x10000000

#define SIMON_MMIO_WAIT_INIT                                            \
  while (!(reg_read64(SIMON_MMIO_BASE + SIMON_MMIO_REG_SCONF) &         \
           SIMON_MMIO_SCONF_INIT))

#define SIMON_MMIO_WAIT_READY                                     \
    while (!(reg_read64(SIMON_MMIO_BASE + SIMON_MMIO_REG_SCONF) &\
             SIMON_MMIO_SCONF_READY))

void simon_mmio_initialize(uint8_t *key, uint8_t config, uint8_t wait);
uint64_t simon_mmio_get_id(void);

uint64_t simon_mmio_64_128_encrypt_single(uint64_t *block);
uint64_t simon_mmio_64_128_encrypt_auto(uint64_t *block);

#endif

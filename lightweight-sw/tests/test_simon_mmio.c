// Test Simon MMIO acc
#include <stdint.h>
// #include <stdio.h>
#include "mmio.h"
#include "util.h"
#include "rvutil.h"
#include "simon_primitives.h"
#include "simon/mmio.h"

#ifndef HWACC_MMIO_BASE
#define HWACC_MMIO_BASE 0x1000000
#endif

#define DEBUGSTUCK
#define MAX_WAIT 1000

#ifdef DEBUGSTUCK
#define WAIT_READY wait_ready(MAX_WAIT)
#else
#define WAIT_READY SIMON_MMIO_WAIT_READY
#endif

// A 128-bit key
const uint8_t test_key[16] = {0x80, 0x01, 0x02, 0xFF, 0x2A, 0xAA,
                              0x42, 0x00, 0x11, 0x30, 0xF9, 0xA4,
                              0xBB, 0x09, 0xAB, 0x56};

// a 64-bit plaintext block
static uint8_t test_block[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

inline static void wait_ready(int64_t max_wait) {
  unsigned int wait = max_wait;
  while (!(reg_read64(HWACC_MMIO_BASE+SIMON_MMIO_REG_SCONF)
           & SIMON_MMIO_SCONF_INIT)) {
    if (max_wait < 0) {
      continue;
    }
    if (wait == 0) {
      // stuck
      printf("DEBUG: got stuck while waiting for MMIO device!\n");
      exit(1);
    }
    wait--;
  }
}

int main(void) {
  uint64_t kw1 = 0, kw2 = 0, block = 0, cipher = 0, tmp = 0;
  uint64_t cycles = 0;
  unsigned int i = 0;

  // arrange key words from array
  kw1 = ((uint64_t*)test_key)[0];
  kw2 = ((uint64_t*)test_key)[1];

  cycles = rv64_get_cycles();

  // initialize
  reg_write64(HWACC_MMIO_BASE+SIMON_MMIO_REG_SCONF,
              SIMON_MMIO_SCONF_SINGLE|SIMON_MMIO_SCONF_ENCDEC);
  reg_write64(HWACC_MMIO_BASE+SIMON_MMIO_REG_KEY1, kw1);
  reg_write64(HWACC_MMIO_BASE+SIMON_MMIO_REG_KEY2, kw2);

  // wait for completion of key expansion
  SIMON_MMIO_WAIT_INIT;

  // load test block; immediately performs one round
  reg_write64(HWACC_MMIO_BASE+SIMON_MMIO_REG_DATA1, ((uint32_t*)test_block)[0]);
  reg_write64(HWACC_MMIO_BASE+SIMON_MMIO_REG_DATA2, ((uint32_t*)test_block)[1]);

  // perform encryption rounds
  for (i=0; i<SIMON_64_128_ROUNDS-1; i++) {
    // start next round
    WAIT_READY;
    tmp = reg_read64(HWACC_MMIO_BASE+SIMON_MMIO_REG_DATA2);
    reg_write64(HWACC_MMIO_BASE+SIMON_MMIO_REG_DATA2, tmp);
  }

  while (!(reg_read64(HWACC_MMIO_BASE+SIMON_MMIO_REG_SCONF)
           & SIMON_MMIO_SCONF_READY));

  cipher = reg_read64(HWACC_MMIO_BASE+SIMON_MMIO_REG_DATA1) |
    reg_read64(HWACC_MMIO_BASE+SIMON_MMIO_REG_DATA2)<<32;

  // set to decryption mode
  tmp = reg_read64(HWACC_MMIO_BASE+SIMON_MMIO_REG_SCONF);
  tmp &= ~SIMON_MMIO_SCONF_ENCDEC;
  reg_write64(HWACC_MMIO_BASE+SIMON_MMIO_REG_SCONF, tmp);

  // perform decryption rounds
  for (i=0; i<SIMON_64_128_ROUNDS; i++) {
    WAIT_READY;
    tmp = reg_read64(HWACC_MMIO_BASE+SIMON_MMIO_REG_DATA2);
    reg_write64(HWACC_MMIO_BASE+SIMON_MMIO_REG_DATA2, tmp);
  }

  block = reg_read64(HWACC_MMIO_BASE+SIMON_MMIO_REG_DATA1)
    | reg_read64(HWACC_MMIO_BASE+SIMON_MMIO_REG_DATA2)<<32;

  cycles = rv64_get_cycles() - cycles;

  printf("INFO: ciphertext is 0x%lx\n", cipher);

  // verify operation
  if (*((uint64_t*)test_block) != block) {
    // fail
    printf("ERROR: validation failed\n");
    printf("expected: 0x%lx, got 0x%lx\n", *((uint64_t*)test_block), block);
    return 1;
  }

  printf("INFO: test succeeded. Cycles = %lu\n", cycles);

  return 0;
}

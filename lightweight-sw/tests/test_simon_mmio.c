// Test Simon RoCC
#include <stdint.h>
// #include <stdio.h>
#include "mmio.h"
#include "util.h"
#include "riscv/rvutil.h"

#define ROCC_FUNC_OP_OFFSET 2
#define ROCC_FUNC_MODE_OFFSET 0

#define ROCC_FUNC_INIT (0<<ROCC_FUNC_OP_OFFSET)
#define ROCC_FUNC_ENC (1<<ROCC_FUNC_OP_OFFSET)
#define ROCC_FUNC_DEC (2<<ROCC_FUNC_OP_OFFSET)
#define ROCC_FUNC_FLAGS (3<<ROCC_FUNC_OP_OFFSET)
#define ROCC_FUNC_HWORD (4<<ROCC_FUNC_OP_OFFSET)

#define ROCC_MODE_64_128 0
#define ROCC_MODE_128_128 1

#define SIMON_64_128_ROUNDS 44
#define SIMON_128_128_ROUNDS 68

#define SIMON_MMIO_REG_SCONF 0x0
#define SIMON_MMIO_REG_KEY1 0x8
#define SIMON_MMIO_REG_KEY2 0x10
#define SIMON_MMIO_REG_DATA1 0x18
#define SIMON_MMIO_REG_DATA2 0x20

#define SIMON_MMIO_BASE 0x10000000

// A 128-bit key
const uint8_t test_key[16] = {0x80, 0x01, 0x02, 0xFF, 0x2A, 0xAA,
                              0x42, 0x00, 0x11, 0x30, 0xF9, 0xA4,
                              0xBB, 0x09, 0xAB, 0x56};

// a 64-bit plaintext block
static uint8_t test_block[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};


int main(void) {
  uint64_t kw1 = 0, kw2 = 0, block = 0, cipher = 0;
  uint64_t cycles = 0;
  unsigned int i = 0;

  // arrange key words from array
  kw1 = ((uint64_t*)test_key)[0];
  kw2 = ((uint64_t*)test_key)[1];

  cycles = rv64_get_cycles();

  // initialize
  reg_write64(SIMON_MMIO_BASE+SIMON_MMIO_REG_KEY1, kw1);
  reg_write64(SIMON_MMIO_BASE+SIMON_MMIO_REG_KEY2, kw2);
  reg_write64(SIMON_MMIO_BASE+SIMON_MMIO_REG_SCONF, ROCC_FUNC_INIT);

  // load test block
  reg_write64(SIMON_MMIO_BASE+SIMON_MMIO_REG_DATA1, *((uint64_t*)test_block));

  // perform encryption rounds
  for (i=0; i<SIMON_64_128_ROUNDS; i++) {
    reg_write64(SIMON_MMIO_BASE+SIMON_MMIO_REG_SCONF, ROCC_FUNC_ENC);
  }

  cipher = reg_read64(SIMON_MMIO_BASE+SIMON_MMIO_REG_DATA1);

  // perform decryption rounds
  for (i=0; i<SIMON_64_128_ROUNDS; i++) {
    reg_write64(SIMON_MMIO_BASE+SIMON_MMIO_REG_SCONF, ROCC_FUNC_DEC);
  }

  block = reg_read64(SIMON_MMIO_BASE+SIMON_MMIO_REG_DATA1);

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

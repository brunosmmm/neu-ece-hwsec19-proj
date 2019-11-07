// Test Simon RoCC
#include <stdint.h>
#include <stdio.h>
#include "xcustom.h"
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

#define SIMON_MMIO_REG_SCONF 0x0
#define SIMON_MMIO_REG_KEY1 0x8
#define SIMON_MMIO_REG_KEY2 0x10
#define SIMON_MMIO_REG_DATA1 0x18
#define SIMON_MMIO_REG_DATA2 0x20

#define SIMON_MMIO_BASE 0x10000000
static uint64_t* simon_base = (uint64_t*)SIMON_MMIO_BASE;

static void regwrite(uint64_t* base, uint64_t offset, uint64_t data) {
  *(uint64_t*)((((uint8_t*)base)+offset)) = data;
}

static uint64_t regread(uint64_t* base, uint64_t offset) {
  return *(uint64_t*)(((uint8_t*)base)+offset);
}

// A 128-bit key
const static uint8_t test_key[16] = {0x80, 0x01, 0x02, 0xFF, 0x2A, 0xAA,
                                     0x42, 0x00, 0x11, 0x30, 0xF9, 0xA4,
                                     0xBB, 0x09, 0xAB, 0x56};

// a 64-bit plaintext block
static uint8_t test_block[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};


int main(void) {
  uint64_t rounds = 0, kw1 = 0, kw2 = 0, block = 0, cipher = 0;
  uint64_t cycles = 0, time = 0, instret = 0;
  unsigned int i = 0;

  // arrange key words from array
  kw1 = ((uint64_t*)test_key)[0];
  kw2 = ((uint64_t*)test_key)[1];

  cycles = rv64_get_cycles();

  // initialize
  regwrite(simon_base, SIMON_MMIO_REG_KEY1, kw1);
  regwrite(simon_base, SIMON_MMIO_REG_KEY2, kw2);
  regwrite(simon_base, SIMON_MMIO_REG_SCONF, ROCC_FUNC_INIT);

  // load test block
  regwrite(simon_base, SIMON_MMIO_REG_DATA1, *((uint64_t*)test_block));

  // perform encryption rounds
  for (i=0; i<rounds; i++) {
    regwrite(simon_base, SIMON_MMIO_REG_SCONF, ROCC_FUNC_ENC);
  }

  cipher = regread(simon_base, SIMON_MMIO_REG_DATA1);

  // perform decryption rounds
  for (i=0; i<rounds; i++) {
    regwrite(simon_base, SIMON_MMIO_REG_SCONF, ROCC_FUNC_DEC);
  }

  block = regread(simon_base, SIMON_MMIO_REG_DATA1);

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

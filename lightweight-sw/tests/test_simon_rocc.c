// Test Simon RoCC
#include <stdint.h>
#ifndef BAREMETAL
#include <stdio.h>
#endif
#include "rocc.h"
#include "rvutil.h"
#include "simon/rocc.h"

#define SIMON_64_128_ROUNDS 44

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
  ROCC_INSTRUCTION_SS(0, kw1, kw2, ROCC_FUNC_INIT);

  // load test block
  block = *((uint64_t*)test_block);

  // perform encryption rounds
  for (i=0; i<SIMON_64_128_ROUNDS; i++) {
    ROCC_INSTRUCTION_DSS(0, block, block, 0, ROCC_FUNC_ENC);
  }

  cipher = block;

  // perform decryption rounds
  for (i=0; i<SIMON_64_128_ROUNDS; i++) {
    ROCC_INSTRUCTION_DSS(0, block, block, 0, ROCC_FUNC_DEC);
  }

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

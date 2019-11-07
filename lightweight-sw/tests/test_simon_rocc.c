// Test Simon RoCC
#include <stdint.h>
#include <stdio.h>
#include "xcustom.h"

#define ROCC_FUNC_OP_OFFSET 2
#define ROCC_FUNC_MODE_OFFSET 0

#define ROCC_FUNC_INIT (0<<ROCC_FUNC_OP_OFFSET)
#define ROCC_FUNC_ENC (1<<ROCC_FUNC_OP_OFFSET)
#define ROCC_FUNC_DEC (2<<ROCC_FUNC_OP_OFFSET)
#define ROCC_FUNC_FLAGS (3<<ROCC_FUNC_OP_OFFSET)
#define ROCC_FUNC_HWORD (4<<ROCC_FUNC_OP_OFFSET)

#define ROCC_MODE_64_128 0
#define ROCC_MODE_128_128 1

// A 128-bit key
const static uint8_t test_key[16] = {0x80, 0x01, 0x02, 0xFF, 0x2A, 0xAA,
                                     0x42, 0x00, 0x11, 0x30, 0xF9, 0xA4,
                                     0xBB, 0x09, 0xAB, 0x56};

// a 64-bit plaintext block
static uint8_t test_block[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

int main(void) {
  uint64_t rounds = 0, kw1 = 0, kw2 = 0, block = 0;
  unsigned int i = 0;

  // arrange key words from array
  kw1 = ((uint64_t*)test_key)[0];
  kw2 = ((uint64_t*)test_key)[1];

  // initialize
  ROCC_INSTRUCTION(0, rounds, kw1, kw2, ROCC_FUNC_INIT);

  // load test block
  block = *((uint64_t*)test_block);

  // perform encryption rounds
  for (i=0; i<rounds; i++) {
    ROCC_INSTRUCTION(0, block, block, 0, ROCC_FUNC_ENC);
  }

  // perform decryption rounds
  for (i=0; i<rounds; i++) {
    ROCC_INSTRUCTION(0, block, block, 0, ROCC_FUNC_DEC);
  }

  // verify operation
  if (*((uint64_t*)test_block) != block) {
    // fail
    printf("ERROR: validation failed\n");
    return 1;
  }

  return 0;
}

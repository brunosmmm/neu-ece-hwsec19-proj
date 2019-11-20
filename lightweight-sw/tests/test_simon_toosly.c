// Test Simon RoCC
#include <stdint.h>
#ifndef BAREMETAL
#include <stdio.h>
#endif
#include "riscv/simon_toosly.h"
#include "riscv/rvutil.h"


// A 128-bit key
const static uint8_t test_key[16] = {0x80, 0x01, 0x02, 0xFF, 0x2A, 0xAA,
                                     0x42, 0x00, 0x11, 0x30, 0xF9, 0xA4,
                                     0xBB, 0x09, 0xAB, 0x56};

// a 64-bit plaintext block
static uint8_t test_block[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};


int main(void) {
  uint64_t kw1 = 0, kw2 = 0, block = 0, cipher = 0;
  uint64_t cycles = 0;

  // arrange key words from array
  kw1 = ((uint64_t*)test_key)[0];
  kw2 = ((uint64_t*)test_key)[1];

  cycles = rv64_get_cycles();

  // initialize
  TOOSLY_INIT(kw1, kw2);

  // load test block
  block = *((uint64_t*)test_block);

  // perform encryption rounds
  TOOSLY_ENC(&block, 1);

  asm volatile ("fence");
  cipher = block;

  // perform decryption rounds
  TOOSLY_DEC(&block, 1);

  asm volatile ("fence");
  cycles = rv64_get_cycles() - cycles;

  printf("INFO: ciphertext is 0x%lx\n", cipher);

  // verify operation
  if (*((uint64_t*)test_block) != block) {
    // fail
    printf("ERROR: validation failed\n");
    printf("expected: 0x%lx, got 0x%lx\n", *((uint64_t*)test_block), block);
    return 1;
  }

  printf("INFO: test 1 succeeded. Cycles = %lu\n", cycles);

  // test encrypted load/store
  TOOSLY_CIPHER_STORE(&cipher, *((uint64_t*)test_block));
  asm volatile("fence");

  TOOSLY_CIPHER_LOAD(&cipher, block);
  asm volatile("fence");

  if (*((uint64_t *)test_block) != block) {
    // fail
    printf("ERROR: validation failed\n");
    printf("expected: 0x%lx, got 0x%lx\n", *((uint64_t *)test_block), block);
    return 1;
  }

  printf("INFO: test 2 succeeded.\n");

  return 0;
}

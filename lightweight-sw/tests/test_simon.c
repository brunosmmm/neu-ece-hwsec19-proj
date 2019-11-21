#include "simon.h"
#include <stdint.h>

#ifdef RISCV64
#include "rvutil.h"
#endif

#ifndef BAREMETAL
#include "util.h"
#endif

// #define DEBUG

const static key128_t test_key = {0x80, 0x01, 0x02, 0xFF, 0x2A, 0xAA, 0x42, 0x00,
                                  0x11, 0x30, 0xF9, 0xA4, 0xBB, 0x09, 0xAB, 0x56};

static block64_t test_block = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

int main(void) {
  unsigned int i = 0;
  block64_t tmp, out, cipher;

#ifdef BAREMETAL
#ifdef DEBUG
  volatile int wait = 1;
  while (wait);
#endif
#endif

#ifdef RISCV64
  uint64_t rv64_cycles = rv64_get_cycles();
#endif

  // initialize
  simon_64_128_initialize((uint8_t*)test_key);

  // encrypt
  simon_64_128_encrypt(&test_block, &tmp);

  // save ciphertext
  *((uint64_t*)cipher) = *((uint64_t*)tmp);

  // decrypt
  simon_64_128_decrypt(&tmp, &out);

#ifdef RISCV64
  rv64_cycles = rv64_get_cycles() - rv64_cycles;
#endif

  // display ciphertext
#ifndef BAREMETAL
  print_bytes((uint8_t *)cipher, sizeof(block64_t));

  // display plaintext
  print_bytes((uint8_t*)out, sizeof(block64_t));
#else
  printf("INFO: ciphertext is 0x%lx\n", *((uint64_t*)cipher));
#endif

  // verify
  for (i=0; i<sizeof(block64_t); i++) {
    if (((uint8_t*)test_block)[i] != ((uint8_t*)out)[i]) {
      // incorrect value!
      printf("ERROR: decrypted block does not match original plaintext\n");
      return 1;
    }
  }
#ifdef RISCV64
  printf("INFO: test succeded. Cycles = %lu\n", rv64_cycles);
#else
  printf("INFO: test succeeded\n");
#endif
  return 0;
}

#include "salsa20.h"
#include "util.h"
#include <stdint.h>

#ifdef RISCV64
#include "rvutil.h"
#endif

static key256_t test_key =
  {
   0x80, 0x01, 0x02, 0xFF, 0x2A, 0xAA, 0x42, 0x00,
   0x11, 0x30, 0xF9, 0xA4, 0xBB, 0x09, 0xAB, 0x56,
   0x80, 0x01, 0x02, 0xFF, 0x2A, 0xAA, 0x42, 0x00,
   0x11, 0x30, 0xF9, 0xA4, 0xBB, 0x09, 0xAB, 0x56
  };

static key64_t test_nonce =
  {
   0xff, 0x05, 0x76, 0x32, 0xf0, 0x21, 0x45, 0xab
  };

static block64_t test_block = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

int main(void) {
  unsigned int i = 0;
  block64_t tmp, cipher, out;

#ifdef RISCV64
  uint64_t rv64_cycles = rv64_get_cycles();
#endif

  // initialize
  salsa20_init(test_key, test_nonce);

  // encrypt
  salsa20_encrypt_decrypt((uint8_t*)test_block, (uint8_t*)tmp, 8);

  // save ciphertext
  *((uint64_t *)cipher) = *((uint64_t *)tmp);

  // decrypt (must re-initialize)
  salsa20_init(test_key, test_nonce);
  salsa20_encrypt_decrypt((uint8_t*)tmp, (uint8_t*)out, 8);

#ifdef RISCV64
  rv64_cycles = rv64_get_cycles() - rv64_cycles;
#endif

  // display ciphertext
  print_bytes((uint8_t *)cipher, sizeof(block64_t));

  // display plaintext
  print_bytes((uint8_t *)out, sizeof(block64_t));

  for (i = 0; i < sizeof(block64_t); i++) {
    if (((uint8_t *)test_block)[i] != ((uint8_t *)out)[i]) {
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
}

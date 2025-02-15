#include "speck.h"
#include "util.h"
#include <stdint.h>

const static key128_t test_key = {0x80, 0x01, 0x02, 0xFF, 0x2A, 0xAA,
                                  0x42, 0x00, 0x11, 0x30, 0xF9, 0xA4,
                                  0xBB, 0x09, 0xAB, 0x56};

static block64_t test_block = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

int main(void) {
  unsigned int i = 0;
  block64_t tmp, out;
  // initialize
  speck_64_128_initialize((uint8_t *)test_key);

  // encrypt
  speck_64_128_encrypt(&test_block, &tmp);

  // display ciphertext
  print_bytes((uint8_t *)tmp, sizeof(block64_t));

  // decrypt
  speck_64_128_decrypt(&tmp, &out);

  // display plaintext
  print_bytes((uint8_t *)out, sizeof(block64_t));

  // verify
  for (i = 0; i < sizeof(block64_t); i++) {
    if (((uint8_t *)test_block)[i] != ((uint8_t *)out)[i]) {
      // incorrect value!
      printf("ERROR: decrypted block does not match original plaintext\n");
      return 1;
    }
  }
  printf("INFO: test succeeded\n");
  return 0;
}

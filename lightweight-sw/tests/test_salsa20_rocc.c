#include "salsa20.h"
#include "util.h"
#include <stdint.h>
#include "riscv/rvutil.h"
#include "rocc.h"

#define SALSA20_FUNC_SET_KEY1 0x0
#define SALSA20_FUNC_SET_KEY2 0x1
#define SALSA20_FUNC_SET_NONCE 0x2
#define SALSA20_FUNC_ENC_DEC 0x3

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
  uint64_t tmp, cipher;
  uint64_t rv64_cycles = 0, ret = 0;
  uint64_t* key = (uint64_t*)test_key;

  rv64_cycles = rv64_get_cycles();

  // initialize
  ROCC_INSTRUCTION_DSS(0, ret, key[1], key[0], SALSA20_FUNC_SET_KEY1);
  ROCC_INSTRUCTION_DSS(0, ret, key[2], key[3], SALSA20_FUNC_SET_KEY2);
  ROCC_INSTRUCTION_DSS(0, ret, test_nonce, 0, SALSA20_FUNC_SET_NONCE);

  tmp = *((uint64_t*)test_block);

  // encrypt
  ROCC_INSTRUCTION_DSS(0, tmp, tmp, 0, SALSA20_FUNC_ENC_DEC);

  // save ciphertext
  cipher = tmp;

  // decrypt (must re-initialize)
  ROCC_INSTRUCTION_DSS(0, ret, test_nonce, 0, SALSA20_FUNC_SET_NONCE);
  ROCC_INSTRUCTION_DSS(0, tmp, tmp, 0, SALSA20_FUNC_ENC_DEC);

  rv64_cycles = rv64_get_cycles() - rv64_cycles;

  // display ciphertext
  print_bytes((uint8_t *)&cipher, sizeof(block64_t));

  // display plaintext
  print_bytes((uint8_t *)&tmp, sizeof(block64_t));

  for (i = 0; i < sizeof(block64_t); i++) {
    if (((uint8_t *)test_block)[i] != ((uint8_t *)&tmp)[i]) {
      // incorrect value!
      printf("ERROR: decrypted block does not match original plaintext\n");
      return 1;
    }
  }
  printf("INFO: test succeded. Cycles = %lu\n", rv64_cycles);

  return 0;
}

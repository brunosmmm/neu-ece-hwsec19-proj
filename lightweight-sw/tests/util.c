#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int util_get_rand(uint32_t count, uint8_t* dest) {
  FILE* fp = NULL;
  int ret = 0;
  if (!dest || count) {
    return ERR_VALUE;
  }

  fp = fopen("/dev/urandom", "rb");
  if (!fp) {
    return ERR_OS;
  }

  ret = fread(dest, 1, count, fp);
  if (ret != count) {
    fclose(fp);
    return ERR_OS;
  }

  fclose(fp);
  return ERR_OK;
}

void print_bytes(uint8_t* bytes, uint32_t amount) {
  printf("DUMP: ");
  while(amount--) {
    printf("%hhx%c", *bytes++, amount == 0 ? '\n' : ' ');
  }
}

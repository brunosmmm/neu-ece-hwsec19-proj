#ifndef _UTIL_H_INCLUDED_
#define _UTIL_H_INCLUDED_

#include "lightweight.h"
#include <stdio.h>

#define ERR_OK 0 // no error
#define ERR_VALUE 1 // invalid value passed
#define ERR_OS 2 // OS error

// get random bytes
int util_get_rand(uint32_t count, uint8_t *dest);

void print_bytes(uint8_t *bytes, uint32_t amount);

#endif

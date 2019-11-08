#ifndef _LIGHTWEIGHT_H_INCLUDED_
#define _LIGHTWEIGHT_H_INCLUDED_

#include <stdint.h>
// key types
typedef uint8_t key64_t[8];
typedef uint8_t key128_t[16];
typedef uint8_t key256_t[32];

// block types
typedef uint8_t block128_t[16];
typedef uint8_t block64_t[8];

// common error codes
#define CIPHER_ERR_OK 0
#define CIPHER_ERR_ENC 1
#define CIPHER_ERR_DEC 2

#endif

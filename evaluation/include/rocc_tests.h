#ifndef _ROCC_TESTS_H_INCLUDED_
#define _ROCC_TESTS_H_INCLUDED_

#include <stdint.h>

uint64_t test_rocc_ecb(unsigned int testSize, uint8_t *testData);
uint64_t test_rocc_cbc(unsigned int testSize, uint8_t *testData);

#endif

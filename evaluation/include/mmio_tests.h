#ifndef _MMIO_TESTS_H_INCLUDED_
#define _MMIO_TESTS_H_INCLUDED_

#include <stdint.h>

uint64_t test_mmio_ecb_single(unsigned int testSize, uint8_t *testData);
uint64_t test_mmio_cbc_single(unsigned int testSize, uint8_t *testData);
uint64_t test_mmio_ecb_auto(unsigned int testSize, uint8_t *testData);

#endif

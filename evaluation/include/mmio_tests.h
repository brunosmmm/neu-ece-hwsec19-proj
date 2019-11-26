#ifndef _MMIO_TESTS_H_INCLUDED_
#define _MMIO_TESTS_H_INCLUDED_

#include "common.h"

TestResult test_mmio_ecb_single(unsigned int testSize, uint8_t *testData);
TestResult test_mmio_cbc_single(unsigned int testSize, uint8_t *testData);
TestResult test_mmio_ecb_auto(unsigned int testSize, uint8_t *testData);

#endif

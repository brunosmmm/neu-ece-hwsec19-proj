#ifndef _ROCC_TESTS_H_INCLUDED_
#define _ROCC_TESTS_H_INCLUDED_

#include "common.h"

TestResult test_rocc_ecb(unsigned int testSize, uint8_t *testData);
TestResult test_rocc_cbc(unsigned int testSize, uint8_t *testData);

#endif

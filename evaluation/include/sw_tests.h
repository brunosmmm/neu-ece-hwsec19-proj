#ifndef _SW_TESTS_H_INCLUDED_
#define _SW_TESTS_H_INCLUDED_

#include "common.h"

TestResult test_sw_ecb(unsigned int testSize, uint8_t* testData);
TestResult test_sw_cbc(unsigned int testSize, uint8_t* testData);

#endif

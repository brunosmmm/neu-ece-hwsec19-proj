#ifndef _SW_TESTS_H_INCLUDED_
#define _SW_TESTS_H_INCLUDED_

#include "common.h"

uint64_t test_sw_ecb(unsigned int testSize, uint8_t* testData);
uint64_t test_sw_cbc(unsigned int testSize, uint8_t* testData);

#endif
